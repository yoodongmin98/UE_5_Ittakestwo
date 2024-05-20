// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FxElectric.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API AFxElectric : public AActor
{
	GENERATED_BODY()

public:
	AFxElectric(const FObjectInitializer& ObjectInitializer);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	enum class Fsm
	{
		ClientWait,
		Delay,
		Active,
		Wait,
	};

public:
	UFUNCTION(NetMulticast,Reliable)
	void NiagaraToggle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	class UFsmComponent* FsmComp = nullptr;
	
	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* NiagaraComp = nullptr;

	UPROPERTY(EditAnywhere)
	bool bDelay = false;

	float ClientWaitTime = 0.f;

	void SetupFsm();

};
