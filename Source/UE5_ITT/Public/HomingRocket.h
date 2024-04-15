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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	enum class ERocketState
	{
		PlayerChase,
		PlayerEquipWait,
		PlayerEquip,

		Destroy,
	};

	void SetupOverlapEvent();

	void SetupFsmComponent();

	void InActive()
	{
		bIsActive = false;
	}
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void DestroyRocket();


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
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* FireEffectComp = nullptr;
	
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
	

};
