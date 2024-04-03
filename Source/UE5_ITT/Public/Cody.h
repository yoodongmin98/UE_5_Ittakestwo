// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBase.h"
#include "Cody.generated.h"


UCLASS()
class UE5_ITT_API ACody : public APlayerBase
{
private:
	GENERATED_BODY()
	ACody();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
