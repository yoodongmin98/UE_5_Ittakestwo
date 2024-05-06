// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundPoundEffect.h"
#include "NiagaraComponent.h"

// Sets default values
AGroundPoundEffect::AGroundPoundEffect()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComp);

	GroundPoundEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GroundPoundEffectComponent"));
	GroundPoundEffectComp->SetupAttachment(SceneComp);
}

// Called when the game starts or when spawned
void AGroundPoundEffect::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGroundPoundEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
