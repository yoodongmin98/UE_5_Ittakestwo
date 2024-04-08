// Fill out your copyright notice in the Description page of Project Settings.


#include "UIRoseArm.h"

// Sets default values
AUIRoseArm::AUIRoseArm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//SkeletalMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserMesh"));
}

// Called when the game starts or when spawned
void AUIRoseArm::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUIRoseArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

