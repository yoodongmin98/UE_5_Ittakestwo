// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingRocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraComponent.h"
#include "SoundManageComponent.h"
#include "FsmComponent.h"
#include "EnemyFlyingSaucer.h"
#include "ExplosionEffect.h"
#include "Floor.h"
#include "Cody.h"
#include "PlayerBase.h"
#include "Misc/OutputDeviceNull.h"

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

		SoundComp = CreateDefaultSubobject<USoundManageComponent>(TEXT("SoundComponent"));
		SetupFsmComponent();
	}
}

// Called when the game starts or when spawned
void AHomingRocket::BeginPlay()
{
	Super::BeginPlay();
	if (true == HasAuthority())
	{
		RocketFsmComponent->ChangeState(ERocketState::PlayerChase);
		SetupOverlapEvent();
		SoundComp->AttachToComponent(SceneComp, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

// Called every frame
void AHomingRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
	}
}

void AHomingRocket::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// �޽� ������Ʈ�� Replication�ϱ� ���� ���� �߰�
	DOREPLIFETIME(AHomingRocket, FireEffectComp);
	DOREPLIFETIME(AHomingRocket, BossActor);
	DOREPLIFETIME(AHomingRocket, RocketDamageToBoss);
	DOREPLIFETIME(AHomingRocket, RocketDamageToPlayer);
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
		return -1;
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
			SoundComp->MulticastChangeSound(TEXT("SC_RocketFireStart"));
		},

		[this](float DT)
		{
			if (false == bIsRocketLoopSoundPlay && 0.75f <= RocketFsmComponent->GetStateLiveTime())
			{
				bIsRocketLoopSoundPlay = true;
				SoundComp->MulticastChangeSound(TEXT("SC_RocketLoop"));
			}

			// �÷��̾� �浹�� 
			if (false == bIsActive)
			{
				Multicast_SpawnDestroyEffect();
				RocketFsmComponent->ChangeState(ERocketState::Destroy);
				SoundComp->MulticastPlaySoundDirect(TEXT("SC_RocketDestroy"));
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
				SoundComp->MulticastChangeSound(TEXT("SC_RocketStopFollowing"));
				return;
			}

			// �÷��̾� ���� ���� 
			TickPlayerChaseLogic(DT);
		},

		[this]
		{
			bIsRocketLoopSoundPlay = false;
		});

	// �ٴڿ� ������ �ִ� ���� 
	RocketFsmComponent->CreateState(ERocketState::PlayerEquipWait,
		[this]
		{
			// �߷� ON, ������ ON
			RocketMeshComp->SetSimulatePhysics(true);
			RocketMeshComp->SetEnableGravity(true);

			// ���̾ư��� ����Ʈ off 
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
	
	RocketFsmComponent->CreateState(ERocketState::PlayerEquipCorrect,
		[this]
		{
			EnablePlayerFlying();

			// ������ ȸ�����־ ����Ʈ���� ���
			FVector TargetRightVector = RocketMeshComp->GetRightVector();
			PlayerEquipLerpEndRotation = TargetRightVector.Rotation();
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
			SoundComp->MulticastChangeSound(TEXT("SC_RocketRideStart"));
		},

		[this](float DT)
		{
			if (false == bIsRocketLoopSoundPlay && 0.75f <= RocketFsmComponent->GetStateLiveTime())
			{
				bIsRocketLoopSoundPlay = true;
				SoundComp->MulticastChangeSound(TEXT("SC_RocketLoop"));
			}

			if (PlayerEquipMaxLiveTime <= RocketFsmComponent->GetStateLiveTime())
			{
				Multicast_SpawnDestroyEffect();
				DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
				// �÷��̾� �������� �Լ� 
				OverlapActor->OverlapHomingFunc();
				RocketFsmComponent->ChangeState(ERocketState::Destroy);
				SoundComp->MulticastPlaySoundDirect(TEXT("SC_RocketDestroy"));
				return;
			}
			
			if (true == bIsBossOverlap)
			{
				Multicast_SpawnDestroyEffect();
				DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
				// �÷��̾����� �� ���� ���������� 
				OverlapActor->OverlapHomingFunc();
				RocketFsmComponent->ChangeState(ERocketState::Destroy);
				BossActor->SetDamage(RocketDamageToBoss);
				SoundComp->MulticastPlaySoundDirect(TEXT("SC_RocketDestroy"));
				return;
			}
		},

		[this]
		{
			bIsRocketLoopSoundPlay = false;
		});

	RocketFsmComponent->CreateState(ERocketState::Destroy,
		[this]
		{
			this->SetActorHiddenInGame(true);
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
				APlayerBase* PlayerBase = Cast<APlayerBase>(OtherActor);
				OverlapActor = PlayerBase;
				PlayerBase->AttackPlayer(RocketDamageToPlayer);
				// UE_LOG(LogTemp, Warning, TEXT("%f"), PlayerBase->GetPlayerHP());
			}
		}
	}
	break;
	case ERocketState::PlayerEquipWait:
	{
		if (true == OtherActor->ActorHasTag(TEXT("Player")))
		{
			// ���⼭ ���� �ڵ��� ��� �븻 ���� �� ���� ���� 
			if (true == OtherActor->ActorHasTag(TEXT("Cody")))
			{
				if (CodySize::NORMAL != Cast<ACody>(OtherActor)->GetCodySize())
				{
					return;
				}
			}

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
			// �Ұ��� �������ְ� FsmComp Tick ���� ���º���
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
	// ������ Ȱ��ȭ �ߴ� �ɼǵ� ��Ȱ��
	bIsPlayerEquip = true;
	RocketMeshComp->SetSimulatePhysics(false);
	RocketMeshComp->SetEnableGravity(false);

	// ����������Ʈ�� ���Ϳ� ����� 
	RocketMeshComp->AttachToComponent(SceneComp, FAttachmentTransformRules::KeepRelativeTransform);

	// ���� ��ġ, ȸ���� �ʱ�ȭ
	SetActorRelativeLocation(FVector::ZeroVector);
	SetActorRelativeRotation(FRotator::ZeroRotator);

	// ���� ��ġ�� �ʱ�ȭ �� ȸ�� �� ���� ������ �����ϰ� ����
	RocketMeshComp->SetRelativeLocation(FVector::ZeroVector);
	RocketMeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	
	// �÷��̾ ����
	AttachToComponent(OverlapActor->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("RocketSocket"));
	// ���ʼ��� 
	this->SetOwner(OverlapActor);

	// �÷��̾��� �����Ϸ�� �Լ� ȣ�� �� ���� 
	OverlapActor->PlayerToHomingRoketJumpFinished();
}

// Fly Ȱ�� 
void AHomingRocket::EnablePlayerFlying()
{
	OverlapActor->PlayerToHomingRocketJumpStart();
}

// Fly ��Ȱ��
void AHomingRocket::DisablePlayerFlying()
{
}
