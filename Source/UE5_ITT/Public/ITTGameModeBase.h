// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ITTGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API AITTGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AITTGameModeBase();

	const TArray<AActor*> GetLoginPlayerControllers() const { return LoginPlayerControllers; }
	const int32 GetPlayerLoginCount() const { return PlayerLoginCount; }

protected:
	virtual void StartPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	
private:
	int32 PlayerLoginCount = 0;
	TArray<AActor*> LoginPlayerControllers;
};
