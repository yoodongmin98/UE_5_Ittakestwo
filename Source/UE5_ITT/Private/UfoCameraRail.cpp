// Fill out your copyright notice in the Description page of Project Settings.


#include "UfoCameraRail.h"
#include "FsmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"

AUfoCameraRail::AUfoCameraRail(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);

		//CamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CamComp"));

		SetupFsm();
	}
}

bool AUfoCameraRail::ShouldTickIfViewportsOnly() const
{
	return false;
}


void AUfoCameraRail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUfoCameraRail::MoveRailCamera_Implementation(float RailRatio)
{
	CurrentPositionOnRail = RailRatio;
	CamComp->SetWorldLocation(GetRailSplineComponent()->GetLocationAtTime(CurrentPositionOnRail, ESplineCoordinateSpace::World));
}

void AUfoCameraRail::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::Wait);
		//CamComp->AttachToComponent(RailCameraMount, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void AUfoCameraRail::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));
	FsmComp->CreateState(Fsm::Wait,
		[this]
		{

		},

		[this](float DT)
		{
			FsmComp->ChangeState(Fsm::Move);
		},

		[this]
		{
		});
	FsmComp->CreateState(Fsm::Move,
		[this]
		{
			CurrentPositionOnRail = 0;
		},

		[this](float DT)
		{			
			CurrentPositionOnRail += DT*0.1;
			MoveRailCamera(CurrentPositionOnRail);
		},

		[this]
		{
		});
}
