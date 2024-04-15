// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MediumBox.generated.h"

UCLASS()
class UE5_ITT_API AMediumBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMediumBox();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	enum class Fsm
	{
		Wait,
		Shake,
		Rotate,
	};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MediumBoxMesh = nullptr;

	class UFsmComponent* FsmComp = nullptr;

	FRotator RotateSize = {  0.f,0.f ,90.f,};
	FRotator CurRotation = FRotator::ZeroRotator;
	FRotator FinalRotation = FRotator::ZeroRotator;
	float RotateRatio = 0.f;

	float ShakeRange= 30.f;

	FRandomStream Random;


	void SetupFsm();

};
