// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSaucer.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AFlyingSaucer::AFlyingSaucer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FlyingSaucerMesh"));
	SetRootComponent(SkeletalMeshComp);
}

// Called when the game starts or when spawned
void AFlyingSaucer::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AFlyingSaucer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

