// Fill out your copyright notice in the Description page of Project Settings.


#include "EnergyCore.h"

// Sets default values
AEnergyCore::AEnergyCore()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GlassMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GlassMesh"));

	RootComponent = GlassMesh;

	CoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMesh"));
	CoreMesh->SetupAttachment(GlassMesh);
}

// Called when the game starts or when spawned
void AEnergyCore::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnergyCore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

