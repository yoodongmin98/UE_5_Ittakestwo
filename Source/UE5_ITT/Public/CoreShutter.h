// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoreShutter.generated.h"

UCLASS()
class UE5_ITT_API ACoreShutter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACoreShutter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetOpenPos(FVector Open)
	{
		OpenPos = Open;
	}

	void SetPivotPos(FVector Pivot)
	{
		PivotPos = Pivot;
	}

	void OpenShutter();
	

	void SetDone();

	enum class Fsm
	{
		Close,
		Opening,
		Open,
		Done,
	};


private:
	void SetupFsm();

private:
	UPROPERTY(EditAnywhere);
	float OpenTime = 3.f;

	bool bDone = false;

	float MovingRatio = 0.f;

	FVector PivotPos = FVector::Zero();
	FVector DefaultPos = FVector::Zero();
	FVector OpenPos = FVector::Zero();

	float RotateSize = 90.f;

	UPROPERTY(EditAnywhere);
	class UStaticMeshComponent* MeshComp = nullptr;
	class UFsmComponent* FsmComp = nullptr;
};
