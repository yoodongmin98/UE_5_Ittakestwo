// Fill out your copyright notice in the Description page of Project Settings.


#include "UfoCutSceneCameraRail.h"
#include "FsmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameManager.h"

AUfoCutSceneCameraRail::AUfoCutSceneCameraRail(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);
		ColComp= CreateDefaultSubobject<USphereComponent>(TEXT("ColComp"));

		SetupFsm();
	}
}


void AUfoCutSceneCameraRail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AUfoCutSceneCameraRail::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::Wait);
		ColComp->OnComponentBeginOverlap.AddDynamic(this, &AUfoCutSceneCameraRail::OnOverlapBegin);
		Cast<UGameManager>(GetGameInstance())->AddCameraRigRail(TEXT("UfoCutScene"), this);
		CamMoveRatio = 0.333333f;
	}
}

void AUfoCutSceneCameraRail::SetupFsm()
{
	FsmComp->CreateState(Fsm::Wait,
		[this]
		{

		},

		[this](float DT)
		{	
			if (bStart)
			{
				FsmComp->ChangeState(Fsm::Delay0);
				ColComp->DestroyComponent();
			}
		},

		[this]
		{
		});

	FsmComp->CreateState(Fsm::Delay0,
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
				FsmComp->ChangeState(Fsm::Delay1);
			}
			Multicast_MoveRailCamera(DT * CamMoveRatio);
		},

		[this]
		{
		});

	FsmComp->CreateState(Fsm::Delay1,
		[this]
		{
		},

		[this](float DT)
		{
			if (FsmComp->GetStateLiveTime() > 1.f)
			{
				FsmComp->ChangeState(Fsm::End);
			}
		},

		[this]
		{
		});


	FsmComp->CreateState(Fsm::End,
		[this]
		{
			Cast<UGameManager>(GetGameInstance())->ChangeCameraView(TEXT("UfoRoad"));
		},

		[this](float DT)
		{
		},

		[this]
		{
		});
}

void AUfoCutSceneCameraRail::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && true == OtherActor->ActorHasTag("Player"))
	{
		UE_LOG(LogTemp, Display, TEXT("overlap"));
		Multicast_SetCameraView();
		bStart = true;
	}
}
