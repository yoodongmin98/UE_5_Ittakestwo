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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:	
	bool bAttackNow = false;

	bool bPhaseEnd = false;	

	FVector DefaultPos = { 0.f,0.f,-300.f };

	FVector AttackPos = { 0.f,0.f,200.f };

	float MovingRatio = 0.f;

	float RotateTime = 15.f;
};
