// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosionEffect.h"
#include "NiagaraComponent.h"

// Sets default values
AExplosionEffect::AExplosionEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);
		SetupComponent();
	}
}

// Called when the game starts or when spawned
void AExplosionEffect::BeginPlay()
{
	Super::BeginPlay();
	if (true == HasAuthority())
	{
		SetupDestroyTimerEvent();
	}
}

// Called every frame
void AExplosionEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AExplosionEffect::EffectDestroy()
{
	Destroy();
}

void AExplosionEffect::SetupDestroyTimerEvent()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AExplosionEffect::EffectDestroy, DestroyDelayTime, false);
}

void AExplosionEffect::SetupComponent()
{
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComp);

	ExplosionEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireEffectComponent"));
	ExplosionEffectComp->SetupAttachment(SceneComp);
}

