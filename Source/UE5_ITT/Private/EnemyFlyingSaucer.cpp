// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFlyingSaucer.h"
#include "Engine/SkeletalMeshSocket.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FsmComponent.h"
#include "InteractionUIComponent.h"
#include "Misc/Paths.h"
#include "HomingRocket.h"
#include "EnemyMoonBaboon.h"
#include "ArcingProjectile.h"
#include "Floor.h"
#include "EnergyChargeEffect.h"
#include "FlyingSaucerAIController.h"
#include "OverlapCheckActor.h"
#include "PlayerBase.h"
#include "Cody.h"
#include "May.h"
#include "GroundPoundEffect.h"
#include "PhaseEndCameraRail.h"
#include "DrawDebugHelpers.h"
#include "CoreExplosionEffect.h"


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

void AEnemyFlyingSaucer::SetupComponent()
{
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	SetRootComponent(CapsuleComp);

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

	CodyHoldingUIComp = CreateDefaultSubobject<UInteractionUIComponent>(TEXT("CodyHoldingUIComponent"));
	CodyHoldingUIComp->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("CodyUISocket"));
	CodyHoldingUIComp->SetVisibility(false, true);

	MayLaserDestroyUIComp = CreateDefaultSubobject<UInteractionUIComponent>(TEXT("MayLaserDestroyUIComponent"));
	MayLaserDestroyUIComp->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("MayUISocket"));
	MayLaserDestroyUIComp->SetVisibility(false, true);

	RotatingComp = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));
	RotatingComp->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
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
		EnemyMoonBaboon->GetMesh()->SetVisibility(false);
		
		// State 진입
		FsmComp->ChangeState(EBossState::None);
		
		// 오버랩이벤트 세팅
		SetupOverlapEvent();
	}
}

void AEnemyFlyingSaucer::AddPatternDestoryCount()
{
	// 패턴 파훼 카운트 추가
	++PatternDestroyCount;
	UBlackboardComponent* BlackboardComp = Cast<AFlyingSaucerAIController>(GetController())->GetBlackboardComponent();
	if (nullptr != BlackboardComp)
	{
		BlackboardComp->SetValueAsInt(TEXT("PatternDestroyCount"), PatternDestroyCount);
	}
}

const int32 AEnemyFlyingSaucer::GetCurrentState()
{
	if (nullptr == FsmComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("FsmComponent is Nullptr"));
		return -1;
	}

	return FsmComp->GetCurrentState();
}


void AEnemyFlyingSaucer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AEnemyFlyingSaucer, EnemyMoonBaboon);
	DOREPLIFETIME(AEnemyFlyingSaucer, FsmComp);
	DOREPLIFETIME(AEnemyFlyingSaucer, CodyHoldingUIComp);
	DOREPLIFETIME(AEnemyFlyingSaucer, MayLaserDestroyUIComp);
	DOREPLIFETIME(AEnemyFlyingSaucer, RotatingComp);
	DOREPLIFETIME(AEnemyFlyingSaucer, SkeletalMeshComp);
	DOREPLIFETIME(AEnemyFlyingSaucer, LaserSpawnPointMesh);
	DOREPLIFETIME(AEnemyFlyingSaucer, HomingRocketSpawnPointMesh1);
	DOREPLIFETIME(AEnemyFlyingSaucer, HomingRocketSpawnPointMesh2);
	DOREPLIFETIME(AEnemyFlyingSaucer, ArcingProjectileSpawnPointMesh);
	DOREPLIFETIME(AEnemyFlyingSaucer, AnimInstance);
	DOREPLIFETIME(AEnemyFlyingSaucer, AnimSequence);
	DOREPLIFETIME(AEnemyFlyingSaucer, OverlapCheckActor);
	DOREPLIFETIME(AEnemyFlyingSaucer, EnergyChargeEffect);
	DOREPLIFETIME(AEnemyFlyingSaucer, bIsCodyHoldingEnter);
	DOREPLIFETIME(AEnemyFlyingSaucer, PrevLaserTargetLocation);
	DOREPLIFETIME(AEnemyFlyingSaucer, NextLaserTargetLocation);
	DOREPLIFETIME(AEnemyFlyingSaucer, CurrentLaserTargetLocation);
	DOREPLIFETIME(AEnemyFlyingSaucer, bPrevTargetLocationValid);
	DOREPLIFETIME(AEnemyFlyingSaucer, LaserLerpRatio);
	DOREPLIFETIME(AEnemyFlyingSaucer, LaserLerpRate);
	DOREPLIFETIME(AEnemyFlyingSaucer, LaserFireCount);
	DOREPLIFETIME(AEnemyFlyingSaucer, PatternDestroyCount);
	DOREPLIFETIME(AEnemyFlyingSaucer, ArcingProjectileFireTime);
	DOREPLIFETIME(AEnemyFlyingSaucer, ArcingProjectileMaxFireTime);
	DOREPLIFETIME(AEnemyFlyingSaucer, PlayerCody);
	DOREPLIFETIME(AEnemyFlyingSaucer, PlayerMay);
	DOREPLIFETIME(AEnemyFlyingSaucer, PrevAnimBoneLocation);
	DOREPLIFETIME(AEnemyFlyingSaucer, MaxHp);
	DOREPLIFETIME(AEnemyFlyingSaucer, CurrentHp);
	DOREPLIFETIME(AEnemyFlyingSaucer, bIsEject);
	DOREPLIFETIME(AEnemyFlyingSaucer, bIsRocketHit);
	DOREPLIFETIME(AEnemyFlyingSaucer, LaserLerpScale);
	DOREPLIFETIME(AEnemyFlyingSaucer, bIsCutSceneEnd);
	DOREPLIFETIME(AEnemyFlyingSaucer, bIsCutSceneStart);
}

// 애니메이션 리소스 타입에 따라서 애니메이션 변경
void AEnemyFlyingSaucer::MulticastChangeAnimationFlyingSaucer_Implementation(const FString& AnimationPath, const uint8 AnimType, bool AnimationLoop)
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
		if (nullptr == LoadedAnimBlueprint)
		{
			UE_LOG(LogTemp, Warning, TEXT("Animation Sequence is nullptr"));
			return;
		}

		SkeletalMeshComp->SetAnimInstanceClass(LoadedAnimBlueprint->GeneratedClass);
	}
		break;
	}
}

void AEnemyFlyingSaucer::MulticastChangeAnimationMoonBaboon_Implementation(const FString& AnimationPath, const uint8 AnimType, bool AnimationLoop)
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

void AEnemyFlyingSaucer::MulticastCheckCodyKeyPressedAndChangeState_Implementation(const bool bIsInput)
{
	if (nullptr != PlayerCody)
	{
		CodySize Size = PlayerCody->GetCodySize();

		// 현재 인터랙트 키 눌렀는지 체크
		if (true == bIsInput && CodySize::BIG == Size)
		{
			FsmComp->ChangeState(EBossState::CodyHolding_Enter);
			bIsCodyHoldingEnter = true;
			return;
		}
	}
}

void AEnemyFlyingSaucer::MulticastSetActivateUIComponent_Implementation(UInteractionUIComponent* UIComponent, bool ParentUIActivate, bool ChildUIActivate)
{
	if (nullptr != UIComponent)
	{
		UIComponent->SetVisibility(ParentUIActivate, ChildUIActivate);
	}
}

void AEnemyFlyingSaucer::MulticastCreateEnergyChargeEffect_Implementation()
{
	EnergyChargeEffect = GetWorld()->SpawnActor<AEnergyChargeEffect>(EnergyChargeEffectClass, LaserSpawnPointMesh->GetComponentLocation(), FRotator::ZeroRotator);
	EnergyChargeEffect->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, TEXT("EnergyChargeEffectSpawnPointSocket"));
}

void AEnemyFlyingSaucer::MulticastSetFocusTarget_Implementation()
{
	AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
	if (nullptr != AIController)
	{
		AIController->SetFocus(Cast<AActor>(LaserTargetActor));
	}
}

void AEnemyFlyingSaucer::MulticastAttachMoonBaboonActorWithFloor_Implementation()
{
	EnemyMoonBaboon->SetActorRelativeLocation(FVector::ZeroVector);
	EnemyMoonBaboon->GetMesh()->SetVisibility(true);
	EnemyMoonBaboon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("ChairSocket"));
	AttachToActor(FloorObject, FAttachmentTransformRules::KeepWorldTransform);
}

void AEnemyFlyingSaucer::MulticastUnPossess_Implementation()
{
	AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
	if (nullptr != AIController)
	{
		AIController->UnPossess();
	}
}


void AEnemyFlyingSaucer::CorrectCodyLocationAndRotation()
{
	if (true == bIsCodyHoldingLerpEnd)
	{
		return;
	}

	float DeltaTime = GetWorld()->GetDeltaSeconds();
	CodyLerpRatio += DeltaTime;
	if (CodyLerpRatio >= 1.0f)
	{
		CodyLerpRatio = 1.0f;
		bIsCodyHoldingLerpEnd = true;
		{
			// 1.0으로 고정된 위치 보정 해준 후 
			FVector StartLocation = PlayerCody->GetActorLocation();
			FVector EndLocation = FVector(CodyLerpEndLocation.X, CodyLerpEndLocation.Y, StartLocation.Z);
			FVector TargetLocation = FMath::Lerp(StartLocation, EndLocation, CodyLerpRatio);
			PlayerCody->SetActorLocation(TargetLocation);
		}
		{
			// 회전 변경 후 return
			FVector TargetLocation = GetActorLocation();
			TargetLocation.Z = 0.0f;
			FVector StartLocation = PlayerCody->GetActorLocation();
			StartLocation.Z = 0.0f;

			FRotator TargetRotation = (TargetLocation - StartLocation).Rotation();
			TargetRotation.Yaw -= 14.0f;
			PlayerCody->SetActorRotation(TargetRotation);
		}
		
		return;
	}

	FVector StartLocation = PlayerCody->GetActorLocation();
	FVector EndLocation = FVector(CodyLerpEndLocation.X, CodyLerpEndLocation.Y, StartLocation.Z);
	FVector TargetLocation = FMath::Lerp(StartLocation, EndLocation, CodyLerpRatio);
	PlayerCody->SetActorLocation(TargetLocation);
}


void AEnemyFlyingSaucer::MulticastHideLaserBaseBoneAndSpawnDestroyEffect_Implementation()
{
	// 레이저 본 렌더링 off
	int32 BoneIndex = SkeletalMeshComp->GetBoneIndex(TEXT("LaserBase"));
	FVector EffectSpawnLocation = SkeletalMeshComp->GetBoneLocation(TEXT("LaserBase"));
	if (INDEX_NONE != BoneIndex)
	{
		SkeletalMeshComp->HideBone(BoneIndex, EPhysBodyOp::PBO_Term);
		AActor* SpawnEffect = GetWorld()->SpawnActor<ACoreExplosionEffect>(CoreExplosionEffectClass, EffectSpawnLocation, FRotator::ZeroRotator);
		SpawnEffect->SetActorRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	}
}



// Called every frame
void AEnemyFlyingSaucer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		if (true == bIsAllPhaseEnd)
		{
			return;
		}

		if (false == bIsAllPhaseEnd && true == bIsEject)
		{
			FsmComp->ChangeState(EBossState::Phase3_Eject);
			bIsAllPhaseEnd = true;
			return;
		}

		// DrawDebugMesh();
		UpdateLerpRatioForLaserBeam(DeltaTime);
	}
}

void AEnemyFlyingSaucer::CorrectMayLocationAndRoation()
{
	PlayerMay->SetActorLocation(MayCorrectLocation);
	FVector LaserGunLocation = SkeletalMeshComp->GetBoneLocation(TEXT("LaserGunRing3"));
	FVector TargetLocation = LaserGunLocation - PlayerMay->GetActorLocation();
	TargetLocation.Z = 0.0f;
	FRotator TargetRotation = TargetLocation.Rotation();

	PlayerMay->SetActorRotation(FRotator(TargetRotation.Pitch, TargetRotation.Yaw, 0.0f));
}

void AEnemyFlyingSaucer::FireHomingRocket()
{
	if (HomingRocket1FireTime <= 0.0f && Cody_State::PlayerDeath != PlayerCody->GetITTPlayerState())
	{
		HomingRocketActor_1 = nullptr;
		APlayerBase* TargetActor = Cast<APlayerBase>(PlayerCody);
		HomingRocketActor_1 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
		HomingRocketActor_1->SetupTarget(TargetActor);
		HomingRocketActor_1->SetActorLocation(HomingRocketSpawnPointMesh1->GetComponentLocation());
		HomingRocketActor_1->SetOwner(this);
		HomingRocket1FireTime = HomingRocketCoolTime;
	}

	if (HomingRocket2FireTime <= 0.0f && Cody_State::PlayerDeath != PlayerMay->GetITTPlayerState())
	{
		HomingRocketActor_2 = nullptr;

		APlayerBase* TargetActor = Cast<APlayerBase>(PlayerMay);
		HomingRocketActor_2 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
		HomingRocketActor_2->SetupTarget(TargetActor);
		HomingRocketActor_2->SetActorLocation(HomingRocketSpawnPointMesh1->GetComponentLocation());
		HomingRocketActor_2->SetOwner(this);
		HomingRocket2FireTime = HomingRocketCoolTime;
	}
}

void AEnemyFlyingSaucer::FireArcingProjectile()
{
	AArcingProjectile* Projectile = GetWorld()->SpawnActor<AArcingProjectile>(ArcingProjectileClass, ArcingProjectileSpawnPointMesh->GetComponentLocation(), FRotator::ZeroRotator);
	Projectile->SetOwner(this);
	
	if (nullptr != Projectile)
	{
		FVector TargetLocation = FVector::ZeroVector;
		AActor* TargetActor = nullptr;
		
		if (0 == CurrentArcingProjectileTargetIndex)
		{
			TargetActor = PlayerCody;
			++CurrentArcingProjectileTargetIndex;
		}
		else
		{
			TargetActor = PlayerMay;
			CurrentArcingProjectileTargetIndex = 0;
		}

		// 타겟 위치세팅
		TargetLocation = TargetActor->GetActorLocation();
		Projectile->SetupTargetLocation(TargetLocation);
		Projectile->SetupProjectileMovementComponent();
		Projectile->AttachToActor(FloorObject, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void AEnemyFlyingSaucer::MulticastCreateGroundPoundEffect_Implementation()
{
	FVector TargetLocation = GetActorLocation();
	TargetLocation.Z = TargetLocation.Z - 1100.0f;
	AGroundPoundEffect* GroundPoundEffect = GetWorld()->SpawnActor<AGroundPoundEffect>(GroundPoundEffectClass, TargetLocation, FRotator::ZeroRotator);
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

void AEnemyFlyingSaucer::SpawnOverlapCheckActor()
{
	OverlapCheckActor = GetWorld()->SpawnActor<AOverlapCheckActor>(OverlapCheckActorClass, GetActorLocation(), GetActorRotation());
	OverlapCheckActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("OverlapCheckActorSocket"));
	OverlapCheckActor->SetOverlapActorNameTag(TEXT("Player"));
}

void AEnemyFlyingSaucer::UpdateLerpRatioForLaserBeam(float DeltaTime)
{
	LaserLerpRatio += DeltaTime * LaserLerpScale;
	if (1.0f <= LaserLerpRatio)
	{
		SavePreviousTargetLocation();
		LaserLerpRatio -= 1.0f;
		if (0.1f <= LaserLerpRatio)
		{
			LaserLerpRatio = 0.0f;
		}
	}
}

void AEnemyFlyingSaucer::SavePreviousTargetLocation()
{
	// 이전 레이저 타겟 액터가 존재할 경우 
	// 그럼 현재 대상은 타겟이 한번 변경된 대상인 거고. 
	if (nullptr != PrevLaserTargetActor)
	{
		// 이전 타겟이 데스상태가 아니라면 살아난거고. 
		if (Cody_State::PlayerDeath != PrevLaserTargetActor->GetITTPlayerState())
		{
			// 타겟을 다시 이전 타겟으로 세팅해주고 초기화.
			LaserTargetActor = PrevLaserTargetActor;
			PrevLaserTargetActor = nullptr;

			// 타겟 포커스. 
			MulticastSetFocusTarget();
		}
	}

	// 여기서 현재 타겟이 누구냐에 따라서 세팅될 수 있도록.
	if (nullptr != LaserTargetActor)
	{
		// 만약 현재 타겟액터가 데스 상태라면 
		if (Cody_State::PlayerDeath == LaserTargetActor->GetITTPlayerState())
		{
			/*Cody_State CodyPlayerState = LaserTargetActor->GetITTPlayerState();
			UE_LOG(LogTemp, Warning, TEXT("%d"), static_cast<int32>(CodyPlayerState));*/

			// 이전 레이저 타겟 액터에 저장
			PrevLaserTargetActor = LaserTargetActor;
			if (true == LaserTargetActor->ActorHasTag(TEXT("Cody")))
			{
				LaserTargetActor = PlayerMay;
				
			}
			else
			{
				LaserTargetActor = PlayerCody;
				
			}

			MulticastSetFocusTarget();
		}
		else
		{
			Cody_State CodyPlayerState = LaserTargetActor->GetITTPlayerState();
			
		}

		// 공격할 액터의 위치를 받아온다. 
		// 다음으로 공격할 위치일 거고. 
		CurrentLaserTargetLocation = LaserTargetActor->GetActorLocation();

		// 이전 타겟의 위치가 유효 하다면 ( 세팅 O ) 
		if (true == bPrevTargetLocationValid)
		{
			PrevLaserTargetLocation = NextLaserTargetLocation;
			NextLaserTargetLocation = CurrentLaserTargetLocation;
		}
		else
		{
			// 이전 타겟의 위치가 유효 하지않다면 ( 세팅 X )
			NextLaserTargetLocation = CurrentLaserTargetLocation;
			bPrevTargetLocationValid = true;
		}
	}
}

void AEnemyFlyingSaucer::SetupLaserTargetActor()
{
	switch (PatternDestroyCount)
	{
	case 0:
		if (nullptr != PlayerMay)
		{
			LaserTargetActor = PlayerMay;
		}
		break;
	case 1:
		if (nullptr != PlayerCody)
		{
			LaserTargetActor = PlayerCody;
		}
		break;
	case 2:
		if (nullptr != PlayerCody)
		{
			LaserTargetActor = PlayerMay;
		}
		break;
	}
}

void AEnemyFlyingSaucer::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (nullptr != OtherActor && true == OtherActor->ActorHasTag(TEXT("HomingRocket")))
	{
		// 로켓으로 cast 
		AHomingRocket* HomingRocket = Cast<AHomingRocket>(OtherActor);
		// State 확인 
		int32 RocketStateToInt = HomingRocket->GetCurrentState();
		// 플레이어 장착 state 라면 로켓을 
		if (static_cast<int32>(AHomingRocket::ERocketState::PlayerEquip) == RocketStateToInt)
		{
			UE_LOG(LogTemp, Warning, TEXT("Boss Rocket Hit True"));
		}
	}
}

void AEnemyFlyingSaucer::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AEnemyFlyingSaucer::SetupOverlapEvent()
{
	if (nullptr != SkeletalMeshComp)
	{
		SkeletalMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyFlyingSaucer::OnOverlapBegin);
		SkeletalMeshComp->OnComponentEndOverlap.AddDynamic(this, &AEnemyFlyingSaucer::OnOverlapEnd);
	}
}

void AEnemyFlyingSaucer::SetupPlayerActorsCodyAndMay()
{
	// 현재 월드의 액터를 받아오고 
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerBase::StaticClass(), PlayerActors);
	if (0 == PlayerActors.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Actors Size : 0"));
		return;
	}

	// 순회해서 세팅해주는 걸로 변경
	for (AActor* Actor : PlayerActors)
	{
		if (true == Actor->ActorHasTag(TEXT("Cody")))
		{
			PlayerCody = Cast<ACody>(Actor);
			break;
		}
	}

	for (AActor* Actor : PlayerActors)
	{
		if (true == Actor->ActorHasTag(TEXT("May")))
		{
			PlayerMay = Cast<AMay>(Actor);
			break;
		}
	}
}

int32 AEnemyFlyingSaucer::GetFloorCurrentState()
{
	if (nullptr == FloorObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Floor Object is nullptr"));
	}

	return FloorObject->GetCurrentPhase();
}

void AEnemyFlyingSaucer::EnableCutSceneCameraBlend(APlayerBase* BlendTargetActor, APhaseEndCameraRail* CameraRail, const float BlendTime, const float BlendRatio)
{
	if (nullptr == BlendTargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("CameraBlend TargetActor is nullptr"));
		return;
	}

	AController* PlayerController = BlendTargetActor->GetController();
	if (nullptr != PlayerController)
	{
		ViewTargetChangeController = Cast<APlayerController>(PlayerController);
		if (nullptr != ViewTargetChangeController)
		{
			// 컨트롤러의 현재 뷰타겟액터 저장
			PrevViewTarget = ViewTargetChangeController->GetViewTarget();
			if (nullptr != PrevViewTarget)
			{
				// 카메라 변경 후 재생비율 세팅
				ViewTargetChangeController->SetViewTargetWithBlend(Cast<AActor>(CameraRail), BlendTime);
				CameraRail->MulticastEnableCameraMove(BlendRatio);

				UE_LOG(LogTemp, Warning, TEXT("Camera Blend Start"));
			}
		}
	}

	bIsCutSceneStart = true;
}

void AEnemyFlyingSaucer::DisableCutSceneCameraBlend(AActor* PrevViewTargetActor, const float BlendTime)
{
	if (nullptr == PrevViewTargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("PrevViewTargetActor is nullptr"));
		return;
	}

	if (nullptr == ViewTargetChangeController)
	{
		UE_LOG(LogTemp, Warning, TEXT("ViewTargetChangeController is nullptr"));
		return;
	}

	ViewTargetChangeController->SetViewTargetWithBlend(PrevViewTargetActor, BlendTime);
	ViewTargetChangeController = nullptr;
	bIsCutSceneStart = false;
}



///////////////////////////////////FSM//////////////////////////////////////




void AEnemyFlyingSaucer::SetupFsmComponent()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComponent"));
	FsmComp->CreateState(EBossState::None,
		[this]
		{
			
		},

		[this](float DT)
		{
			if (true == bIsFsmStart)
			{
				ServerDelayTime -= DT;
				if (ServerDelayTime <= 0.0f)
				{
					FsmComp->ChangeState(EBossState::Phase1_LaserBeam_1);
					AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
					AIController->GetBlackboardComponent()->SetValueAsBool(TEXT("bIsFsmStart"), true);
					return;
				}
			}
		},

		[this]
		{
			MulticastAttachMoonBaboonActorWithFloor();
		});

	FsmComp->CreateState(EBossState::Phase1_LaserBeam_1,
		[this]
		{
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},

		[this](float DT)
		{
			if (1 == PatternDestroyCount)
			{
				FsmComp->ChangeState(EBossState::Phase1_LaserBeam_1_Destroy);
				return;
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase1_LaserBeam_1_Destroy,
		[this]
		{
			SetDamage(CoreExplodeDamage);
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Laser_HitPod_Anim"), 1, false);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Laser_HitPod_Anim"), 1, false);
		},

		[this](float DT)
		{
			USkeletalMeshComponent* MoonBaboonMesh = EnemyMoonBaboon->GetMesh();
			if (false == SkeletalMeshComp->IsPlaying() && false == MoonBaboonMesh->IsPlaying())
			{
				FsmComp->ChangeState(EBossState::Phase1_ArcingProjectile_1);
				return;
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase1_ArcingProjectile_1,
		[this]
		{
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},

		[this](float DT)
		{
			// 바닥의 상태값에 따라서 changestate 
			int32 CurrentFloorPhaseToInt = static_cast<int32>(AFloor::Fsm::Phase1_2);
			if (CurrentFloorPhaseToInt == GetFloorCurrentState())
			{
				FsmComp->ChangeState(EBossState::Phase1_LaserBeam_2);
				return;
			}

			ArcingProjectileFireTime -= DT;
			if (ArcingProjectileFireTime <= 0.0f)
			{
				FireArcingProjectile();
				ArcingProjectileFireTime = ArcingProjectileMaxFireTime;
			}
		},

		[this]
		{
			ArcingProjectileFireTime = ArcingProjectileMaxFireTime;
		});

	FsmComp->CreateState(EBossState::Phase1_LaserBeam_2,
		[this]
		{
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},

		[this](float DT)
		{
			if (2 == PatternDestroyCount)
			{
				FsmComp->ChangeState(EBossState::Phase1_LaserBeam_2_Destroy);
				return;
			}
		},

		[this]
		{
			
		});


	FsmComp->CreateState(EBossState::Phase1_LaserBeam_2_Destroy,
		[this]
		{
			SetDamage(CoreExplodeDamage);
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Laser_HitPod_Anim"), 1, false);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Laser_HitPod_Anim"), 1, false);
		},

		[this](float DT)
		{
			USkeletalMeshComponent* MoonBaboonMesh = EnemyMoonBaboon->GetMesh();
			if (false == SkeletalMeshComp->IsPlaying() && false == MoonBaboonMesh->IsPlaying())
			{
				FsmComp->ChangeState(EBossState::Phase1_ArcingProjectile_2);
				return;
			}
		},

		[this]
		{

		});


	FsmComp->CreateState(EBossState::Phase1_ArcingProjectile_2,
		[this]
		{


			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},

		[this](float DT)
		{
			int32 CurrentFloorPhaseToInt = static_cast<int32>(AFloor::Fsm::Phase1_3);
			if (CurrentFloorPhaseToInt == GetFloorCurrentState())
			{
				FsmComp->ChangeState(EBossState::Phase1_LaserBeam_3);
				return;
			}

			ArcingProjectileFireTime -= DT;
			if (ArcingProjectileFireTime <= 0.0f)
			{
				FireArcingProjectile();
				ArcingProjectileFireTime = ArcingProjectileMaxFireTime;
			}
		},

		[this]
		{
			ArcingProjectileFireTime = ArcingProjectileMaxFireTime;
		});


	FsmComp->CreateState(EBossState::Phase1_LaserBeam_3,
		[this]
		{
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},

		[this](float DT)
		{
			if (3 == PatternDestroyCount)
			{
				// UE_LOG(LogTemp, Warning, TEXT("Change State : Phase1_BreakThePattern"));
				FsmComp->ChangeState(EBossState::Phase1_BreakThePattern);
				return;
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase1_BreakThePattern,
		[this]
		{
			SetDamage(CoreExplodeDamage);

			// 추락애니메이션 재생
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/CutScenes/PlayRoom_SpaceStation_BossFight_PowerCoresDestroyed_FlyingSaucer_Anim"), 1, false);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Programming_Anim"), 1, true);

			// UI Component Activate
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUFunction(this, TEXT("MulticastSetActivateUIComponent"), CodyHoldingUIComp, true, true);
			GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 4.5f, false);

			// overlap actor spawn
			FTimerHandle TimerHandle2;
			GetWorldTimerManager().SetTimer(TimerHandle2, this, &AEnemyFlyingSaucer::SpawnOverlapCheckActor, 4.5f, false);

			// 카메라블렌드 
			EnableCutSceneCameraBlend(PlayerCody, PowerCoreDestroyCameraRail, 0.2f, 0.25f);

			// 포커스 해제 
			AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
			AIController->ClearFocus(EAIFocusPriority::Gameplay);
		},

		[this](float DT)
		{
			if (nullptr == OverlapCheckActor)
			{
				return;
			}

			if (true == PowerCoreDestroyCameraRail->IsMoveEnd())
			{
				if (nullptr != ViewTargetChangeController)
				{
					DisableCutSceneCameraBlend(PrevViewTarget, 0.2f);
				}
			}

			APlayerBase* CurrentOverlapPlayer = OverlapCheckActor->GetCurrentOverlapPlayer();
			if (nullptr != CurrentOverlapPlayer)
			{
				if (true == CurrentOverlapPlayer->ActorHasTag("Cody"))
				{
					PlayerCody = Cast<ACody>(CurrentOverlapPlayer);
					if (nullptr != PlayerCody)
					{
						// 코디가 커진 상태인것도 체크 
						bool bIsInteract = CurrentOverlapPlayer->GetIsInteract();
						if (true == bIsInteract)
						{
							MulticastCheckCodyKeyPressedAndChangeState(bIsInteract);
						}
					}
				}
			}
		},

		[this]
		{
			
		});

	// 코디가 널부러져있는 보스 살짝 들기 수행
	FsmComp->CreateState(EBossState::CodyHolding_Enter,
		[this]
		{
			// 시작할때 한번 돌리고 끝나고 한번 더돌려 
			FVector TargetLocation = GetActorLocation();
			TargetLocation.Z = 0.0f;
			FVector StartLocation = PlayerCody->GetActorLocation();
			StartLocation.Z = 0.0f;

			FRotator TargetRotation = (TargetLocation - StartLocation).Rotation();
			TargetRotation.Yaw -= 14.0f;
			PlayerCody->SetActorRotation(TargetRotation);

			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_CodyHolding_Enter_Anim"), 1, false);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_CodyHolding_Enter_Anim"), 1, false);
			// UE_LOG(LogTemp, Warning, TEXT("Set Cody Lerp Timer"));
		},

		[this](float DT)
		{
			// 둘다 애니메이션 종료됐으면, Low, 애니메이션 반복재생상태로 변경
			USkeletalMeshComponent* MoonBaboonMesh = EnemyMoonBaboon->GetMesh();
			if (false == SkeletalMeshComp->IsPlaying() && false == MoonBaboonMesh->IsPlaying() && true == bIsCodyHoldingLerpEnd)
			{
				FsmComp->ChangeState(EBossState::CodyHolding_Low);
				return;
			}

			CorrectCodyLocationAndRotation();
		},

		[this]
		{
			bIsCodyHoldingEnter = false;
			bIsCodyHoldingLerpEnd = false;

			// MulticastUnPossess();
		});

	// 코디가 살짝 들고 있는 상태 
	FsmComp->CreateState(EBossState::CodyHolding_Low,
		[this]
		{
			// UFO 각도변경 애니메이션 
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_CodyHolding_Low_Anim"), 1, true);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_CodyHolding_Anim"), 1, true);
		},

		[this](float DT)
		{
			if (nullptr == OverlapCheckActor->GetCurrentOverlapPlayer())
			{
				return;
			}

			bool IsPressed = PlayerCody->GetIsInteract();
			if (true == IsPressed)
			{
				FsmComp->ChangeState(EBossState::CodyHolding_ChangeOfAngle);
				UE_LOG(LogTemp, Warning, TEXT("Cody Input"));
				return;
			}
		},

		[this]
		{
			
		});


	FsmComp->CreateState(EBossState::CodyHolding_ChangeOfAngle,
		[this]
		{
			// ufo 각도 변경 애니메이션 재생
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/BluePrints/ABP_EnemyFlyingSaucer_CodyHoldingRotation"), 2, false);
			StateCompleteTime = 2.5f;
		},

		[this](float DT)
		{
			if (StateCompleteTime <= FsmComp->GetStateLiveTime())
			{
				FsmComp->ChangeState(EBossState::CodyHolding_InputKey);
				StateCompleteTime = 0.0f;
				return;
			}
		},

		[this]
		{
			
		});

	FsmComp->CreateState(EBossState::CodyHolding_InputKey,
		[this]
		{
			// ufo 각도 변경 애니메이션 재생
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_CodyHolding_Anim"), 1, true);
			bIsKeyInput = true;
			KeyInputTime = KeyInputMaxTime;

			// 여기들어왔을때 메이 UI On
			MulticastSetActivateUIComponent(MayLaserDestroyUIComp, true, true);


			// 임시코드 
			/*AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
			AIController->ClearFocus(EAIFocusPriority::Gameplay);*/
		},

		[this](float DT)
		{
			if (KeyInputTime <= 0.0f)
			{
				FsmComp->ChangeState(EBossState::CodyHolding_ChangeOfAngle_Reverse);
				return;
			}
				
			bool CodyInput = PlayerCody->GetIsInteract();
			if (true == CodyInput)
			{
				KeyInputTime = KeyInputAdditionalTime;
			}

			APlayerBase* OverlapPlayer = OverlapCheckActor->GetCurrentOverlapPlayer();
			if (OverlapPlayer != nullptr)
			{
				if (true == OverlapPlayer->ActorHasTag("May"))
				{
					UE_LOG(LogTemp, Warning, TEXT("May Overlap"));
					PlayerMay = Cast<AMay>(OverlapPlayer);
					bool MayInput = PlayerMay->GetIsInteract();
					if (true == MayInput)
					{
						UE_LOG(LogTemp, Warning, TEXT("Key Input True"));
						FsmComp->ChangeState(EBossState::Phase1_ChangePhase_2);
						return;
					}
				}
			}
			KeyInputTime -= DT;
		},

		[this]
		{
			bIsKeyInput = false;
			KeyInputTime = KeyInputMaxTime;
			MulticastSetActivateUIComponent(CodyHoldingUIComp, false, true);
			MulticastSetActivateUIComponent(MayLaserDestroyUIComp, false, true);
			
			PrevAnimBoneLocation = SkeletalMeshComp->GetBoneLocation(TEXT("Base"));
		});

	FsmComp->CreateState(EBossState::CodyHolding_ChangeOfAngle_Reverse,
		[this]
		{
			// 고각도 -> 저각도 애니메이션 
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/BluePrints/ABP_EnemyFlyingSaucer_CodyHoldingRotationReverse"), 2, false);
			StateCompleteTime = 2.0f;
		},

		[this](float DT)
		{
			if (StateCompleteTime <= FsmComp->GetStateLiveTime())
			{
				FsmComp->ChangeState(EBossState::CodyHolding_Low);
				StateCompleteTime = 0.0f;
				return;
			}
		},

		[this]
		{
			
		});

	
	FsmComp->CreateState(EBossState::Phase1_ChangePhase_2,
		[this]
		{
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/PlayRoom_SpaceStation_BossFight_LaserRippedOff_FlyingSaucer_Anim"), 1, false);
			PlayerCody->CutScenceStart();
			PlayerMay->CutSceneStart();
			CorrectMayLocationAndRoation();

			// 카메라블렌드 
			EnableCutSceneCameraBlend(PlayerMay, LaserDestroyCameraRail, 0.2f, 0.23f);
		},

		[this](float DT)
		{
			// 애니메이션이 종료 되었을 때 
			if (false == SkeletalMeshComp->IsPlaying())
			{
				MulticastHideLaserBaseBoneAndSpawnDestroyEffect();
				FsmComp->ChangeState(EBossState::Phase2_RotateSetting);
				PrevAnimBoneLocation = SkeletalMeshComp->GetBoneLocation(TEXT("Root"));
				return;
			}

			if (false == bIsCorretLocation)
			{
				SetActorLocation(PrevAnimBoneLocation);
				bIsCorretLocation = true;
			}
		},

		[this]
		{
			bIsCorretLocation = false;
			OverlapCheckActor->Destroy();

			DisableCutSceneCameraBlend(PrevViewTarget, 0.2f);
		});

	
	FsmComp->CreateState(EBossState::Phase2_RotateSetting,
		[this]
		{
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Left_Anim"), 1, true);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},

		[this](float DT)
		{
			// 보스 위치보정 적용안되어 있으면 적용하고. 로테이팅컴포넌트 피봇 세팅 
			if (false == bIsCorretLocation)
			{
				SetActorLocation(PrevAnimBoneLocation);
				bIsCorretLocation = true;
			}

			FsmComp->ChangeState(EBossState::Phase2_Rotating);
		},

		[this]
		{
			// 타겟의 위치를 받아오고. 
			FVector TargetLocation = FloorObject->GetActorLocation() - GetActorLocation();
			TargetLocation = TargetLocation.RotateAngleAxis(-(GetActorRotation().Yaw), FVector::UpVector);
			RotatingComp->PivotTranslation = TargetLocation;
			RotatingComp->RotationRate = FRotator(0.0f, 7.0f, 0.0f);
			bIsCorretLocation = false;
		});

	FsmComp->CreateState(EBossState::Phase2_Rotating,
		[this]
		{
			// 만약 이전 스테이트가 미사일 히트 스테이트 였다면 애니메이션 재생 
			if (static_cast<int32>(EBossState::Phase2_RocketHit) == FsmComp->GetCurrentState())
			{
				MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Left_Anim"), 1, true);
			}
		},

		[this](float DT)
		{
			if (CurrentHp <= 33.0f)
			{
				FsmComp->ChangeState(EBossState::Phase2_BreakThePattern);
				return;
			}

			if (true == bIsRocketHit)
			{
				FsmComp->ChangeState(EBossState::Phase2_RocketHit);
				return;
			}

			HomingRocket1FireTime -= DT;
			HomingRocket2FireTime -= DT;
			FireHomingRocket();

		},

		[this]
		{
			HomingRocket1FireTime = HomingRocketCoolTime;
			HomingRocket2FireTime = HomingRocketCoolTime;
		});

	FsmComp->CreateState(EBossState::Phase2_RocketHit,
		[this]
		{
			// 우주선 히트 애니메이션 적용 
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Laser_HitPod_Anim"), 1, false);
		},

		[this](float DT)
		{
			// 애니메이션 종료시 Phase2_Rotating으로 상태변경
			if (false == SkeletalMeshComp->IsPlaying())
			{
				FsmComp->ChangeState(EBossState::Phase2_Rotating);
				return;
			}
		},

		[this]
		{
			bIsRocketHit = false;
		});



	FsmComp->CreateState(EBossState::Phase2_BreakThePattern,
		[this]
		{
			RotatingComp->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/BluePrints/ABP_EnemyFlyingSaucer_RocketPhaseEnd"), 2, false);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/CutScenes/PlayRoom_SpaceStation_BossFight_RocketsPhaseFinished_MoonBaboon_Anim"), 1, false);
			

			// 카메라블렌드 
			EnableCutSceneCameraBlend(PlayerMay, BossFallCameraRail, 0.2f, 0.25f);
		},

		[this](float DT)
		{
			if (7.5f <= FsmComp->GetStateLiveTime())
			{
				FsmComp->ChangeState(EBossState::Phase2_ChangePhase_Wait);
				return;
			}
		},

		[this]
		{
			DisableCutSceneCameraBlend(PrevViewTarget, 0.2f);
		});

	FsmComp->CreateState(EBossState::Phase2_ChangePhase_Wait,
		[this]
		{
			// 원숭이 타자열심히치는 애니메이션으로 변경
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_KnockDownMh_Anim"), 1, true);
		},

		[this](float DT)
		{
			// 조건만족시 
			
			// 임시,
			if (2.0f <= FsmComp->GetStateLiveTime())
			{
				FsmComp->ChangeState(EBossState::Phase2_Fly);
				return;
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase2_Fly,
		[this]
		{
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/CutScenes/PlayRoom_SpaceStation_BossFight_EnterUFO_FlyingSaucer_Anim"), 1, false);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/CutScenes/PlayRoom_SpaceStation_BossFight_EnterUFO_MoonBaboon_Anim"), 1, false);
		},

		[this](float DT)
		{
			// 우주선 애니메이션 재생 완료시 중앙으로 이동
			if (false == SkeletalMeshComp->IsPlaying())
			{
				FsmComp->ChangeState(EBossState::Phase2_MoveToCenter);
				return;
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase2_MoveToCenter,
		[this]
		{
			// 우주선 포워드이동 애니메이션 적용, 
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Fwd_Anim"), 1, false);
			MoveStartLocation = GetActorLocation();
		},

		[this](float DT)
		{
			MoveToCenterLerpRatio += DT / 2.0f;
			if (1.0f <= MoveToCenterLerpRatio)
			{
				MoveToCenterLerpRatio = 1.0f;
				FVector TargetLocation = FMath::Lerp(MoveStartLocation, FVector(0.0f, 0.0f, MoveStartLocation.Z), MoveToCenterLerpRatio);
				(TargetLocation); // ???? 뭐지이건 
				FsmComp->ChangeState(EBossState::Phase3_MoveFloor);
				FloorObject->SetPhase(AFloor::Fsm::Phase3);
				return;
			}
			
			FVector TargetLocation = FMath::Lerp(MoveStartLocation, FVector(0.0f, 0.0f, MoveStartLocation.Z), MoveToCenterLerpRatio);
			SetActorLocation(TargetLocation);
		},

		[this]
		{
			MoveToCenterLerpRatio = 0.0f;
			MoveStartLocation = FVector::ZeroVector;
			// 중앙으로 이동 종료 되었을 때 원숭이 루프애니메이션 적용
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		});

	FsmComp->CreateState(EBossState::Phase3_MoveFloor,
		[this]
		{
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},
		
		[this](float DT)
		{
			if (static_cast<int32>(AFloor::Fsm::KeepPhase) == FloorObject->GetCurrentPhase())
			{
				FsmComp->ChangeState(EBossState::Phase3_MoveToTarget);
			}

		},

		[this]
		{
			UE_LOG(LogTemp, Warning, TEXT("Phase3_MoveToTarget Start"));
		});

	FsmComp->CreateState(EBossState::Phase3_MoveToTarget,
		[this]
		{
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Fwd_Anim"), 1, false);
			MoveStartLocation = GetActorLocation();
			GroundPoundTargetLocation = PlayerCody->GetActorLocation();
			GroundPoundTargetLocation.Z = MoveStartLocation.Z;

			AController* BossController = GetController();
			if (nullptr == BossController)
			{
				UE_LOG(LogTemp, Warning, TEXT("Boss Controller is nullptr"));
				return;
			}

			// 포커스는 임시로 코디 설정, 
			AFlyingSaucerAIController* Controller = Cast<AFlyingSaucerAIController>(GetController());
			if (nullptr != Controller)
			{
				Controller->SetFocus(Cast<AActor>(PlayerCody));
			}
		},

		[this](float DT)
		{
			MoveToTargetLerpRatio += DT;
			if (1.0f <= MoveToTargetLerpRatio)
			{
				MoveToTargetLerpRatio = 1.0f;
				FVector TargetLocation = FMath::Lerp(MoveStartLocation, GroundPoundTargetLocation, MoveToTargetLerpRatio);
				SetActorLocation(TargetLocation);
				FsmComp->ChangeState(EBossState::Phase3_GroundPounding);
				return;
			}

			FVector TargetLocation = FMath::Lerp(MoveStartLocation, GroundPoundTargetLocation, MoveToTargetLerpRatio);
			SetActorLocation(TargetLocation);
		},

		[this]
		{
			MoveToTargetLerpRatio = 0.0f;
			MoveStartLocation = FVector::ZeroVector;
		});

	FsmComp->CreateState(EBossState::Phase3_GroundPounding,
		[this]
		{
			// 그라운드파운딩 애니메이션 
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_GroundPound_Anim"), 1, false);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_GroundPound_Anim"), 1, false);
		},

		[this](float DT)
		{
			if (false == SkeletalMeshComp->IsPlaying())
			{
				FsmComp->ChangeState(EBossState::Phase3_MoveToTarget);
				return;
			}

			if (GroundPoundEffectCreateTime <= FsmComp->GetStateLiveTime() && false == bIsSetGroundPoundEffect)
			{
				MulticastCreateGroundPoundEffect();
				bIsSetGroundPoundEffect = true;
			}

		},

		[this]
		{
			bIsSetGroundPoundEffect = false;
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		});

	FsmComp->CreateState(EBossState::Phase3_Eject,
		[this]
		{
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/CutScenes/PlayRoom_SpaceStation_BossFight_Eject_FlyingSaucer_Anim"), 1, false);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Programming_Anim.MoonBaboon_Ufo_Programming_Anim"), 1, false);
			
			EnableCutSceneCameraBlend(PlayerMay, BossEjectCameraRail, 0.2f, 0.35f);
		},

		[this](float DT)
		{
			// 애니메이션 재생 완료시 페이즈 종료 상태로 전환, 아무작업도 수행 X 
			// 여기서 되돌리지말고 그냥 End; 
			if (6.0f <= FsmComp->GetStateLiveTime())
			{
				FsmComp->ChangeState(EBossState::AllPhaseEnd);
				return;
			}
		},

		[this]
		{
			DisableCutSceneCameraBlend(PrevViewTarget, 0.2f);
		});

	FsmComp->CreateState(EBossState::AllPhaseEnd,
		[this]
		{
		},

		[this](float DT)
		{
		},

		[this]
		{
			
		});


	// TEST 전용 state 
	FsmComp->CreateState(EBossState::TestState,
		[this]
		{
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/PlayRoom_SpaceStation_BossFight_LaserRippedOff_FlyingSaucer_Anim"), 1, false);
			AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
			AIController->ClearFocus(EAIFocusPriority::Gameplay);

			CorrectMayLocationAndRoation();
			PlayerMay->CutSceneStart();

			// 카메라블렌드 
			EnableCutSceneCameraBlend(PlayerMay, LaserDestroyCameraRail, 0.2f, 0.2f);
		},

		[this](float DT)
		{
			// 애니메이션이 종료 되었을 때 
			if (false == SkeletalMeshComp->IsPlaying())
			{
				MulticastHideLaserBaseBoneAndSpawnDestroyEffect();
				FsmComp->ChangeState(EBossState::Phase2_RotateSetting);
				PrevAnimBoneLocation = SkeletalMeshComp->GetBoneLocation(TEXT("Root"));
				return;
			}
		},

		[this]
		{
		});
}


