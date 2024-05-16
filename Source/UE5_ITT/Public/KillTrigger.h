// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "KillTrigger.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API AKillTrigger : public ATriggerBox
{
	GENERATED_BODY()
public:
	AKillTrigger();

protected:
	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
