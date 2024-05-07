// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HomingRocket.generated.h"

UCLASS()
class UE5_ITT_API AHomingRocket : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHomingRocket();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void SetupTarget(AActor* Target) 
	{
		if (nullptr != Target)
		{
			TargetActor = Target; 
		}
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	const int32 GetCurrentState() const;

	UFUNCTION()
	void DestroyRocket();

	enum class ERocketState
	{
		PlayerChase,	     // 플레이어 추적 상태
		PlayerEquipWait,     // 라이프타임 소진시 추락, 대기상태 
		PlayerEquip,		 // 플레이어 상호작용 키 입력시 플레이어 장착

		PlayerEquipCorrect,   // 플레이어 위치보정

		DestroyWait,		 // 완전히 Destroy 하기 전 대기상태, 보스에서 미사일이 제거되었는지 파악하기 위한 상태 
		Destroy,			 // 완전한 Destroy, 상태 진입시 액터 Destory 

	};
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnDestroyEffect();

	void SetupOverlapEvent();

	void SetupFsmComponent();

	void InActive()
	{
		bIsActive = false;
	}
	
	void TickPlayerChaseLogic(float DeltaTime);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	


	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	class USceneComponent* SceneComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	class UStaticMeshComponent* RocketMeshComp = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RocketLifeTime = 30.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RocketMoveSpeed = 750.0f;

	UPROPERTY(VisibleAnywhere)
	class ACody* PlayerCodyRef = nullptr;

	UPROPERTY(EditAnywhere)
	bool bIsActive = true;

	// 파티클
	UPROPERTY(Replicated)
	class UNiagaraComponent* FireEffectComp = nullptr;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ActivateFireEffectComponent();
	
	// 파티클
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AExplosionEffect> ExplosionEffectClass;

	UPROPERTY(EditDefaultsOnly)
	class AActor* TargetActor = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UFsmComponent* RocketFsmComponent = nullptr;

	UPROPERTY(EditDefaultsOnly)
	bool bIsPlayerEquip = false;

	UPROPERTY(EditDefaultsOnly)
	bool bIsPlayerOverlap = false;

	UPROPERTY(EditDefaultsOnly)
	class APlayerBase* OverlapActor = nullptr;

	UPROPERTY(EditAnywhere)
	float MaxFloorDistance = 425.0f;

	bool IsMaxFloorDistance();
	
	UFUNCTION()
	void SetRocektLifeTime(const float LifeTime) { RocketLifeTime = LifeTime; }
	bool bIsSetLifeTime = false;

	UPROPERTY(EditAnywhere)
	bool bIsBossOverlap = false;

	// 파티클
	UPROPERTY(Replicated)
	class AEnemyFlyingSaucer* BossActor = nullptr;
		
	UPROPERTY(Replicated)
	float RocketDamageToBoss = 7.5f;

	UPROPERTY(Replicated)
	float PlayerEquipLerpRatio = 0.0f;

	UPROPERTY(Replicated)
	FRotator PlayerEquipLerpStartRotation = FRotator::ZeroRotator;

	UPROPERTY(Replicated)
	FRotator PlayerEquipLerpEndRotation = FRotator::ZeroRotator;

	UPROPERTY(Replicated)
	float PlayerEquipMaxLiveTime = 15.0f;

	void EnablePlayerFlying();
	void DisablePlayerFlying();
};
