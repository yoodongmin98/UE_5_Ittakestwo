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
	SetActorLocation(DefaultPos);
}

// Called every frame
void ALaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bAttackNow = false)
	{
		Ratio += 1.f * DeltaTime;
		SetActorLocation(FMath::Lerp(DefaultPos, DefaultPos, Ratio));
		if (Ratio>=1.f)
		{
			bAttackNow = true;
		}
	}
}

