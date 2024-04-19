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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// 2페이즈 원형 움직임을 위한 pivot actor 세팅, 부착, 해제 및 회전 관련
	UFUNCTION(BlueprintCallable)
	void SetupRotateCenterPivotActor();

	UFUNCTION(BlueprintCallable)
	void DetachRotateCenterPivotActor();

	UFUNCTION(BlueprintCallable)
	ABossRotatePivotActor* GetRotateCenterPivotActor() { return RotateCenterPivotActor; }

	UFUNCTION(BlueprintCallable)
	void RotationCenterPivotActor(float DeltaTime);

	// 보스 패턴 중 투사체 발사 관련 함수 
	UFUNCTION(BlueprintCallable)
	void FireHomingRocket();

	UFUNCTION(BlueprintCallable)
	void DisCountHomingRocketFireCount()
	{
		if (0 < HomingRocketFireCount)
		{
			--HomingRocketFireCount;
		}
	}

	UFUNCTION(BlueprintCallable)
	void FireArcingProjectile();

	UFUNCTION(BlueprintCallable)
	AEnergyChargeEffect* CreateEnergyChargeEffect();

	// 체력(임시) 
	UFUNCTION(BlueprintCallable)
	void SetCurrentHp(float HpValue) { CurrentHp = HpValue; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentHp() const { return CurrentHp; }

	// Get, Set 
	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetLaserSpawnPointMesh() const { return LaserSpawnPointMesh; }

	UFUNCTION(BlueprintCallable)
	class AEnemyMoonBaboon* GetMoonBaboonActor() const { return EnemyMoonBaboon; }

	UFUNCTION(BlueprintCallable)
	AFloor* GetFloor() { return FloorObject; }

	// test, 보스 테스트 끝나면 삭제 
	void BossHitTestFireRocket();

	// 추후 사용가능성 
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	enum class EBossState
	{
		None,

		Intro,
		Phase1_Progress,
		Phase1_BreakThePattern,
		
		

		// 얘 할 차례
		CodyHoldingStart,




		Phase1End,

		CodyHoldingProgress_NotKeyMashing,

		CodyHoldingProgress_ChangeOfAngle,

		CodyHoldingProgress_KeyMashing,

		CodyHoldingProgress_KeyMashingEnd,

		CodyHoldingProgress,
		CodyHoldingEnd,

		Phase2Start,
		Phase2Progress,
		Phase2End,

		Phase3Start,
		Phase3Progress,
		Phase3End,

		FireHomingRocket,
		FireArcingProjectile,

	};

	enum class EAnimationAssetType : uint8
	{
		None,
		Sequence,
		Blueprint,
	};
	// 디버그 
	void DrawDebugMesh();

	UPROPERTY(EditDefaultsOnly)
	float ServerDelayTime = 4.0f;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChangeAnimationFlyingSaucer(const FString& AnimPath, const uint8 AnimType, bool AnimLoop);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ChangeAnimationMoonBaboon(const FString& AnimPath, const uint8 AnimType, bool AnimLoop);

	void SetupComponent();

	UFUNCTION(BlueprintCallable)
	void SetupFsmComponent();
	
	// 패턴 파훼시 플레이어 추가 키 입력 관련 변수 
	UPROPERTY(EditDefaultsOnly)
	bool bIsKeyMashing = false;
	float KeyMashingTime = 0.0f;
	float KeyMashingMaxTime = 1.25f;
	
	UPROPERTY(EditDefaultsOnly)
	float CurrentHp = 67.0f;

	// 특정시간 내에 State 변경 시 해당 변수 사용
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

	// 보스 투사체 관련 
	UFUNCTION(BlueprintCallable)
	void ResetArcingProjectileFireCount() { ArcingProjectileFireCount = 0; }
	
	UPROPERTY(EditAnywhere)
	int32 ArcingProjectileFireCount = 0;

	UPROPERTY(EditAnywhere)
	int32 HomingRocketFireCount = 0;

	// 보스 2페이즈 회전시 기준점이 되는 액터 
	UPROPERTY(EditDefaultsOnly)
	class ABossRotatePivotActor* RotateCenterPivotActor = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ABossRotatePivotActor> RotateCenterPivotActorClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float RotateCenterPivotActorMoveSpeed = 9.0f;

	// 보스가 생성하는 액터 
	UPROPERTY(Replicated, EditDefaultsOnly)
	class AEnemyMoonBaboon* EnemyMoonBaboon = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class AFloor* FloorObject = nullptr;
	
	UPROPERTY(EditAnywhere)
	class AEnergyChargeEffect* EnergyChargeEffect = nullptr;

	// Component
	UPROPERTY(Replicated, EditDefaultsOnly)
	class UFsmComponent* FsmComp = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class UInteractionUIComponent* UIComp = nullptr;

	UPROPERTY(Replicated, EditDefaultsOnly)
	class USkeletalMeshComponent* SkeletalMeshComp = nullptr;


	UFUNCTION(BlueprintCallable)
	void ActivateUIComponent();

	// 오버랩 체크 관련 
	UFUNCTION(BlueprintCallable)
	void SpawnOverlapCheckActor();

	UPROPERTY(EditDefaultsOnly)
	class AOverlapCheckActor* OverlapCheckActor = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AOverlapCheckActor> OverlapCheckActorClass = nullptr;

	// 보스 공격 생성 지점 컴포넌트 
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
};
