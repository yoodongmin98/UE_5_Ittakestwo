// Fill out your copyright notice in the Description page of Project Settings.


#include "UfoCameraRail.h"
#include "FsmComponent.h"

AUfoCameraRail::AUfoCameraRail()
{
	SetupFsm();
}

void AUfoCameraRail::Tick(float DeltaTime)
{
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

		},

		[this]
		{
		});
}
