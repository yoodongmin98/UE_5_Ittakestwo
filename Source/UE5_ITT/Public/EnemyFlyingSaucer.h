// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyFlyingSaucer.generated.h"

UCLASS()
class UE5_ITT_API AEnemyFlyingSaucer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyFlyingSaucer();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void FireArcingProjectile();

	UPROPERTY(BlueprintReadWrite, Replicated)
	float ArcingProjectileFireTime = 3.0f;

	UPROPERTY(BlueprintReadWrite, Replicated)
	float ArcingProjectileMaxFireTime = 3.0f;

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastCreateEnergyChargeEffect();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastCreateGroundPoundEffect();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastSetFocusTarget();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastAttachMoonBaboonActorWithFloor();

	// ������ ���� ���� ���� ����
	UPROPERTY(BlueprintReadWrite, Replicated)
	FVector PrevLaserTargetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Replicated)
	FVector NextLaserTargetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Replicated)
	FVector CurrentLaserTargetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bPrevTargetLocationValid = false;

	UPROPERTY(BlueprintReadWrite, Replicated)
	float LaserLerpRatio = 0.0f;

	UPROPERTY(BlueprintReadWrite, Replicated)
	float LaserLerpScale = 7.5f;

	UPROPERTY(BlueprintReadWrite, Replicated)
	float LaserLerpRate = 25.0f;

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 LaserFireCount = 0;

	UPROPERTY(VisibleDefaultsOnly)
	int32 CurrentArcingProjectileTargetIndex = 0;

	UPROPERTY(VisibleDefaultsOnly)
	class APlayerBase* LaserTargetActor = nullptr;

	UPROPERTY(VisibleDefaultsOnly)
	class APlayerBase* PrevLaserTargetActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float CoreExplodeDamage = 11.0f;

	UPROPERTY(BlueprintReadWrite)
	bool bIsFsmStart = false;

	UFUNCTION(BlueprintCallable)
	void SetDamage(const float Damage)
	{
		CurrentHp -= Damage;
		if (CurrentHp <= 0.0f)
		{
			CurrentHp = 0.0f;
		}
	}

	UPROPERTY(BlueprintReadWrite, Replicated)
	int32 PatternDestroyCount = 0;

	UFUNCTION(BlueprintCallable)
	void AddPatternDestoryCount();
	
	UPROPERTY(VisibleDefaultsOnly)
	TArray<class AActor*> PlayerActors;

	// Get, Set
	UFUNCTION(BlueprintCallable)
	void SetCurrentHp(float HpValue) { CurrentHp = HpValue; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentHp() const { return CurrentHp; }

	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetLaserSpawnPointMesh() const { return LaserSpawnPointMesh; }

	UFUNCTION(BlueprintCallable)
	class AEnemyMoonBaboon* GetMoonBaboonActor() const { return EnemyMoonBaboon; }

	UFUNCTION(BlueprintCallable)
	AFloor* GetFloor() { return FloorObject; }
	
	UFUNCTION(BlueprintCallable)
	const bool IsCodyHoldingEnter() const { return bIsCodyHoldingEnter; }

	UPROPERTY(BlueprintReadWrite, Replicated)
	FVector PrevAnimBoneLocation = FVector::ZeroVector;

	UFUNCTION(BlueprintCallable)
	const int32 GetCurrentState();

	UFUNCTION(BlueprintCallable)
	ACody* GetPlayerCody() const { return PlayerCody; }

	UFUNCTION(BlueprintCallable)
	AMay* GetPlayerMay() const { return PlayerMay; }

	enum class EBossState
	{
		None,

		Intro,
		Phase1_LaserBeam_1,
		Phase1_LaserBeam_1_Destroy,
		Phase1_ArcingProjectile_1,
		Phase1_LaserBeam_2,
		Phase1_LaserBeam_2_Destroy,
		Phase1_ArcingProjectile_2,
		Phase1_LaserBeam_3,
		Phase1_BreakThePattern,

		CodyHolding_Enter,
		CodyHolding_Low,
		CodyHolding_ChangeOfAngle,
		CodyHolding_InputKey,
		CodyHolding_ChangeOfAngle_Reverse,
		Phase1_ChangePhase_2,

		Phase2_RotateSetting,
		Phase2_Rotating,
		Phase2_RocketHit,
		Phase2_BreakThePattern,
		Phase2_ChangePhase_Wait,
		Phase2_Fly,
		Phase2_MoveToCenter,

		Phase3_MoveFloor,
		Phase3_MoveToTarget,
		Phase3_GroundPounding,

		Phase3_Eject,

		FireHomingRocket,
		FireArcingProjectile,

		AllPhaseEnd,
		TestState,
		InteractionTestState,
	};

	UFUNCTION(BlueprintCallable)
	void EnableEject() 
	{ 
		bIsEject = true; 
	}

	void SetRocketHit()
	{
		bIsRocketHit = true;
	}

	UFUNCTION(BlueprintCallable)
	void SetupPlayerActorsCodyAndMay();

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bIsCutSceneEnd = false;

	UPROPERTY(BlueprintReadWrite, Replicated)
	bool bIsCutSceneStart = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsDebugChangePhase = false;

	// ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USoundManageComponent* SoundComp = nullptr;
	
	// �����
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class USoundManageComponent* BGMComp = nullptr;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	

	void SpawnCodyChaseHomingRocket();
	void SpawnMayChaseHomingRocket();

	void CodyChaseRocketSpawnCheck(float DeltaTime);
	void MayChaseRocketSpawnCheck(float DeltaTime);

	// �����ִϸ��̼� ����� ����ϴ� �ִϸ��̼� ���ҽ� ���� �ִϸ��̼� ������ or �ִϸ��̼� �������Ʈ 
	enum class EAnimationAssetType : uint8
	{
		None,
		Sequence,
		Blueprint,
	};

	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void PlayerCheckComponentOnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void PlayerCheckComponentOnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void CodyCheckComponentOnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CodyCheckComponentOnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void SetupHitEvent();
	void SetupOverlapEvent();
	
	// multicast �Լ� 
	UFUNCTION(NetMulticast, Reliable)
	void MulticastChangeAnimationFlyingSaucer(const FString& AnimPath, const uint8 AnimType, bool AnimLoop);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastChangeAnimationMoonBaboon(const FString& AnimPath, const uint8 AnimType, bool AnimLoop);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastCheckCodyKeyPressedAndChangeState(const bool bIsInput);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetActivateUIComponent(UInteractionUIComponent* UIComponent, bool ParentUIActivate, bool ChildUIActivate);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSwapMesh(bool bSkeletalMeshCompVisible, bool bStaticMeshCompVisible);


	int32 GetFloorCurrentState();
	void DrawDebugMesh();

	UPROPERTY(EditDefaultsOnly)
	float ServerDelayTime = 5.0f;

	void SetupComponent();
	void SetupFsmComponent();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastEnableCutSceneCameraBlend(class APhaseEndCameraRail* CameraRail, const float BlendTime, const float BlendRatio);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDisableCutSceneCameraBlend(AActor* PrevViewTargetActor, const float BlendTime);

	void ActiveCodyUI(bool bNewVisibility, bool bPropagateToChildren);
	void ActiveMayUI(bool bNewVisibility, bool bPropagateToChildren);
	void ActiveSplitScreen(bool bActive);

	// ���� ���ѽ� �÷��̾� �߰� Ű �Է� ���� ���� 
	UPROPERTY(EditDefaultsOnly)
	bool bIsKeyInput = false;
	float KeyInputTime = 0.0f;
	float KeyInputMaxTime = 1.25f;
	float KeyInputAdditionalTime = 0.75f;

	UPROPERTY(Replicated, EditAnywhere)
	float CurrentHp = 100.0f;

	UPROPERTY(Replicated, EditAnywhere)
	float MaxHp = 100.0f;

	UPROPERTY(Replicated, EditAnywhere)
	bool bIsRocketHit = false;

	// Ư���ð� ���� State ���� �� �ش� ���� ���
	UPROPERTY(EditDefaultsOnly)
	float StateCompleteTime = 0.0f;

	// tsubclass
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEnemyMoonBaboon> EnemyMoonBaboonClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AHomingRocket> HomingRocketClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AArcingProjectile> ArcingProjectileClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AFloor> FloorClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEnergyChargeEffect> EnergyChargeEffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AGroundPoundEffect> GroundPoundEffectClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ACoreExplosionEffect> CoreExplosionEffectClass = nullptr;


	// ���� ����ü ���� 
	UFUNCTION(BlueprintCallable)
	void ResetArcingProjectileFireCount() { ArcingProjectileFireCount = 0; }
	
	UPROPERTY(EditAnywhere)
	int32 ArcingProjectileFireCount = 0;


	UPROPERTY(Replicated, EditDefaultsOnly)
	class AEnemyMoonBaboon* EnemyMoonBaboon = nullptr;

	UPROPERTY(EditAnywhere)
	class AFloor* FloorObject = nullptr;
	
	UPROPERTY(Replicated, EditDefaultsOnly)
	class AEnergyChargeEffect* EnergyChargeEffect = nullptr;

	// Component
	UPROPERTY(Replicated, EditDefaultsOnly)
	class UFsmComponent* FsmComp = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class URotatingMovementComponent* RotatingComp = nullptr;

	UPROPERTY(Replicated, EditAnywhere)
	class USkeletalMeshComponent* SkeletalMeshComp = nullptr;

	UPROPERTY(Replicated, EditAnywhere)
	class UStaticMeshComponent* PlayerOverlapCheckComp = nullptr;

	UPROPERTY(Replicated, EditAnywhere)
	class UStaticMeshComponent* HatchOpenStaticMeshComp = nullptr;

	UPROPERTY(Replicated, EditAnywhere)
	class UStaticMeshComponent* CodyOverlapCheckComp = nullptr;

	UPROPERTY(EditDefaultsOnly)
	bool bIsCodyOverlap = false;

	UPROPERTY(EditDefaultsOnly)
	bool bIsMayOverlap = false;

	// ���� ���� ���� ����, ����׵�ο� Ȱ��ȭ�� ��ġ Ȯ�� ���� 
	UPROPERTY(Replicated, EditDefaultsOnly)
	class UStaticMeshComponent* LaserSpawnPointMesh = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class UStaticMeshComponent* HomingRocketSpawnPointMesh1 = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class UStaticMeshComponent* HomingRocketSpawnPointMesh2 = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class UStaticMeshComponent* ArcingProjectileSpawnPointMesh = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class UAnimInstance* AnimInstance = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class UAnimSequence* AnimSequence = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	bool bIsCodyHoldingEnter = false;

	// ������ ���� ���� ����
	void UpdateLerpRatioForLaserBeam(float DeltaTime);
	void SavePreviousTargetLocation();

	UFUNCTION(BlueprintCallable)
	void SetupLaserTargetActor();

	// �ڵ� Ȧ�� ���� ���۽� �ڵ� ��ġ �̵��� ������ ��
	// �̵� ���Ѽ� ������ų �ڵ� ��ġ
	FVector CodyLerpEndLocation = FVector(621.47f, -591.51f, 10366.55f);
	
	// ���� ��ġ������ 
	// 104 + 9990
	FVector MayCorrectLocation = FVector(491.0f, -308.0f, 10094.55f);

	FVector CodyUfoInsideLocation = FVector(395.0f, 113.0f, -4437.0f);
	FRotator CodyUfoInsideRotation = FRotator(0.f, -150.f, 0.f);

	// ���� ������ ������ float
	float CodyLerpRatio = 0.0f;
	// ���� �ϷḦ üũ�� bool 
	bool bIsCodyHoldingLerpEnd = false;
	
	// �ڵ� Ȧ���� ��ġ����
	void CorrectCodyLocationAndRotation();

	// ���� �ƽ� ���۽� ��ġ����
	void CorrectMayLocationAndRoation();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHideLaserBaseBoneAndSpawnDestroyEffect();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHideLaserBaseBone();

	UPROPERTY(Replicated)
	class ACody* PlayerCody = nullptr;

	UPROPERTY(Replicated)
	class AMay* PlayerMay = nullptr;

	bool bIsCorretLocation = false;
	float HomingRocket1FireTime = 4.0f;
	float HomingRocket2FireTime = 4.0f;
	float HomingRocketCoolTime = 4.0f;

	bool bIsCodyChaseRocketDestroy = false;
	bool bIsMayChaseRocketDestroy = false;

	// ���� ���� 
	class AHomingRocket* HomingRocketActor_1 = nullptr;
	class AHomingRocket* HomingRocketActor_2 = nullptr;

	// 3������ �����̵����� 
	float MoveToCenterLerpRatio = 0.0f;
	FVector MoveStartLocation = FVector::ZeroVector;

	// GroundPound �̵����� 
	float MoveToTargetLerpRatio = 0.0f;
	FVector GroundPoundTargetLocation = FVector::ZeroVector;
	
	float GroundPoundEffectCreateTime = 1.61f;
	bool bIsSetGroundPoundEffect = false;

	// ���� Ż�� ����
	UPROPERTY(Replicated)
	bool bIsEject = false;

	// ī�޶� ���� ���� 
	class APlayerController* ViewTargetChangeController = nullptr;
	class AActor* PrevViewTarget = nullptr;

	UPROPERTY(EditAnywhere)
	class APhaseEndCameraRail* PowerCoreDestroyCameraRail = nullptr;

	UPROPERTY(EditAnywhere)
	class APhaseEndCameraRail* BossFallCameraRail = nullptr;

	UPROPERTY(EditAnywhere)
	class APhaseEndCameraRail* LaserDestroyCameraRail = nullptr;

	UPROPERTY(EditAnywhere)
	class APhaseEndCameraRail* BossEjectCameraRail = nullptr;

	float GroundPoundDamage = 6.0f;

	// ��� ���� ����� ���� �� 
	UPROPERTY(EditAnywhere)
	bool bIsAllPhaseEnd = false;

	UPROPERTY(EditAnywhere)
	bool bIsPhase1End = false;

	
};
