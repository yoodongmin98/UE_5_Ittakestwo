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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	APawn* GetPlayerPawn() const { return PlayerRef1; }
	
	UFUNCTION(BlueprintCallable)
	FVector GetTargetPrevLocation() const { return PrevTargetLocation; }

	UFUNCTION(BlueprintCallable)
	void AddPatternMatchCount();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetupPlayerRefAndBehaviorTreePhase1();
	
	UFUNCTION(BlueprintCallable)
	void SetupStartBehaviorTreePhase1();
	UFUNCTION(BlueprintCallable)
	void SetupStartBehaviorTreePhase2();
	UFUNCTION(BlueprintCallable)
	void SetupStartBehaviorTreePhase3();
	void SavePreviousTargetLocation();
	void UpdateLerpRatioForLaserBeam(float DeltaTime);

	UPROPERTY(VisibleDefaultsOnly, Category = "Player Character")
	class APawn* PlayerRef1;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player Character")
	class APawn* PlayerRef2;

	// Behavior Tree
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* AIBehaviorTreePhase1 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* AIBehaviorTreePhase2 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* AIBehaviorTreePhase3 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* CurrentBehaviorTree = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTreeComponent* CurrentBehaviorTreeComp = nullptr;



	UPROPERTY(EditDefaultsOnly)
	class AFloor* FloorObject = nullptr;

	FVector PrevTargetLocation = FVector::ZeroVector;
	FVector PrevTargetLocationBuffer = FVector::ZeroVector;
	bool bPrevTargetLocationValid = false;
	float LaserLerpRatio = 0.0f;
	float LaserLerpRate = 25.0f;
		
	UPROPERTY(EditAnywhere)
	int32 PatternMatchCount = 0;

	UPROPERTY(EditDefaultsOnly)
	bool bIsSetupPlayerRef= false;
};
