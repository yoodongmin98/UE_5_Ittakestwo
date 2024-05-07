// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBase.h"
#include "May.generated.h"


UENUM(BlueprintType)
enum class MayWalkable
{
	GROUNDWALK UMETA(DisPlayName = "GROUNDWALK"),
	SIDEWALK UMETA(DisPlayName = "SIDEWALK"),
};

UCLASS()
class UE5_ITT_API AMay : public APlayerBase
{
	GENERATED_BODY()

public:
	AMay();

public:
	virtual void Tick(float DeltaTime) override;

	virtual void DashEnd() override;

public:



protected:
	virtual void BeginPlay() override;


private:
};
