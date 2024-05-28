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
		Delay,
		Move,
		End,
	};
	virtual void Tick(float DeltaTime)override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_MoveRailCamera(float RailRatio);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetCameraView();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	class USphereComponent* ColComp = nullptr;
	bool bStart = false;

	float CamMoveRatio = 0.333333f;
	UPROPERTY(EditAnywhere)
	class UCameraComponent* CamComp = nullptr;
	class UFsmComponent* FsmComp;
	void SetupFsm();


	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
