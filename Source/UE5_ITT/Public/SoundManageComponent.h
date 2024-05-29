// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "SoundManageComponent.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API USoundManageComponent : public UAudioComponent
{
	GENERATED_BODY()
	
public:
	USoundCue* GetCurSound();
	void ChangeSound(const FString& KeyName);


protected:

private:
	USoundCue* CurSound = nullptr;

};
