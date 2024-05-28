// Fill out your copyright notice in the Description page of Project Settings.


#include "UfoCutSceneCameraRail.h"
#include "FsmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"
#include "Net/UnrealNetwork.h"

AUfoCutSceneCameraRail::AUfoCutSceneCameraRail(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);

		CamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CamComp"));
		CamComp->SetupAttachment(RailCameraMount);

		SetupFsm();
	}
}

bool AUfoCutSceneCameraRail::ShouldTickIfViewportsOnly() const
{
	return false;
}


void AUfoCutSceneCameraRail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUfoCutSceneCameraRail::Multicast_MoveRailCamera_Implementation(float RailRatio)
{
	CurrentPositionOnRail += RailRatio;
	CamComp->SetWorldLocation(GetRailSplineComponent()->GetLocationAtTime(CurrentPositionOnRail, ESplineCoordinateSpace::World));
}

void AUfoCutSceneCameraRail::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::Wait);
	}
}

void AUfoCutSceneCameraRail::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));
	FsmComp->CreateState(Fsm::Wait,
		[this]
		{

		},

		[this](float DT)
		{				
			if (bTest == false)
			{
				return;
			}
			FsmComp->ChangeState(Fsm::Move);
		},

		[this]
		{
		});
	FsmComp->CreateState(Fsm::Move,
		[this]
		{
			CurrentPositionOnRail = 0.f;
		},

		[this](float DT)
		{
			Multicast_MoveRailCamera(DT * CamMoveRatio);
			UE_LOG(LogTemp, Display, TEXT("======================"));
		},

		[this]
		{
		});
}
