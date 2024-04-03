// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBase.h"
#include "May.generated.h"


UCLASS()
class UE5_ITT_API AMay : public APlayerBase
{
private:
	GENERATED_BODY()
	AMay();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
