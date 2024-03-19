// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreShutter.h"
#include <Components/StaticMeshComponent.h>

// Sets default values
ACoreShutter::ACoreShutter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}
	
// Called when the game starts or when spawned
void ACoreShutter::BeginPlay()
{
	Super::BeginPlay();

	bOpen = true;
}

// Called every frame
void ACoreShutter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (true == bAttackNow)
	{		
		OpenTime -= DeltaTime;
	
		if (OpenTime <= 0.f)
		{
			OpenTime = 3.f;
			bAttackNow = false;
			bOpen = false;
		}
		return;
	}
	if (true == bOpen)
	{
		MovingRatio += DeltaTime*0.5f;

		if (MovingRatio >= 1.f)
		{
			bAttackNow = true;
			MovingRatio = 1.f;
		}

		AddActorLocalRotation({ 0.f,RotateSize * DeltaTime * 0.5f,0.f });
		SetActorRelativeLocation(PivotPos+FMath::Lerp(DefaultPos, OpenPos, MovingRatio));
	
		
	}
	else
	{
		MovingRatio -= DeltaTime * 0.5f;
		if (MovingRatio <= 0.f)
		{
			bOpen = true;
			MovingRatio = 0.f;
		}
	
		AddActorLocalRotation({ 0.f,-RotateSize * DeltaTime * 0.5f,0.f });
		SetActorRelativeLocation(PivotPos+FMath::Lerp(DefaultPos, OpenPos, MovingRatio));
	
		
	}
}

