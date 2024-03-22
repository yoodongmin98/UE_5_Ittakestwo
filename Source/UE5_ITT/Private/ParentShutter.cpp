// Fill out your copyright notice in the Description page of Project Settings.


#include "ParentShutter.h"
#include "CoreShutter.h"

// Sets default values
AParentShutter::AParentShutter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AParentShutter::BeginPlay()
{
	Super::BeginPlay();

	ArrayCoreShutter.SetNum(4);

	for (int i = 0; i < 4; i++)
	{
		ArrayCoreShutter[i] = GetWorld()->SpawnActor<ACoreShutter>(CoreShutterClass);
		ArrayCoreShutter[i]->AttachToActor(this,FAttachmentTransformRules::KeepRelativeTransform);
		ArrayCoreShutter[i]->SetActorRelativeRotation({ 0.f, 90.f * i,0.f });
		ArrayCoreShutter[i]->SetPivotPos(GetActorLocation());
	}
	ArrayCoreShutter[0]->SetOpenPos({ -MovingSize,MovingSize ,0.f });
	ArrayCoreShutter[1]->SetOpenPos({ -MovingSize,-MovingSize ,0.f });
	ArrayCoreShutter[2]->SetOpenPos({ MovingSize,-MovingSize ,0.f });
	ArrayCoreShutter[3]->SetOpenPos({ MovingSize,MovingSize ,0.f });	
}

// Called every frame
void AParentShutter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AParentShutter::SetShutterOpen(bool bValue)
{
	for (size_t i = 0; i < ArrayCoreShutter.Num(); i++)
	{
		ArrayCoreShutter[i]->OpenShutter(bValue);
	}
}

