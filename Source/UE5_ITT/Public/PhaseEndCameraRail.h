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

	UFUNCTION(BlueprintCallable)
	void EnableCameraMove(const float MoveRatio = 0.25f);
	
	UFUNCTION(BlueprintCallable)
	bool IsMoveEnd() { return bIsMoveEnd; }

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
	
	float CameraMoveRatio = 0.0f;
	bool bIsMoveCheck = false;		// 카메라 무브 완료 
	bool bIsMoveEnd = false;		// 완전히 종료 되었는지 
	float EndTime = 2.0f;			// 이전 카메라로 전환되기 까지의 텀 
};
