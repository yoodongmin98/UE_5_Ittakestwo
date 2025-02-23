// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "RespawnTrigger.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API ARespawnTrigger : public ATriggerBox
{
	GENERATED_BODY()

public:
	ARespawnTrigger();

	FVector GetRespawnPosition()
	{
		return RespawnPivotComp->GetComponentLocation();
	}

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	class USceneComponent* RespawnPivotComp = nullptr;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
