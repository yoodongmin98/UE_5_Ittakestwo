// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraRig_Rail.h"
#include "PhaseEndCameraRail.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API APhaseEndCameraRail : public ACameraRig_Rail
{
	GENERATED_BODY()

public:
	APhaseEndCameraRail(const FObjectInitializer& ObjectInitializer);

	virtual bool ShouldTickIfViewportsOnly() const override;

	enum class Fsm
	{
		Wait,		
		Move,		
		End,	    
	};
	virtual void Tick(float DeltaTime)override;

	void EnableCameraMove();

protected:
	virtual void BeginPlay() override;

private:
	class UCameraComponent* CamComp = nullptr;
	class UFsmComponent* FsmComp = nullptr;

	void SetupFsmState();
	void SetupActorsRef();

	class ACody* PlayerCody = nullptr;
	class AMay* PlayerMay = nullptr;
	class AEnemyFlyingSaucer* EnemyFlyingSaucer = nullptr;

	FRotator LerpStartRotation = FRotator::ZeroRotator;
	FRotator LerpEndRotation = FRotator::ZeroRotator;
	float LerpRatio = 0.0f;
};
