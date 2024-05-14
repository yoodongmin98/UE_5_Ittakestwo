// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserExplosionEffect.h"
#include "NiagaraComponent.h"

// Sets default values
ALaserExplosionEffect::ALaserExplosionEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);

		SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
		SetRootComponent(SceneComp);

		ExplosionEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExplosionEffectComponent"));
		ExplosionEffectComp->SetupAttachment(SceneComp);
	}
}

// Called when the game starts or when spawned
void ALaserExplosionEffect::BeginPlay()
{
	Super::BeginPlay();
	
	if (true == HasAuthority())
	{
		SetupDestroyTimerEvent();
	}
}

void ALaserExplosionEffect::EffectDestroy()
{
	Destroy();
}

void ALaserExplosionEffect::SetupDestroyTimerEvent()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ALaserExplosionEffect::EffectDestroy, DestroyDelayTime, false);
}

// Called every frame
void ALaserExplosionEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

