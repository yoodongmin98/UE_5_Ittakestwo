// Fill out your copyright notice in the Description page of Project Settings.


#include "Laser.h"

// Sets default values
ALaser::ALaser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALaser::BeginPlay()
{
	Super::BeginPlay();
	
	bPhaseEnd = true;
}

// Called every frame
void ALaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (true == bAttackNow)
	{
		RotateTime -= DeltaTime;

		AddActorLocalRotation({0.f,360.f * DeltaTime,0.f});

		if (RotateTime<=0.f)
		{
			RotateTime = 15.f; 
			bAttackNow = false;
			bPhaseEnd = false;
		}
		return;
	}
	if (true == bPhaseEnd)
	{
		MovingRatio += DeltaTime;

		if (MovingRatio >= 1.f)
		{
			bAttackNow = true;
			MovingRatio = 1.f;
		}

		SetActorRelativeLocation(FMath::Lerp(DefaultPos, AttackPos, MovingRatio));		
	}
	else
	{
		MovingRatio -= DeltaTime;

		if (MovingRatio <= 0.f)
		{
			bPhaseEnd = true;
			MovingRatio = 0.f;
		}

		SetActorRelativeLocation(FMath::Lerp(DefaultPos, AttackPos, MovingRatio));
	}
}

