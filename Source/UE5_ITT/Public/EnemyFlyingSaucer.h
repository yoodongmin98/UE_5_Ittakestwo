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

	// 각종 발사
	UFUNCTION(BlueprintCallable, Category = "Battle")
	void FireLaserBeam();
	UFUNCTION(BlueprintCallable, Category = "Battle")
	void FireHomingRocket();
	UFUNCTION(BlueprintCallable, Category = "Battle")
	void FireArcingProjectile();

	UFUNCTION(BlueprintCallable)
	float GetCurrentHp() const
	{
		return CurrentHp;
	}

	UFUNCTION(BlueprintCallable)
	void SetCurrentHp(float HpValue)
	{
		CurrentHp = HpValue;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// 컴포넌트 초기화
	void SetupComponent();

	// 디버그 관련 함수 
	void DrawDebugMesh();

	UPROPERTY(EditAnywhere)
	float CurrentHp = 100.0f;
	
	//tsubclass 
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AEnemyMoonBaboon> EnemyMoonBaboonClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AHomingRocket> HomingRocketClass = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AArcingProjectile> ArcingProjectileClass = nullptr;

	// 원숭이 
	UPROPERTY(EditDefaultsOnly)
	class AEnemyMoonBaboon* EnemyMoonBaboon = nullptr;

	// 들고 있어야 되나 싶음
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* LaserSpawnPointMesh = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* HomingRocketSpawnPointMesh1 = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* HomingRocketSpawnPointMesh2 = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* ArcingProjectileSpawnPointMesh = nullptr;
};
