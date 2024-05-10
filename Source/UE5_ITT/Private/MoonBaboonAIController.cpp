// Fill out your copyright notice in the Description page of Project Settings.


#include "MoonBaboonAIController.h"

AMoonBaboonAIController::AMoonBaboonAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);
	}
}

void AMoonBaboonAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AMoonBaboonAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
