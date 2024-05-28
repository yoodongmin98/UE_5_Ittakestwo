// Fill out your copyright notice in the Description page of Project Settings.


#include "UfoCutSceneCameraRail.h"
#include "FsmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

AUfoCutSceneCameraRail::AUfoCutSceneCameraRail(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);
		ColComp = CreateDefaultSubobject<USphereComponent>(TEXT("ColComp"));
		ColComp->SetupAttachment(RootComponent);

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

void AUfoCutSceneCameraRail::Multicast_SetCameraView_Implementation()
{
	FConstPlayerControllerIterator PlayerControllerIter = GetWorld()->GetPlayerControllerIterator();

	if (HasAuthority() == true)
	{
		++++PlayerControllerIter;
	}
	else
	{
		++PlayerControllerIter;
	}

	PlayerControllerIter->Get()->SetViewTargetWithBlend(this, 0.1f);
}

void AUfoCutSceneCameraRail::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::Wait);
		ColComp->OnComponentBeginOverlap.AddDynamic(this, &AUfoCutSceneCameraRail::OnOverlapBegin);
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
			if (bStart)
			{
				FsmComp->ChangeState(Fsm::Delay);
			}
		},

		[this]
		{
		});

	FsmComp->CreateState(Fsm::Delay,
		[this]
		{
		},

		[this](float DT)
		{
			if (FsmComp->GetStateLiveTime()>1.f)
			{
				FsmComp->ChangeState(Fsm::Move);
			}
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
			if (CurrentPositionOnRail>=1.f)
			{

			}
			Multicast_MoveRailCamera(DT * CamMoveRatio);
		},

		[this]
		{
		});


	FsmComp->CreateState(Fsm::End,
		[this]
		{
		},

		[this](float DT)
		{
			if (CurrentPositionOnRail >= 1.f)
			{
				FsmComp->ChangeState(Fsm::End);
			}
		},

		[this]
		{
		});
}

void AUfoCutSceneCameraRail::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && true == OtherActor->ActorHasTag("Player"))
	{
		Multicast_SetCameraView();
		bStart = true;
	}
}
