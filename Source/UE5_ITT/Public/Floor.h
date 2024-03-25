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
	// Sets default values for this actor's propertiesS
	AFloor();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	enum class Fsm
	{
		Phase1_1,
		Phase1_2,
		Phase1_2Wait,
		Phase1_3,
		Phase1_3Wait,
		Phase2,
		Phase3,
	};
	void SetPhase(Fsm Phase);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	float MoveTime = 20.f;
	float MoveSize = 4995.f;
	float MoveRatio = 0.f;
	float PillarSecond = 0.f;

	FVector CurPos = FVector::Zero();
	FVector NextPos = FVector::Zero();

	//UPROPERTY(EditAnywhere,Category = "Pointer")
	//class AParentShutter* ParentShutter0 = nullptr;
	//UPROPERTY(EditAnywhere, Category = "Pointer")
	//class AParentShutter* ParentShutter1 = nullptr;
	//UPROPERTY(EditAnywhere, Category = "Pointer")
	//class AParentShutter* ParentShutter2 = nullptr;

	UPROPERTY(EditAnywhere, Category = "Pointer")
	class APillar* Pillar0 = nullptr;
	UPROPERTY(EditAnywhere, Category = "Pointer")
	class APillar* Pillar1 = nullptr;
	UPROPERTY(EditAnywhere, Category = "Pointer")
	class APillar* Pillar2 = nullptr;


	UPROPERTY(EditAnywhere, Category = "Pointer")
	class ALaser* MainLaser = nullptr;

	class UFsmComponent* FsmComp = nullptr;

	void SetupFsm();
};
