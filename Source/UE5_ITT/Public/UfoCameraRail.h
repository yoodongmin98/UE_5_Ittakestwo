// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraRail.h"
#include "UfoCameraRail.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API AUfoCameraRail : public ACameraRail
{
	GENERATED_BODY()

public:
	AUfoCameraRail();
	virtual void Tick(float DeltaTime) override;

	enum class Fsm
	{
		Wait,
		Move,
	};

private:
	void SetupFsm();
};
