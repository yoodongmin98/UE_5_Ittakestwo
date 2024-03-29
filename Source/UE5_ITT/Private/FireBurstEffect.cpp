// Fill out your copyright notice in the Description page of Project Settings.


#include "FireBurstEffect.h"

// Sets default values
AFireBurstEffect::AFireBurstEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FireBurstMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FireBurstEffectMesh"));
	SetRootComponent(FireBurstMeshComp);
}

// Called when the game starts or when spawned
void AFireBurstEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFireBurstEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector ActorScale = GetActorScale();
	// 현재 최대 스케일보다 작다면 더크게 
	if (ActorScale.X <= MaxScale)
	{
		FVector NewScale = ActorScale + FVector(1.0f, 1.0f, 1.0f) * DeltaTime;
		GetRootComponent()->SetWorldScale3D(NewScale);
	}

	else
	{
		Destroy();
	}
	

	



	


}

