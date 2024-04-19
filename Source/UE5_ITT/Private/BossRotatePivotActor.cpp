// Fill out your copyright notice in the Description page of Project Settings.


#include "BossRotatePivotActor.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABossRotatePivotActor::ABossRotatePivotActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);

		SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
		SetRootComponent(SceneComp);

		StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
		StaticMeshComp->SetupAttachment(SceneComp);
	}
}

// Called when the game starts or when spawned
void ABossRotatePivotActor::BeginPlay()
{
	Super::BeginPlay();
	
	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
	}
}

// Called every frame
void ABossRotatePivotActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		FVector LaserSpawnPointMeshLocation = GetActorLocation();

		float SphereRadius = 100.0f;
		int32 Segments = 12;
		float LifeTime = 0.1f;
		float Thickness = 2.0f;

		DrawDebugSphere(
			GetWorld(),
			LaserSpawnPointMeshLocation,
			SphereRadius,
			Segments,
			FColor::Red,
			false,
			LifeTime,
			0,
			Thickness
		);
	}

}

