// Fill out your copyright notice in the Description page of Project Settings.


#include "UfoCameraRail.h"
#include "FsmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"
#include "GameManager.h"

AUfoCameraRail::AUfoCameraRail(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);

		SetupFsm();
	}
}

void AUfoCameraRail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUfoCameraRail::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::Wait);
		CamComp->AttachToComponent(RailCameraMount, FAttachmentTransformRules::KeepRelativeTransform);
		Cast<UGameManager>(GetGameInstance())->AddCameraRigRail(TEXT("UfoRoad"), this);
	}
}

void AUfoCameraRail::SetupFsm()
{
	FsmComp->CreateState(Fsm::Wait,
		[this]
		{

		},

		[this](float DT)
		{
		},

		[this]
		{
		});
	FsmComp->CreateState(Fsm::Move,
		[this]
		{
		},

		[this](float DT)
		{			
		},

		[this]
		{
		});
}
