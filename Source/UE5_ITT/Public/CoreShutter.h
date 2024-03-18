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

private:
	bool bOpen = false;

	float OpenTime = 15.f;

	bool bAttackNow = false;

	float MovingRatio = 0.f;

	FVector PivotPos = { 1741.f,-4205.f, 34660.f };
	FVector DefaultPos = FVector::Zero();
	FVector OpenPos = {-170.f,170.f,0.f};

	float RotateSize = 90.f;

	UPROPERTY(EditAnywhere);
	class UStaticMeshComponent* MeshComp = nullptr;
};
