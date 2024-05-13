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

	// 메시 컴포넌트를 Replication하기 위한 설정 추가
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
	DOREPLIFETIME(AEnemyFlyingSaucer, PrevTargetLocation);
	DOREPLIFETIME(AEnemyFlyingSaucer, PrevTargetLocationBuffer);
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

void AEnemyFlyingSaucer::MulticastHideLaserBaseBone_Implementation()
{
	// 레이저 본 렌더링 off
	int32 BoneIndex = SkeletalMeshComp->GetBoneIndex(TEXT("LaserBase"));
	if (INDEX_NONE != BoneIndex)
	{
		SkeletalMeshComp->HideBone(BoneIndex, EPhysBodyOp::PBO_Term);
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

void AEnemyFlyingSaucer::FireHomingRocket()
{
	// 플레이어 데스상태라면 발사 안하는 로직 추가 필요 

	// 로켓 1, 초기 한발은 생성 후 발사, 이후부터는 시간계산하여 미사일발사. 
	if (nullptr == HomingRocketActor_1)
	{
		APlayerBase* TargetActor = Cast<APlayerBase>(PlayerActors[0]);
		HomingRocketActor_1 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
		HomingRocketActor_1->SetupTarget(TargetActor);
		HomingRocketActor_1->SetActorLocation(HomingRocketSpawnPointMesh1->GetComponentLocation());
		HomingRocketActor_1->SetOwner(this);
	}

	else if (nullptr != HomingRocketActor_1 && static_cast<int32>(AHomingRocket::ERocketState::DestroyWait) == HomingRocketActor_1->GetCurrentState())
	{
		HomingRocketActor_1->DestroyRocket();

		APlayerBase* TargetActor = Cast<APlayerBase>(PlayerActors[0]);
		HomingRocketActor_1 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
		HomingRocketActor_1->SetupTarget(TargetActor);
		HomingRocketActor_1->SetActorLocation(HomingRocketSpawnPointMesh1->GetComponentLocation());
		HomingRocketActor_1->SetOwner(this);
	}


	// 로켓 2
	if (nullptr == HomingRocketActor_2)
	{
		APlayerBase* TargetActor = Cast<APlayerBase>(PlayerActors[1]);
		HomingRocketActor_2 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
		HomingRocketActor_2->SetupTarget(TargetActor);
		HomingRocketActor_2->SetActorLocation(HomingRocketSpawnPointMesh2->GetComponentLocation());
		HomingRocketActor_2->SetOwner(this);
	}

	else if (nullptr != HomingRocketActor_2 && static_cast<int32>(AHomingRocket::ERocketState::DestroyWait) == HomingRocketActor_2->GetCurrentState())
	{
		HomingRocketActor_2->DestroyRocket();

		APlayerBase* TargetActor = Cast<APlayerBase>(PlayerActors[1]);
		HomingRocketActor_2 = GetWorld()->SpawnActor<AHomingRocket>(HomingRocketClass);
		HomingRocketActor_2->SetupTarget(TargetActor);
		HomingRocketActor_2->SetActorLocation(HomingRocketSpawnPointMesh2->GetComponentLocation());
		HomingRocketActor_2->SetOwner(this);
	}
}

void AEnemyFlyingSaucer::FireArcingProjectile()
{
	if (nullptr == FloorObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("FloorObject nullptr"));
	}

	// 투사체 생성
	AArcingProjectile* Projectile = GetWorld()->SpawnActor<AArcingProjectile>(ArcingProjectileClass, ArcingProjectileSpawnPointMesh->GetComponentLocation(), FRotator::ZeroRotator);
	Projectile->SetOwner(this);
	
	// 투사체와 컨트롤러모두 nullptr 이 아니라면 
	if (nullptr != Projectile)
	{
		FVector TargetLocation = FVector::ZeroVector;
		AActor* TargetActor = nullptr;
		
		// 번갈아가면서 타겟 지정
		if (0 == CurrentArcingProjectileTargetIndex)
		{
			TargetActor = PlayerActors[0];
			++CurrentArcingProjectileTargetIndex;
		}
		else
		{
			TargetActor = PlayerActors[1];
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
	LaserLerpRatio += DeltaTime * LaserLerpRate;
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
	// 여기서 현재 타겟이 누구냐에 따라서 세팅될 수 있도록.
	if (nullptr != LaserTargetActor)
	{
		FVector CurrentTargetLocation = LaserTargetActor->GetActorLocation();

		// 이전 타겟 위치가 유효하다면 
		if (true == bPrevTargetLocationValid)
		{
			// 타겟 위치는 저장되어있는 이전타겟위치로 지정하고 false 처리
			PrevTargetLocation = PrevTargetLocationBuffer;
			bPrevTargetLocationValid = false;
		}

		else
		{
			// 유효하지 않다면 타겟 위치는 현재 위치로 지정
			PrevTargetLocation = CurrentTargetLocation;
		}

		// 타겟위치를 세팅
		PrevTargetLocationBuffer = CurrentTargetLocation;
		bPrevTargetLocationValid = true;
	}
}

void AEnemyFlyingSaucer::SetupLaserTargetActor()
{
	switch (PatternDestroyCount)
	{
	case 0:
		for (AActor* Actor : PlayerActors)
		{
			if (nullptr != Actor && Actor->ActorHasTag(TEXT("May")))
			{
				LaserTargetActor = Actor;
			}
		}

		break;
	case 1:
		for (AActor* Actor : PlayerActors)
		{
			if (nullptr != Actor && Actor->ActorHasTag(TEXT("Cody")))
			{
				LaserTargetActor = Actor;
			}
		}
		break;
	case 2:
		for (AActor* Actor : PlayerActors)
		{
			if (nullptr != Actor && Actor->ActorHasTag(TEXT("May")))
			{
				LaserTargetActor = Actor;
			}
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
	// 일단.. 오버랩 대상이 플레이어이고.. 플라이상태면.. .. 일단들어오는지부터
	// UE_LOG(LogTemp, Warning, TEXT("Boss Overlap Begin End Check"));
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

	// 0번이 코디면 그대로 세팅, 코디가 아니라면 반대로 세팅. 
	if (true == PlayerActors[0]->ActorHasTag("Cody"))
	{
		PlayerCody = Cast<ACody>(PlayerActors[0]);
		PlayerMay = Cast<AMay>(PlayerActors[1]);
	}
	else
	{
		PlayerCody = Cast<ACody>(PlayerActors[1]);
		PlayerMay = Cast<AMay>(PlayerActors[0]);
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
			// 서버 클라 연동 지연 문제로 인해 스테이트 변경 딜레이 추가 
			if (ServerDelayTime <= FsmComp->GetStateLiveTime())
			{
				FsmComp->ChangeState(EBossState::Phase1_BreakThePattern);
				return;
			}
		},

		[this]
		{
			MulticastAttachMoonBaboonActorWithFloor();
			SetupPlayerActorsCodyAndMay();
		});

	FsmComp->CreateState(EBossState::Phase1_Progress_LaserBeam_1,
		[this]
		{
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},

		[this](float DT)
		{
			if (1 == PatternDestroyCount)
			{
				FsmComp->ChangeState(EBossState::Phase1_Progress_LaserBeam_1_Destroy);
				return;
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase1_Progress_LaserBeam_1_Destroy,
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
				FsmComp->ChangeState(EBossState::Phase1_Progress_ArcingProjectile_1);
				return;
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(EBossState::Phase1_Progress_ArcingProjectile_1,
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
				FsmComp->ChangeState(EBossState::Phase1_Progress_LaserBeam_2);
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

	FsmComp->CreateState(EBossState::Phase1_Progress_LaserBeam_2,
		[this]
		{
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/Animations/FlyingSaucer_Ufo_Mh_Anim"), 1, true);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Anim"), 1, true);
		},

		[this](float DT)
		{
			if (2 == PatternDestroyCount)
			{
				FsmComp->ChangeState(EBossState::Phase1_Progress_LaserBeam_2_Destroy);
				return;
			}
		},

		[this]
		{
			
		});


	FsmComp->CreateState(EBossState::Phase1_Progress_LaserBeam_2_Destroy,
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
				FsmComp->ChangeState(EBossState::Phase1_Progress_ArcingProjectile_2);
				return;
			}
		},

		[this]
		{

		});


	FsmComp->CreateState(EBossState::Phase1_Progress_ArcingProjectile_2,
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
				FsmComp->ChangeState(EBossState::Phase1_Progress_LaserBeam_3);
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


	FsmComp->CreateState(EBossState::Phase1_Progress_LaserBeam_3,
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
			// 추락애니메이션 재생
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/CutScenes/PlayRoom_SpaceStation_BossFight_PowerCoresDestroyed_FlyingSaucer_Anim"), 1, false);
			MulticastChangeAnimationMoonBaboon(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Programming_Anim"), 1, true);

			// UI Component Activate
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUFunction(this, TEXT("MulticastSetActivateUIComponent"), CodyHoldingUIComp, true, true);
			GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 4.5f, false);

			FTimerHandle TimerHandle2;
			GetWorldTimerManager().SetTimer(TimerHandle2, this, &AEnemyFlyingSaucer::SpawnOverlapCheckActor, 4.5f, false);

			// 임시로 0번 플레이어 카메라만 블렌드 처리 
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (nullptr == PlayerController)
			{
				UE_LOG(LogTemp, Warning, TEXT("PlayerController is nullptr"));
				return;
			}

			// 플레이어 컨트롤러 세팅 
			ViewTargetChangeController = PlayerController;
			// 플레이어 컨트롤러의 이전 카메라 액터 저장
			AActor* PrevCameraActor = ViewTargetChangeController->GetViewTarget();
			if (nullptr != PrevCameraActor)
			{
				PrevViewTarget = PrevCameraActor;
			}

			// 임시주석
			// 카메라 변경 후 재생비율 세팅
			/*ViewTargetChangeController->SetViewTargetWithBlend(Cast<AActor>(Phase1EndCameraRail), 0.2f);
			Phase1EndCameraRail->EnableCameraMove(0.25f);*/

			AFlyingSaucerAIController* AIController = Cast<AFlyingSaucerAIController>(GetController());
			AIController->ClearFocus(EAIFocusPriority::Gameplay);
		},

		[this](float DT)
		{
			if (nullptr == OverlapCheckActor)
			{
				return;
			}

			//if (true == Phase1EndCameraRail->IsMoveEnd())
			//{
			//	if (nullptr != ViewTargetChangeController)
			//	{
			//		// 기존 카메라로 뷰타겟 변경 후 nullptr 처리 
			//		ViewTargetChangeController->SetViewTargetWithBlend(PrevViewTarget, 0.2f);
			//		ViewTargetChangeController = nullptr;
			//	}
			//}

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
			// 여기들어왔다는건 이미 오버랩플레이어가 있다는 거고. 따로 검사 할필요 없이 키를 한번 더 눌렀다면 키입력 상태로 전환 
			// 근데 지금 플레이어 위치고정을 안한상태니까 nullptr 검사 한번 해주고 수정할거임~~ 
			if (nullptr == OverlapCheckActor->GetCurrentOverlapPlayer())
			{
				return;
			}

			bool IsPressed = PlayerCody->GetIsInteract();
			if (true == IsPressed)
			{
				FsmComp->ChangeState(EBossState::CodyHolding_ChangeOfAngle);
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

			// 원래대로라면 메이 인터랙트 체크 필요. 
			// 페이즈 넘기기용 테스트 코드 
			if (true)
			{
				MulticastSetActivateUIComponent(CodyHoldingUIComp, false, true);
				FsmComp->ChangeState(EBossState::Phase1_ChangePhase_2);
			}

			// 기존 동작 코드 
			/*APlayerBase* OverlapPlayer = OverlapCheckActor->GetCurrentOverlapPlayer();
			if (OverlapPlayer != nullptr)
			{
				if (true == OverlapPlayer->ActorHasTag("May"))
				{
					PlayerMay = Cast<AMay>(OverlapPlayer);
					bool MayInput = PlayerMay->GetIsInteract();
					if (true == MayInput)
					{
						UE_LOG(LogTemp, Warning, TEXT("Key Input True"));
						FsmComp->ChangeState(EBossState::Phase1_ChangePhase_2);
						return;
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("Key Input False"));
					}
				}
			}*/
			KeyInputTime -= DT;
		},

		[this]
		{
			bIsKeyInput = false;
			KeyInputTime = KeyInputMaxTime;

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
			
			// 코디메이 애니메이션 변경
			PlayerCody->CutScenceStart();
			// 메이 애니메이션 변경 추가 예정
		},

		[this](float DT)
		{
			// 애니메이션이 종료 되었을 때 
			if (false == SkeletalMeshComp->IsPlaying())
			{
				MulticastHideLaserBaseBone();
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


			// 유도로켓 발사 
			HomingRocketFireTime -= DT;
			if (0.0f >= HomingRocketFireTime)
			{
				FireHomingRocket();
				HomingRocketFireTime = HomingRocketCoolTime;
			}
		},

		[this]
		{
			HomingRocketFireTime = 3.0f;
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
			
			// 카메라 블렌딩 코드 
			// 임시로 0번 플레이어 카메라만 블렌드 처리 
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (nullptr == PlayerController)
			{
				UE_LOG(LogTemp, Warning, TEXT("PlayerController is nullptr"));
				return;
			}

			// 플레이어 컨트롤러 세팅 
			ViewTargetChangeController = PlayerController;
			// 플레이어 컨트롤러의 이전 카메라 액터 저장
			AActor* PrevCameraActor = ViewTargetChangeController->GetViewTarget();
			if (nullptr != PrevCameraActor)
			{
				PrevViewTarget = PrevCameraActor;
			}

			// 카메라 변경 후 재생비율 세팅
			ViewTargetChangeController->SetViewTargetWithBlend(Cast<AActor>(Phase2EndCameraRail), 0.2f);
			Phase2EndCameraRail->EnableCameraMove(0.25f);
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
			ViewTargetChangeController->SetViewTargetWithBlend(PrevViewTarget, 0.2f);
			ViewTargetChangeController = nullptr;
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
			
			// 임시로 0번 플레이어 카메라만 블렌드 처리 
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (nullptr == PlayerController)
			{
				UE_LOG(LogTemp, Warning, TEXT("PlayerController is nullptr"));
				return;
			}

			// 플레이어 컨트롤러 세팅 
			ViewTargetChangeController = PlayerController;
			// 플레이어 컨트롤러의 이전 카메라 액터 저장
			AActor* PrevCameraActor = ViewTargetChangeController->GetViewTarget();
			if (nullptr != PrevCameraActor)
			{
				PrevViewTarget = PrevCameraActor;
			}

			// 카메라 변경 후 재생비율 세팅
			ViewTargetChangeController->SetViewTargetWithBlend(Cast<AActor>(Phase3EndCameraRail_1), 0.2f);
			Phase3EndCameraRail_1->EnableCameraMove(0.55f);
		},

		[this](float DT)
		{
			// 애니메이션 재생 완료시 페이즈 종료 상태로 전환, 아무작업도 수행 X 
			// 여기서 되돌리지말고 그냥 End; 
			if (6.0f <= FsmComp->GetStateLiveTime())
			{
				ViewTargetChangeController->SetViewTargetWithBlend(PrevViewTarget, 0.2f);
				FsmComp->ChangeState(EBossState::AllPhaseEnd);
				return;
			}

			/*	if (true == Phase3EndCameraRail_1->IsMoveEnd())
				{
					ViewTargetChangeController->SetViewTargetWithBlend(PrevViewTarget, 0.2f);
					return;
				}*/
		},

		[this]
		{
			ViewTargetChangeController = nullptr;
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
			MulticastChangeAnimationFlyingSaucer(TEXT("/Game/Characters/EnemyFlyingSaucer/CutScenes/PlayRoom_SpaceStation_BossFight_Eject_FlyingSaucer_Anim"), 1, false);
			// 임시로 0번 플레이어 카메라만 블렌드 처리 
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			if (nullptr == PlayerController)
			{
				UE_LOG(LogTemp, Warning, TEXT("PlayerController is nullptr"));
				return;
			}
			
			// 플레이어 컨트롤러 세팅 
			ViewTargetChangeController = PlayerController;
			// 플레이어 컨트롤러의 이전 카메라 액터 저장
			AActor* PrevCameraActor = ViewTargetChangeController->GetViewTarget();
			if (nullptr != PrevCameraActor)
			{
				PrevViewTarget = PrevCameraActor;
			}

			// 카메라 변경 후 재생비율 세팅
			ViewTargetChangeController->SetViewTargetWithBlend(Cast<AActor>(Phase3EndCameraRail_1), 0.2f);
			Phase3EndCameraRail_1->EnableCameraMove(0.55f);
		},

		[this](float DT)
		{

			// 여기서 되돌리지말고 그냥 End; 
			if (6.0f <= FsmComp->GetStateLiveTime())
			{
				ViewTargetChangeController->SetViewTargetWithBlend(PrevViewTarget, 0.2f);
				return;
			}

		/*	if (true == Phase3EndCameraRail_1->IsMoveEnd())
			{
				ViewTargetChangeController->SetViewTargetWithBlend(PrevViewTarget, 0.2f);
				return;
			}*/
		},

		[this]
		{
			
		});
}



