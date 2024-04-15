// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreExplosionEffect.h"
#include "NiagaraComponent.h"

// Sets default values
ACoreExplosionEffect::ACoreExplosionEffect()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComp);

	ExplosionEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CoreExplosionEffectComponent"));
	ExplosionEffectComp->SetupAttachment(SceneComp);
}

// Called when the game starts or when spawned
void ACoreExplosionEffect::BeginPlay()
{
	Super::BeginPlay();

	SetupDestroyTimerEvent();
}

void ACoreExplosionEffect::EffectDestroy()
{
	Destroy();
}

void ACoreExplosionEffect::SetupDestroyTimerEvent()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ACoreExplosionEffect::EffectDestroy, 2.9f, false);
}

// Called every frame
void ACoreExplosionEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

