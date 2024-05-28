// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraRig_Rail.h"
#include "UfoCutSceneCameraRail.generated.h"

/**
 *
 */
UCLASS()
class UE5_ITT_API AUfoCutSceneCameraRail : public ACameraRig_Rail
{
	GENERATED_BODY()

public:
	AUfoCutSceneCameraRail(const FObjectInitializer& ObjectInitializer);

	virtual bool ShouldTickIfViewportsOnly() const override;

	enum class Fsm
	{
		Wait,
		Move,
	};
	virtual void Tick(float DeltaTime)override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_MoveRailCamera(float RailRatio);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	bool bTest = false;
	float CamMoveRatio = 0.333333f;
	UPROPERTY(EditAnywhere)
	class UCameraComponent* CamComp = nullptr;
	class UFsmComponent* FsmComp;
	void SetupFsm();
};
