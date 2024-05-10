// Fill out your copyright notice in the Description page of Project Settings.


#include "BurstEffect.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ABurstEffect::ABurstEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetupComponent();
}

// Called when the game starts or when spawned
void ABurstEffect::BeginPlay()
{
	Super::BeginPlay();
	
	if (true == HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

void ABurstEffect::SetupComponent()
{
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BurstEffectMesh"));
	StaticMeshComp->SetupAttachment(SceneComp);
}

void ABurstEffect::TickIncreaseScale(float DeltaTime)
{
	FVector NewScale = GetActorScale3D() + FVector::OneVector * 3.0f * DeltaTime;
	GetRootComponent()->SetWorldScale3D(NewScale);
	if (NewScale.X >= MaxScale)
	{
		SetupDestroyTimerEvent();
		bDestroyValue = true;
	}
}

void ABurstEffect::SetupDestroyTimerEvent()
{
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABurstEffect::EffectDestroy, 1.0f, false);
}

// Called every frame
void ABurstEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		if (true == bDestroyValue)
		{
			return;
		}
		TickIncreaseScale(DeltaTime);
	}
}

void ABurstEffect::EffectDestroy()
{
	Destroy();
}
