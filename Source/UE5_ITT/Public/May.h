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
	virtual void Tick(float DeltaTime) override;
	virtual void DashEnd() override;
	virtual void BeginPlay() override;


	UFUNCTION(BlueprintCallable)
	inline void CutSceneStart()
	{
		MayCutSceneTrigger = true;
		CodyHoldEnemy = true;
	}
	UFUNCTION(BlueprintCallable)
	inline void SetMayMoveable()
	{
		CodyHoldEnemy = false;
	}

	UFUNCTION(BlueprintCallable)
	inline bool GetMayHoldEnemy() const
	{
		return CodyHoldEnemy;
	}



	virtual void SprintInput() override;
	UFUNCTION(Client, Reliable)
	void ClientSprintInput();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSprintInput();
private:
	bool MayCutSceneTrigger = false;
};
