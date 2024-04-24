// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraRig_Rail.h"
#include "CameraRail.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API ACameraRail : public ACameraRig_Rail
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
};
