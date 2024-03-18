// Fill out your copyright notice in the Description page of Project Settings.


#include "ParentShutter.h"
#include "CoreShutter.h"

// Sets default values
AParentShutter::AParentShutter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AParentShutter::BeginPlay()
{
	Super::BeginPlay();

	ACoreShutter* test = GetWorld()->SpawnActor<ACoreShutter>();
	test->SetOwner(this);
}

// Called every frame
void AParentShutter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

