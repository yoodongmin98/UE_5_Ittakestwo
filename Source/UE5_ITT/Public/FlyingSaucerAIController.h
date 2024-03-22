// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FlyingSaucerAIController.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API AFlyingSaucerAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlyingSaucerAIController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void SetupPlayerReference();
	void SetupBehaviorTree();

	// 처음부터 코디, 메이를 들고 있는걸로 
	UPROPERTY(VisibleDefaultsOnly, Category = "Player Character")
	class APawn* PlayerCody;

	// Focus
	UPROPERTY(EditDefaultsOnly, Category = "Focus")
	bool bFocus = false;

	UPROPERTY(EditDefaultsOnly, Category = "Focus")
	float FocusRange = 300;

	// Behavior Tree
	UPROPERTY(EditDefaultsOnly, Category = "Behavior Tree")
	class UBehaviorTree* AIBehaviorTree;

};
