// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArcingProjectile.generated.h"

UCLASS()
class UE5_ITT_API AArcingProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AArcingProjectile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetupProjectileMovementComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// 오버랩이벤트 테스트
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* SceneComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	class UStaticMeshComponent* ProjectileMeshComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovementComp = nullptr;

	UPROPERTY(EditAnywhere, Category = "Movement")
	FVector TargetLocation = FVector::ZeroVector;

	// 파티클
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* TrailEffectComp = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABurstEffect> BurstEffectClass;
};
