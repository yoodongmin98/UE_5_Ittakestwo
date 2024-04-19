// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFlyingSaucer.h"
#include "Misc/Paths.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "InteractionUIComponent.h"
#include "FsmComponent.h"
#include "HomingRocket.h"
#include "EnemyMoonBaboon.h"
#include "ArcingProjectile.h"
#include "Floor.h"
#include "EnergyChargeEffect.h"
#include "FlyingSaucerAIController.h"
#include "BossRotatePivotActor.h"
#include "OverlapCheckActor.h"
#include "PlayerBase.h"
#include "Cody.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

// Sets default values
AEnemyFlyingSaucer::AEnemyFlyingSaucer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		SetupComponent();
		SetupFsmComponent();
		Tags.Add(FName("Boss"));

		bReplicates = true;
		SetReplicateMovement(true);
	}
}

// Called when the game starts or when spawned
void AEnemyFlyingSaucer::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		// 탑승한 원숭이 세팅
		EnemyMoonBaboon = GetWorld()->SpawnActor<AEnemyMoonBaboon>(EnemyMoonBaboonClass);
		EnemyMoonBaboon->SetOwner(this);
		FsmComp->ChangeState(EBossState::None);
		AttachToActor(FloorObject, FAttachmentTransformRules::KeepWorldTransform);
	}
}

// test code, 추후 삭제 
void AEnemyFlyingSaucer::BossHitTestFireRocket()
{
	// 1번로켓 세팅
	AActor* FloorActor = Cast<AActor>(GetFloor());
	AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
	AActor* TargetActor = this;
	AHomingRocket* TestHomingRocket = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
	TestHomingRocket->SetupTarget(TargetActor);
	TestHomingRocket->SetActorLocation(FVector(0.0f, 0.0f, 1000.0f));
	TestHomingRocket->SetOwner(this);
}

void AEnemyFlyingSaucer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 메시 컴포넌트를 Replication하기 위한 설정 추가
	DOREPLIFETIME(AEnemyFlyingSaucer, EnemyMoonBaboon);
	DOREPLIFETIME(AEnemyFlyingSaucer, FsmComp);
	DOREPLIFETIME(AEnemyFlyingSaucer, UIComp);
	DOREPLIFETIME(AEnemyFlyingSaucer, LaserSpawnPointMesh);
	DOREPLIFETIME(AEnemyFlyingSaucer, HomingRocketSpawnPointMesh1);
	DOREPLIFETIME(AEnemyFlyingSaucer, HomingRocketSpawnPointMesh2);
	DOREPLIFETIME(AEnemyFlyingSaucer, ArcingProjectileSpawnPointMesh);
	DOREPLIFETIME(AEnemyFlyingSaucer, AnimInstance);
	DOREPLIFETIME(AEnemyFlyingSaucer, AnimSequence);
	DOREPLIFETIME(AEnemyFlyingSaucer, SkeletalMeshComp);
}

// 우주선 애니메이션 변경 
void AEnemyFlyingSaucer::Multicast_ChangeAnimationFlyingSaucer_Implementation(const FString& AnimationPath, const uint8 AnimType, bool AnimationLoop)
{
	if (nullptr == SkeletalMeshComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkeletalMeshComp is nullptr"));
		return;
	}

	EAnimationAssetType AnimationAssetType = static_cast<EAnimationAssetType>(AnimType);
	switch (AnimationAssetType)
	{
	case AEnemyFlyingSaucer::EAnimationAssetType::Sequence:
	{
		UAnimInstance* AnimationInstance = SkeletalMeshComp->GetAnimInstance();
		UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, *AnimationPath);
		if (nullptr == LoadedAnimationSequence)
		{
			UE_LOG(LogTemp, Warning, TEXT("Animation Sequence is nullptr"));
			return;
		}
		SkeletalMeshComp->PlayAnimation(LoadedAnimationSequence, AnimationLoop);
	}
		break;
	case AEnemyFlyingSaucer::EAnimationAssetType::Blueprint:
	{
		UAnimBlueprint* LoadedAnimBlueprint = LoadObject<UAnimBlueprint>(nullptr, *AnimationPath);
		if (nullptr != LoadedAnimBlueprint)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Change AnimBluerprint"));
			SkeletalMeshComp->SetAnimInstanceClass(LoadedAnimBlueprint->GeneratedClass);
		}
	}
		break;
	}
	
}

// 원숭이 애니메이션 변경
void AEnemyFlyingSaucer::Multicast_ChangeAnimationMoonBaboon_Implementation(const FString& AnimationPath, const uint8 AnimType, bool AnimationLoop)
{
	if (nullptr == EnemyMoonBaboon)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyMoonBaboon is nullptr"));
		return;
	}

	EAnimationAssetType AnimationType = static_cast<EAnimationAssetType>(AnimType);
	switch (AnimationType)
	{
	case AEnemyFlyingSaucer::EAnimationAssetType::Sequence:
	{
		UAnimInstance* AnimationInstance = SkeletalMeshComp->GetAnimInstance();
		UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, *AnimationPath);
		if (nullptr == LoadedAnimationSequence)
		{
			UE_LOG(LogTemp, Warning, TEXT("Animation Sequence is nullptr"));
			return;
		}

		EnemyMoonBaboon->GetMesh()->PlayAnimation(LoadedAnimationSequence, AnimationLoop);
	}
	break;
	case AEnemyFlyingSaucer::EAnimationAssetType::Blueprint:
	{
		UAnimBlueprint* LoadedAnimBlueprint = LoadObject<UAnimBlueprint>(nullptr, *AnimationPath);
		if (nullptr == LoadedAnimBlueprint)
		{
			UE_LOG(LogTemp, Warning, TEXT("AnimBlueprint is nullptr"));
			return;
		}

		EnemyMoonBaboon->GetMesh()->SetAnimInstanceClass(LoadedAnimBlueprint->GeneratedClass);
	}
	break;
	}
}

// Called every frame
void AEnemyFlyingSaucer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
			
	}
}

// Called to bind functionality to input
void AEnemyFlyingSaucer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyFlyingSaucer::SetupRotateCenterPivotActor()
{
	// 여기서 현재 어태치된 액터 해제해야하고 부착해야할 수 도 있음. 체크 

	RotateCenterPivotActor = GetWorld()->SpawnActor<ABossRotatePivotActor>(RotateCenterPivotActorClass, FVector::ZeroVector,FRotator::ZeroRotator);
	if (nullptr != RotateCenterPivotActor)
	{
		RotateCenterPivotActor->SetActorEnableCollision(false);
		UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(RotateCenterPivotActor->GetRootComponent());
		if (nullptr != PrimitiveComp)
		{
			PrimitiveComp->SetEnableGravity(false);
		}
		AttachToActor(RotateCenterPivotActor, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void AEnemyFlyingSaucer::DetachRotateCenterPivotActor()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	if (nullptr != RotateCenterPivotActor)
	{
		RotateCenterPivotActor->Destroy();
	}
}

void AEnemyFlyingSaucer::RotationCenterPivotActor(float DeltaTime)
{
	if (nullptr != RotateCenterPivotActor)
	{
		RotateCenterPivotActor->AddActorLocalRotation({ 0.f, 1.f * DeltaTime * RotateCenterPivotActorMoveSpeed , 0.0f });
	}
}

void AEnemyFlyingSaucer::FireHomingRocket()
{
	if (0 < HomingRocketFireCount)
	{
		return;
	}

	HomingRocketFireCount += 2;

	// 1번로켓 세팅
	AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
	AActor* TargetActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerCody")));
	AHomingRocket* HomingRocket1 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
	HomingRocket1->SetupTarget(TargetActor);
	HomingRocket1->SetActorLocation(HomingRocketSpawnPointMesh1->GetComponentLocation());
	HomingRocket1->SetOwner(this);

	// 2번로켓 세팅
	TargetActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerMay")));
	AHomingRocket* HomingRocket2 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
	HomingRocket2->SetupTarget(TargetActor);
	HomingRocket2->SetActorLocation(HomingRocketSpawnPointMesh2->GetComponentLocation());
	HomingRocket2->SetOwner(this);
}

void AEnemyFlyingSaucer::FireArcingProjectile()
{
	++ArcingProjectileFireCount;

	AActor* FloorActor = Cast<AActor>(GetFloor());
	AArcingProjectile* Projectile = GetWorld()->SpawnActor<AArcingProjectile>(ArcingProjectileClass, ArcingProjectileSpawnPointMesh->GetComponentLocation(), FRotator::ZeroRotator);
	AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
	if (nullptr != Projectile && nullptr != AIController)
	{
		FVector TargetLocation = FVector::ZeroVector;
		if (ArcingProjectileFireCount % 2 == 0)
		{
			AActor* TargetActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerCody")));
			TargetLocation = TargetActor->GetActorLocation();
			
		}

		else if (ArcingProjectileFireCount % 2 == 1)
		{
			AActor* TargetActor = Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(TEXT("PlayerMay")));
			TargetLocation = TargetActor->GetActorLocation();
		}

		Projectile->SetupTargetLocation(TargetLocation);
	}
	
	Projectile->SetupProjectileMovementComponent();
	Projectile->SetOwner(this);
	if (Projectile != nullptr)
	{
		Projectile->AttachToActor(FloorActor, FAttachmentTransformRules::KeepWorldTransform);
	}
}

AEnergyChargeEffect* AEnemyFlyingSaucer::CreateEnergyChargeEffect()
{
	EnergyChargeEffect = nullptr;
	EnergyChargeEffect = GetWorld()->SpawnActor<AEnergyChargeEffect>(EnergyChargeEffectClass,LaserSpawnPointMesh->GetComponentLocation(), FRotator::ZeroRotator);
	EnergyChargeEffect->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("EnergyChargeEffectSpawnPointSocket"));

	return EnergyChargeEffect;
}

void AEnemyFlyingSaucer::SetupComponent()
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	SetRootComponent(CapsuleComp);
	GetArrowComponent()->SetupAttachment(CapsuleComp);
	GetCharacterMovement()->SetUpdatedComponent(GetCapsuleComponent());
	
	SkeletalMeshComp = GetMesh();
	SkeletalMeshComp->SetupAttachment(CapsuleComp);

	LaserSpawnPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserSpawnPointMesh"));
	LaserSpawnPointMesh->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("LaserSpawnPointSocket"));

	HomingRocketSpawnPointMesh1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HomingRocketSpawnPointMesh1"));
	HomingRocketSpawnPointMesh1->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("HomingRocketSpawnPointSocket_01"));

	HomingRocketSpawnPointMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HomingRocketSpawnPointMesh2"));
	HomingRocketSpawnPointMesh2->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("HomingRocketSpawnPointSocket_02"));

	ArcingProjectileSpawnPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ArcingProjectileSpawnPointMesh"));
	ArcingProjectileSpawnPointMesh->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("ArcingProjectileSpawnPointSocket"));

	UIComp = CreateDefaultSubobject<UInteractionUIComponent>(TEXT("InteractionUIComponent"));
	UIComp->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("UISocket"));
	
	// 부모 비활성, 자식 비활성 
	UIComp->SetVisibility(false, true);
}

void AEnemyFlyingSaucer::DrawDebugMesh()
{
	FVector LaserSpawnPointMeshLocation = LaserSpawnPointMesh->GetComponentLocation();

	float SphereRadius = 50.0f;
	int32 Segments = 12;
	float LifeTime = 0.1f;
	float Thickness = 2.0f;

	DrawDebugSphere(
		GetWorld(),
		LaserSpawnPointMeshLocation,
		SphereRadius,
		Segments,
		FColor::Red,
		false,
		LifeTime,
		0,
		Thickness
		);

	FVector HomingRocketSpawnPointMesh1Locaiton = HomingRocketSpawnPointMesh1->GetComponentLocation();
	
	DrawDebugSphere(
		GetWorld(),
		HomingRocketSpawnPointMesh1Locaiton,
		SphereRadius,
		Segments,
		FColor::Blue,
		false,
		LifeTime,
		0,
		Thickness
	);

	FVector HomingRocketSpawnPointMesh2Locaiton = HomingRocketSpawnPointMesh2->GetComponentLocation();

	DrawDebugSphere(
		GetWorld(),
		HomingRocketSpawnPointMesh2Locaiton,
		SphereRadius,
		Segments,
		FColor::Blue,
		false,
		LifeTime,
		0,
		Thickness
	);

	FVector ArcingProjectileSpawnPointMeshLocaiton = ArcingProjectileSpawnPointMesh->GetComponentLocation();

	DrawDebugSphere(
		GetWorld(),
		ArcingProjectileSpawnPointMeshLocaiton,
		SphereRadius,
		Segments,
		FColor::Green,
		false,
		LifeTime,
		0,
		Thickness
	);
}

void AEnemyFlyingSaucer::SetupFsmComponent()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComponent"));
	FsmComp->CreateState(EBossState::None,
		[this]
		{
		},

		[this](float DT)
		{
			if (3.0f <= FsmComp->GetStateLiveTime())
			{
				FsmComp->ChangeState(EBossState::Phase1Start);
				return;
			}
		},

		[this]
		{
			EnemyMoonBaboon->SetActorRelativeLocation(FVector::ZeroVector);
			EnemyMoonBaboon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("ChairSocket"));
		});


	FsmComp->CreateState(EBossState::Phase1Start,
		[this]
		{
			Multicast_ChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/CutScenes/PlayRoom_SpaceStation_BossFight_EnterUFO_FlyingSaucer_Anim"), 1, false);
			Multicast_ChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Programming_Anim"), 1, false);
		},

		[this](float DT)
		{
			// 애니메이션 종료 체크 
			bool bIsAnimationPlaying = GetMesh()->IsPlaying();
			if (!bIsAnimationPlaying)
			{
				FsmComp->ChangeState(EBossState::Phase1Progress);
				return;
			}

		
		},

		[this]
		{
			// MoonBaboon 애니메이션 변경
			USkeletalMeshComponent* SkeletalMeshComponent = EnemyMoonBaboon->GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// 애님인스턴스 받아와서 애니메이션 재생
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, true);
				}
			}
		});

	FsmComp->CreateState(EBossState::Phase1Progress,
		[this]
		{
			// FlyingSaucer 애니메이션 변경
			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// 애님인스턴스 받아와서 애니메이션 재생
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, true);
				}
			}

			
		},

		[this](float DT)
		{
			// 페이즈 종료 검사 임시 
			if (CurrentHp <= 67.0f)
			{
				FsmComp->ChangeState(EBossState::Phase1End);
				return;
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase1End,
		[this]
		{
			// 페이즈 종료시점에 추락 + 원숭이 애니메이션 변경 후 
			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// 애님인스턴스 받아와서 애니메이션 재생
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyFlyingSaucer/CutScenes/PlayRoom_SpaceStation_BossFight_PowerCoresDestroyed_FlyingSaucer_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, false);
				}
			}

			// MoonBaboon 애니메이션 변경
			SkeletalMeshComponent = EnemyMoonBaboon->GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// 애님인스턴스 받아와서 애니메이션 재생
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Programming_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, true);
				}
			}

			// 4.5초 후에 UI 활성화 
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AEnemyFlyingSaucer::ActivateUIComponent, 4.5f, false);

			FTimerHandle TimerHandle2;
			GetWorldTimerManager().SetTimer(TimerHandle2, this, &AEnemyFlyingSaucer::SpawnOverlapCheckActor, 4.5f, false);
		},

		[this](float DT)
		{
			if (nullptr == OverlapCheckActor)
			{
				return;
			}

			APlayerBase* CurrentOverlapPlayer = OverlapCheckActor->GetCurrentOverlapPlayer();
			if (nullptr != CurrentOverlapPlayer)
			{
				// 코디이고.
				if (true == CurrentOverlapPlayer->ActorHasTag("Cody"))
				{
					ACody* PlayerCody = Cast<ACody>(CurrentOverlapPlayer);
					
					// 코디가 커진 상태인것도 체크 
					bool bIsInteract = CurrentOverlapPlayer->GetIsInteract();
					CodySize Size = PlayerCody->GetCodySize();

					// 현재 인터랙트 키 눌렀는지 체크
					if (true == bIsInteract && CodySize::BIG == Size)
					{
						// UE_LOG(LogTemp, Warning, TEXT("Change State : CodyHolding Start"));

						// 눌렀으면 ChangeState 
						FsmComp->ChangeState(EBossState::CodyHoldingStart);
						return;
					}
				}
			}
		},

		[this]
		{

		});


	FsmComp->CreateState(EBossState::CodyHoldingStart,
		[this]
		{
			// e키 눌렀을 때 여기 들어오고 
			// 우주선 -> 코디홀딩 시작모션
			// 원숭이 -> 그 이상한 모션 loop 로 변경

			// 페이즈 종료시점에 추락 + 원숭이 애니메이션 변경 후 
			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// 애님인스턴스 받아와서 애니메이션 재생
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_CodyHolding_Enter_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, false);
				}
			}

			// MoonBaboon 애니메이션 변경
			SkeletalMeshComponent = EnemyMoonBaboon->GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// 애님인스턴스 받아와서 애니메이션 재생
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_CodyHolding_Enter_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, false);
				}
			}
		},

		[this](float DT)
		{
			USkeletalMeshComponent* FlyingSaucerMesh = GetMesh();
			USkeletalMeshComponent* MoonBaboonMesh = EnemyMoonBaboon->GetMesh();
			if (false == FlyingSaucerMesh->IsPlaying() && false == MoonBaboonMesh->IsPlaying())
			{
				
				FsmComp->ChangeState(EBossState::CodyHoldingProgress_NotKeyMashing);
				return;
			}
		},

		[this]
		{

		});

	// 들고 있는 상태, 키연타 X, 여기서 상호작용키를 한번더 입력했다면 
	// 각도 변경 상태로 전환 
	FsmComp->CreateState(EBossState::CodyHoldingProgress_NotKeyMashing,
		[this]
		{
			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// 애님인스턴스 받아와서 애니메이션 재생
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_CodyHolding_Low_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, true);
				}
			}

			// MoonBaboon 애니메이션 변경
			SkeletalMeshComponent = EnemyMoonBaboon->GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// 애님인스턴스 받아와서 애니메이션 재생
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_CodyHolding_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, true);
				}
			}
		},

		[this](float DT)
		{
			if (nullptr == OverlapCheckActor)
			{
				return;
			}

			APlayerBase* CurrentOverlapPlayer = OverlapCheckActor->GetCurrentOverlapPlayer();
			if (nullptr != CurrentOverlapPlayer)
			{
				// 코디이고.
				if (true == CurrentOverlapPlayer->ActorHasTag("Cody"))
				{
					// 코디가 커진 상태인것도 체크 
					bool bIsInteract = CurrentOverlapPlayer->GetIsInteract();

					// 현재 인터랙트 키 눌렀는지 체크
					if (true == bIsInteract)
					{
						

						// 눌렀으면 ChangeState 
						FsmComp->ChangeState(EBossState::CodyHoldingProgress_ChangeOfAngle);
						return;
					}
				}
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::CodyHoldingProgress_ChangeOfAngle,
		[this]
		{
			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				UAnimBlueprint* LoadedAnimBlueprint = LoadObject<UAnimBlueprint>(nullptr, TEXT("/Game/Characters/EnemyFlyingSaucer/BluePrints/ABP_EnemyFlyingSaucer_CodyHoldingRotation"));
				if (nullptr != LoadedAnimBlueprint)
				{
					// UE_LOG(LogTemp, Warning, TEXT("Change AnimBluerprint"));
					SkeletalMeshComponent->SetAnimInstanceClass(LoadedAnimBlueprint->GeneratedClass);
				}
			}

			StateCompleteTime = 3.0f;
			
		},

		[this](float DT)
		{
			if (StateCompleteTime <= 0.0f)
			{
				FsmComp->ChangeState(EBossState::CodyHoldingProgress_KeyMashing);
				return;
			}

			StateCompleteTime -= DT;
		},

		[this]
		{
			StateCompleteTime = 0.0f;
		});


	// 키연타중 
	FsmComp->CreateState(EBossState::CodyHoldingProgress_KeyMashing,
		[this]
		{
			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				// 애님인스턴스 받아와서 애니메이션 재생
				UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
				UAnimSequence* LoadedAnimationSequence = LoadObject<UAnimSequence>(nullptr, TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_CodyHolding_Anim"));
				if (nullptr != LoadedAnimationSequence)
				{
					SkeletalMeshComponent->PlayAnimation(LoadedAnimationSequence, true);
				}
			}

			bIsKeyMashing = true;
			KeyMashingTime = KeyMashingMaxTime;
		},

		[this](float DT)
		{
			if (KeyMashingTime <= 0.0f)
			{
				// UE_LOG(LogTemp, Warning, TEXT("KeyMashing Time : 0.0f"));
				FsmComp->ChangeState(EBossState::CodyHoldingProgress_KeyMashingEnd);
				return;
			}

			APlayerBase* OverlapPlayer = OverlapCheckActor->GetCurrentOverlapPlayer();
			if (nullptr != OverlapPlayer)
			{
				if (OverlapPlayer->ActorHasTag(TEXT("Cody")))
				{
					bool CheckInput = OverlapPlayer->GetIsInteract();
					if (true == CheckInput)
					{
						 // UE_LOG(LogTemp, Warning, TEXT("KeyMashing Time ++"));
						KeyMashingTime = 0.75f;
					}
				}
			}

			KeyMashingTime -= DT;
		},

		[this]
		{
			bIsKeyMashing = false;
			KeyMashingTime = KeyMashingMaxTime;
		});


	FsmComp->CreateState(EBossState::CodyHoldingProgress_KeyMashingEnd,
		[this]
		{
			// 고각도에서 다시 내려오는 애니메이션 적용
			UE_LOG(LogTemp, Warning, TEXT("CodyHoldingProgress_KeyMashingEnd Start"));

			USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
			if (nullptr != SkeletalMeshComponent)
			{
				UAnimBlueprint* LoadedAnimBlueprint = LoadObject<UAnimBlueprint>(nullptr, TEXT("/Game/Characters/EnemyFlyingSaucer/BluePrints/ABP_EnemyFlyingSaucer_CodyHoldingRotationReverse"));
				if (nullptr != LoadedAnimBlueprint)
				{
					// UE_LOG(LogTemp, Warning, TEXT("Change AnimBluerprint"));
					SkeletalMeshComponent->SetAnimInstanceClass(LoadedAnimBlueprint->GeneratedClass);
				}
			}

			StateCompleteTime = 2.0f;
		},

		[this](float DT)
		{
			// 애니메이션 재생완료 시 CodyHoldingProgress_NotKeyMashing 로 변경 

			// 1. 블렌드스페이스 역재생 만들어야함. 완
			// start 에서 애니메이션 적용 완
			// tick 에서 애니메이션 종료시 notkeymashing 으로 변경
			// 여기서 애니메이션 종료되었는지 체크 
			
			if (StateCompleteTime <= 0.0f)
			{
				UE_LOG(LogTemp, Warning, TEXT("CodyHoldingProgress_NotKeyMashingEnd Start"));
				FsmComp->ChangeState(EBossState::CodyHoldingProgress_NotKeyMashing);
				return;
			}

			StateCompleteTime -= DT;
		},

		[this]
		{
			StateCompleteTime = 0.0f;
		});



	FsmComp->CreateState(EBossState::Phase2Start,
		[this]
		{
			
		},

		[this](float DT)
		{
			

		},

		[this]
		{
			
		});

	FsmComp->CreateState(EBossState::Phase2Progress,
		[this]
		{
		

		},

		[this](float DT)
		{

		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase2End,
		[this]
		{

		},

		[this](float DT)
		{

		},

		[this]
		{

		});
}

void AEnemyFlyingSaucer::ActivateUIComponent()
{
	if (nullptr != UIComp)
	{
		UIComp->SetVisibility(true, true);
	}
}

void AEnemyFlyingSaucer::SpawnOverlapCheckActor()
{
	OverlapCheckActor = GetWorld()->SpawnActor<AOverlapCheckActor>(OverlapCheckActorClass, GetActorLocation(), GetActorRotation());
	OverlapCheckActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("OverlapCheckActorSocket"));
	OverlapCheckActor->SetOverlapActorNameTag(TEXT("Player"));
}
