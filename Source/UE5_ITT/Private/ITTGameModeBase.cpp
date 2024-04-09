// Fill out your copyright notice in the Description page of Project Settings.


#include "ITTGameModeBase.h"
#include "Kismet/GameplayStatics.h"


AITTGameModeBase::AITTGameModeBase()
{
}

void AITTGameModeBase::StartPlay()
{
	Super::StartPlay();
}

void AITTGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	++PlayerLoginCount;
	if (2 == PlayerLoginCount)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), LoginPlayerControllers);
	}
}

void AITTGameModeBase::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

