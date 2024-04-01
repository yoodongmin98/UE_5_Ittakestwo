// Fill out your copyright notice in the Description page of Project Settings.


#include "BurstEffect.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ABurstEffect::ABurstEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BurstEffectMesh"));
	StaticMeshComp->SetupAttachment(SceneComp);
}

// Called when the game starts or when spawned
void ABurstEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABurstEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewScale = GetActorScale3D() + FVector(1.0f, 1.0f, 1.0f) * DeltaTime * 3.0f;
	GetRootComponent()->SetWorldScale3D(NewScale);

	if (NewScale.X >= MaxScale)
	{
		Destroy();
	}
}

