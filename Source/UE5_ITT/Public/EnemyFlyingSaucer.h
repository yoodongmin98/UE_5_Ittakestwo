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

	UFUNCTION(BlueprintCallable)
	void FireLaserBeam();
	UFUNCTION(BlueprintCallable)
	void FireHomingRocket();
	UFUNCTION(BlueprintCallable)
	void FireArcingProjectile();

	UFUNCTION(BlueprintCallable)
	float GetCurrentHp() const { return CurrentHp; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentHp(float HpValue) { CurrentHp = HpValue; }
	
	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetLaserSpawnPointMesh() const { return LaserSpawnPointMesh; }

	UFUNCTION(BlueprintCallable)
	AFloor* GetFloor() { return FloorObject; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION(BlueprintCallable)

	void SetupComponent();
	void DrawDebugMesh();

	UPROPERTY(EditDefaultsOnly)
	bool bIsStartMotion = true;

	// ÃÊ¹Ý ¿ø¼þÀÌ °øÁß¿¡ ºØ¶ß´Â °Å 
	void StartMotionUpdate(float DeltaTime);
	FVector StartMotionTargetLocation = FVector(0, 0, 650);

	UPROPERTY(EditAnywhere)
	float CurrentHp = 100.0f;
	
	//tsubclass 
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEnemyMoonBaboon> EnemyMoonBaboonClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AHomingRocket> HomingRocketClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AArcingProjectile> ArcingProjectileClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AFloor> FloorClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class AEnemyMoonBaboon* EnemyMoonBaboon = nullptr;

	UPROPERTY(EditAnywhere)
	class AFloor* FloorObject = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* LaserSpawnPointMesh = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* HomingRocketSpawnPointMesh1 = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* HomingRocketSpawnPointMesh2 = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* ArcingProjectileSpawnPointMesh = nullptr;
};
