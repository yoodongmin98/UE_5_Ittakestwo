// Fill out your copyright notice in the Description page of Project Settings.


#include "ParentCameraRail.h"
#include "FsmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"

AParentCameraRail::AParentCameraRail(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);

		CamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CamComp"));
		CamComp->SetupAttachment(RailCameraMount);

		FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));
	}
}

bool AParentCameraRail::ShouldTickIfViewportsOnly() const
{
	return false;
}

void AParentCameraRail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AParentCameraRail::Multicast_MoveRailCamera_Implementation(float RailRatio)
{
	CurrentPositionOnRail += RailRatio;
	CamComp->SetWorldLocation(GetRailSplineComponent()->GetLocationAtTime(CurrentPositionOnRail, ESplineCoordinateSpace::World));
}

void AParentCameraRail::Multicast_SetCameraView_Implementation()
{
	FConstPlayerControllerIterator PlayerControllerIter = GetWorld()->GetPlayerControllerIterator();

	UE_LOG(LogTemp, Display, TEXT("%d"), GetWorld()->GetNumPlayerControllers());

	if (HasAuthority() == true)
	{
		++++PlayerControllerIter;
		UE_LOG(LogTemp, Display, TEXT("server"));
	}
	else
	{
		++PlayerControllerIter;
		UE_LOG(LogTemp, Display, TEXT("client"));
	}

	PlayerControllerIter->Get()->SetViewTargetWithBlend(this, 0.1f);
}

bool AParentCameraRail::IsSupportedForNetworking() const
{
	return true;
}

void AParentCameraRail::BeginPlay()
{
	Super::BeginPlay();
}
