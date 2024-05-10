// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingRocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "FsmComponent.h"
#include "EnemyFlyingSaucer.h"
#include "ExplosionEffect.h"
#include "Floor.h"
#include "Cody.h"
#include "PlayerBase.h"

// Sets default values
AHomingRocket::AHomingRocket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);
		Tags.Add(FName("HomingRocket"));

		SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
		SetRootComponent(SceneComp);

		RocketMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
		RocketMeshComp->SetupAttachment(SceneComp);

		FireEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireEffectComponent"));
		FireEffectComp->SetupAttachment(SceneComp);

		SetupFsmComponent();
	}
}

// Called when the game starts or when spawned
void AHomingRocket::BeginPlay()
{
	Super::BeginPlay();
	if (true == HasAuthority())
	{
		RocketFsmComponent->ChangeState(ERocketState::PlayerEquipWait);
		SetupOverlapEvent();
	}
}

// Called every frame
void AHomingRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
	}
}

void AHomingRocket::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 메시 컴포넌트를 Replication하기 위한 설정 추가
	DOREPLIFETIME(AHomingRocket, FireEffectComp);
	DOREPLIFETIME(AHomingRocket, BossActor);
	DOREPLIFETIME(AHomingRocket, RocketDamageToBoss);
	DOREPLIFETIME(AHomingRocket, PlayerEquipLerpRatio);
	DOREPLIFETIME(AHomingRocket, PlayerEquipLerpStartRotation);
	DOREPLIFETIME(AHomingRocket, PlayerEquipLerpEndRotation);
	DOREPLIFETIME(AHomingRocket, PlayerEquipMaxLiveTime);
}

int32 AHomingRocket::GetCurrentState() const
{
	if (nullptr == RocketFsmComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("RocketFsmComponent is nullptr"));
	}

	return RocketFsmComponent->GetCurrentState();
}

void AHomingRocket::Multicast_SpawnDestroyEffect_Implementation()
{
	FVector EffectSettingLocation = GetActorLocation();
	AExplosionEffect* Effect = GetWorld()->SpawnActor<AExplosionEffect>(ExplosionEffectClass, EffectSettingLocation, FRotator::ZeroRotator);
}

void AHomingRocket::SetupOverlapEvent()
{
	if (nullptr != RocketMeshComp)
	{
		RocketMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AHomingRocket::OnOverlapBegin);
		RocketMeshComp->OnComponentEndOverlap.AddDynamic(this, &AHomingRocket::OnOverlapEnd);
	}
}

void AHomingRocket::SetupFsmComponent()
{
	RocketFsmComponent = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComponent"));
	RocketFsmComponent->CreateState(ERocketState::PlayerChase,
		[this]
		{
		},

		[this](float DT)
		{
			if (false == bIsActive)
			{
				RocketFsmComponent->ChangeState(ERocketState::DestroyWait);
				return;
			}
			
			if (nullptr == TargetActor)
			{
				return;
			}

			if (0.0f >= RocketLifeTime)
			{
				RocketLifeTime = 0.0f;
				RocketFsmComponent->ChangeState(ERocketState::PlayerEquipWait);
				return;
			}

			// 플레이어 추적 로직 
			TickPlayerChaseLogic(DT);
		},

		[this]
		{
			
		});

	// 바닥에 떨어져 있는 상태 
	RocketFsmComponent->CreateState(ERocketState::PlayerEquipWait,
		[this]
		{
			// 중력 ON, 피직스 ON
			RocketMeshComp->SetSimulatePhysics(true);
			RocketMeshComp->SetEnableGravity(true);

			// 나이아가라 이펙트 off 
			Multicast_FireEffectToggleSwitch();
		},

		[this](float DT)
		{
			if (true == bIsPlayerOverlap && nullptr != OverlapActor)
			{
				bool InputCheck = OverlapActor->GetIsInteract();
				if (true == InputCheck)
				{
					RocketFsmComponent->ChangeState(ERocketState::PlayerEquipCorrect);
					return;
				}
			}
		},

		[this]
		{
			
		});

	// 플레이어가 애니메이션을 동작하는 중일 때 
	// 플레이어 회전보정
	// 애니메이션 프레임 시간 체크해서 이제 장착 -> 스테이트 변경 장착 
	RocketFsmComponent->CreateState(ERocketState::PlayerEquipCorrect,
		[this]
		{
			EnablePlayerFlying();
			FVector TargetVector = GetActorForwardVector();
			PlayerEquipLerpEndRotation = TargetVector.Rotation();
			PlayerEquipLerpStartRotation = OverlapActor->GetActorRotation();
		},

		[this](float DT)
		{
			PlayerEquipLerpRatio += DT * 2.0f;
			if (1.0f <= PlayerEquipLerpRatio)
			{
				PlayerEquipLerpRatio = 1.0f;
				FRotator TargetRotation = FMath::Lerp(PlayerEquipLerpStartRotation, PlayerEquipLerpEndRotation, PlayerEquipLerpRatio);
				OverlapActor->SetActorRotation(TargetRotation);
				RocketFsmComponent->ChangeState(ERocketState::PlayerEquip);
				return;
			}

			FRotator TargetRotation = FMath::Lerp(PlayerEquipLerpStartRotation, PlayerEquipLerpEndRotation, PlayerEquipLerpRatio);
			OverlapActor->SetActorRotation(TargetRotation);
		},

		[this]
		{
			PlayerEquipLerpRatio = 0.0f;
		});

	RocketFsmComponent->CreateState(ERocketState::PlayerEquip,
		[this]
		{
			PlayerEquipBegin();
			Multicast_FireEffectToggleSwitch();
		},

		[this](float DT)
		{
			//  함수로 변경할거고, 이름..은 
			// 해당상태로 전환된지 10초 이상 지났다면 플레이어 장착 해제 후 Destroy;
			if (PlayerEquipMaxLiveTime <= RocketFsmComponent->GetStateLiveTime())
			{
				Multicast_SpawnDestroyEffect();
				DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
				
				// 플레이어 로켓하차 함수 호출 
				OverlapActor->OverlapHomingFunc();
				RocketFsmComponent->ChangeState(ERocketState::DestroyWait);
				return;
			}
			
			if (true == bIsBossOverlap)
			{
				Multicast_SpawnDestroyEffect();

				// 플레이어 액터 장착해제 
				DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
				// 플레이어한테 너 이제 로켓하차임 
				OverlapActor->OverlapHomingFunc();
				if (nullptr == GetAttachParentActor())
				{
					// UE_LOG(LogTemp, Warning, TEXT("Parent Actor Detach"));
					// 디스트로이 대기상태로 전환하고
					RocketFsmComponent->ChangeState(ERocketState::DestroyWait);
					
					// 보스한테 데미지 
					BossActor->SetDamage(RocketDamageToBoss);
				}
				return;
			}

		},

		[this]
		{
			
		});

	// 대기상태일 때 실제로 제거하지 않고 이펙트만 생성한 상태로 10초후에 삭제
	RocketFsmComponent->CreateState(ERocketState::DestroyWait,
		[this]
		{
			this->SetActorHiddenInGame(true);
		},

		[this](float DT)
		{
			// 상태지속시간이 10초가 넘었다면 Destroy 상태로 변경
			if (10.0f <= RocketFsmComponent->GetStateLiveTime())
			{
				// UE_LOG(LogTemp, Warning, TEXT("Rocket Destroy"));
				RocketFsmComponent->ChangeState(ERocketState::Destroy);
			
				return;
			}
		},

		[this]
		{
		});

	RocketFsmComponent->CreateState(ERocketState::Destroy,
		[this]
		{
			//UE_LOG(LogTemp, Warning, TEXT("Rocket Destroy State Start"));
		},

		[this](float DT)
		{
		},

		[this]
		{
		});
}

void AHomingRocket::Multicast_FireEffectToggleSwitch_Implementation()
{
	if (nullptr != FireEffectComp)
	{
		FireEffectComp->ToggleActive();
	}
	
}

bool AHomingRocket::IsMaxFloorDistance()
{
	AEnemyFlyingSaucer* OnwerActor = Cast<AEnemyFlyingSaucer>(GetOwner());
	if (nullptr != OnwerActor)
	{
		FVector FloorLocation = OnwerActor->GetFloor()->GetActorLocation();
		float LocationZ = GetActorLocation().Z - FloorLocation.Z;
		if (MaxFloorDistance >= LocationZ)
		{
			return true;
		}
	}

	return false;
}



void AHomingRocket::TickPlayerChaseLogic(float DeltaTime)
{
	RocketLifeTime -= DeltaTime;
	FVector RocketLocation = GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	FVector Dir = TargetLocation - RocketLocation;
	Dir.Normalize();

	SetActorRotation(Dir.Rotation());

	FVector NewRocketLocation = RocketLocation + Dir * RocketMoveSpeed * DeltaTime;
	SetActorLocation(NewRocketLocation);


	if (false == bIsSetLifeTime && true == IsMaxFloorDistance())
	{
		SetRocektLifeTime(3.0f);
		bIsSetLifeTime = true;
	}
}

void AHomingRocket::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	int32 CurrentState = RocketFsmComponent->GetCurrentState();
	ERocketState CurrentStateEnum = static_cast<ERocketState>(CurrentState);

	switch (CurrentStateEnum)
	{
	case ERocketState::PlayerChase:
	{
		if (true == OtherActor->ActorHasTag(TEXT("Player")))
		{
			if (true == bIsActive)
			{
				bIsActive = false;
			}
		}
	}
	break;
	case ERocketState::PlayerEquipWait:
	{
		if (true == OtherActor->ActorHasTag(TEXT("Player")))
		{
			bIsPlayerOverlap = true;
			APlayerBase* PlayerBase = Cast<APlayerBase>(OtherActor);
			OverlapActor = PlayerBase;
		}
	}
	break;
	case ERocketState::PlayerEquip:
	{
		if (true == OtherActor->ActorHasTag(TEXT("Boss")))
		{
			// 불값만 변경해주고 FsmComp Tick 에서 상태변경
			bIsBossOverlap = true;
			BossActor = Cast<AEnemyFlyingSaucer>(OtherActor);
			BossActor->SetRocketHit();
		}
	}
	break;
	default:
		break;
	}
}

void AHomingRocket::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (nullptr != OtherActor)
	{
		if (true == OtherActor->ActorHasTag("Player"))
		{
			bIsPlayerOverlap = false;
		}
	}
}

void AHomingRocket::PlayerEquipBegin()
{
	bIsPlayerEquip = true;
	RocketMeshComp->SetSimulatePhysics(false);
	RocketMeshComp->SetEnableGravity(false);
	// 부착해제 되었던 로켓메시를 다시 부착
	RocketMeshComp->AttachToComponent(SceneComp, FAttachmentTransformRules::KeepRelativeTransform);

	// 로컬값 초기화
	SetActorRelativeLocation(FVector::ZeroVector);
	RocketMeshComp->SetRelativeLocation(FVector::ZeroVector);

	// 플레이어의 메시에 로켓메시 부착
	AttachToComponent(OverlapActor->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("RocketSocket"));
	this->SetOwner(OverlapActor);

	// 플레이어함수 세팅함수 호출 
	OverlapActor->SetLocationBool();
}

void AHomingRocket::DestroyRocket()
{
	Destroy();
}

// Fly 활성 
void AHomingRocket::EnablePlayerFlying()
{
	OverlapActor->TestFunction();
}

// Fly 비활성
void AHomingRocket::DisablePlayerFlying()
{
}
