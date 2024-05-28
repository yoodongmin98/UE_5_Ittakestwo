// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraRig_Rail.h"
#include "ParentCameraRail.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API AParentCameraRail : public ACameraRig_Rail
{
	GENERATED_BODY()

public:
	AParentCameraRail(const FObjectInitializer& ObjectInitializer);


	virtual void Tick(float DeltaTime)override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCameraView();

protected:
	virtual void BeginPlay() override;
	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual bool IsSupportedForNetworking() const override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_MoveRailCamera(float RailRatio);

	UPROPERTY(EditAnywhere)
	class UCameraComponent* CamComp = nullptr;
	class UFsmComponent* FsmComp;
	float CamMoveRatio = 1.f;
private:
};
