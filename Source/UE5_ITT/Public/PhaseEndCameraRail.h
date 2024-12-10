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

	
	void EnableCameraMove(const float MoveRatio = 0.25f);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastTickCameraMove(float DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	bool IsMoveEnd() { return bIsMoveEnd; }

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UPROPERTY(Replicated)
	class UCameraComponent* CamComp = nullptr;
	class UFsmComponent* FsmComp = nullptr;

	void SetupFsmState();
	void SetupActorsRef();

	class ACody* PlayerCody = nullptr;
	class AMay* PlayerMay = nullptr;
	class AEnemyFlyingSaucer* EnemyFlyingSaucer = nullptr;

	UPROPERTY(Replicated)
	float CameraMoveRatio = 0.0f;
	bool bIsMoveStart = false;
	bool bIsMoveCheck = false;		// ī�޶� ���� �Ϸ� 
	bool bIsMoveEnd = false;		// ������ ���� �Ǿ����� 
	float EndTime = 2.0f;			// ���� ī�޶�� ��ȯ�Ǳ� ������ �� 
};
