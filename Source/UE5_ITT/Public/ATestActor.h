// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ATestActor.generated.h"

UCLASS()
class UE5_ITT_API AATestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AATestActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//UPROPERTY(EditAnywhere)
	FVector StartPosFloat = FVector(0.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere)
	float MovingSpeed = 100.0f;

	UPROPERTY(EditAnywhere)
	FVector DestPosFloat = FVector(1.0f, 1.0f, 1.0f);

	FVector Test_Dir = FVector(0.0f, 0.0f, 0.0f);

	float MovingUnit = 0.0f;
};
