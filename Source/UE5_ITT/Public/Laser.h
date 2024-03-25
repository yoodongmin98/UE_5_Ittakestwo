// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Laser.generated.h"

UCLASS()
class ALaser : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaser();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	enum class Fsm
	{
		Wait,
		MoveUp,
		LaserOn,
		Attack,
		LaserOff,
		MoveDown,
	};

	void SetAttack(bool bValue)
	{
		bAttackStart = bValue;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	bool bAttackStart = false;

	bool bPhaseEnd = false;	

	FVector DefaultPos = FVector::Zero();
	FVector AttackPos = FVector::Zero();
	float AttackMoveSize = 500.f;

	float MovingRatio = 0.f;

	float RotateTime = 15.f;

	UPROPERTY(EditAnywhere, Category = "Pointer")
	class UStaticMeshComponent* LaserMesh = nullptr;
	class UFsmComponent* FsmComp = nullptr;

	void SetupFsm();
};
