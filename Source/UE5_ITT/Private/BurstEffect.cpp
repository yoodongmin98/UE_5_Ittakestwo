// Fill out your copyright notice in the Description page of Project Settings.


#include "BurstEffect.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

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

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

void ABurstEffect::EffectDestroy()
{
	Destroy();
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

		FVector NewScale = GetActorScale3D() + FVector(1.0f, 1.0f, 1.0f) * DeltaTime * 3.0f;
		GetRootComponent()->SetWorldScale3D(NewScale);

		if (NewScale.X >= MaxScale)
		{
			SetupDestroyTimerEvent();
			bDestroyValue = true;
		}
	}
}

