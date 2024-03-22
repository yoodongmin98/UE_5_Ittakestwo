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
	void PhaseExcute(float DeltaTime);

private:
	EPhase CurPhase = EPhase::None;

	float MoveTime = 20.f;
	float MoveSize = 4995.f;

	UPROPERTY(EditAnywhere,Category = "Pointer")
	class AParentShutter* ParentShutter0 = nullptr;
	UPROPERTY(EditAnywhere, Category = "Pointer")
	class AParentShutter* ParentShutter1 = nullptr;
	UPROPERTY(EditAnywhere, Category = "Pointer")
	class AParentShutter* ParentShutter2 = nullptr;

	UPROPERTY(EditAnywhere, Category = "Pointer")
	class APillar* Pillar0 = nullptr;
	UPROPERTY(EditAnywhere, Category = "Pointer")
	class APillar* Pillar1 = nullptr;
	UPROPERTY(EditAnywhere, Category = "Pointer")
	class APillar* Pillar2 = nullptr;


	UPROPERTY(EditAnywhere, Category = "Pointer")
	class ALaser* MainLaser = nullptr;
};
