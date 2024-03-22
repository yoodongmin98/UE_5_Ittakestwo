// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floor.generated.h"

UCLASS()
class UE5_ITT_API AFloor : public AActor
{
	GENERATED_BODY()

public:
	enum class EPhase
	{
		Phase1_1,
		Phase1_2,
		Phase1_3,
		Phase2,
		Phase3,
		None,
	};

public:	
	// Sets default values for this actor's propertiesS
	AFloor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetPhase(EPhase Phase)
	{
		CurPhase = Phase;
	};


private:
	EPhase CurPhase = EPhase::None;
};
