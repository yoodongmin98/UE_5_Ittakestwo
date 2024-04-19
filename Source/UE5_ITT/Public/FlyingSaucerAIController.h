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
	APawn* GetPlayer1Pawn() const { return PlayerRef1; }

	UFUNCTION(BlueprintCallable)
	APawn* GetPlayer2Pawn() const { return PlayerRef2; }

	
	UFUNCTION(BlueprintCallable)
	FVector GetTargetPrevLocation() const { return PrevTargetLocation; }

	UFUNCTION(BlueprintCallable)
	void AddPatternMatchCount();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
	UPROPERTY(EditDefaultsOnly, Category = "AI", Replicated)
	class UBehaviorTree* AIBehaviorTreePhase1 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI", Replicated)
	class UBehaviorTree* AIBehaviorTreePhase2 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI", Replicated)
	class UBehaviorTree* AIBehaviorTreePhase3 = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI", Replicated)
	class UBehaviorTree* CurrentBehaviorTree = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI", Replicated)
	class UBehaviorTreeComponent* CurrentBehaviorTreeComp = nullptr;



	UPROPERTY(EditDefaultsOnly)
	class AFloor* FloorObject = nullptr;

	UPROPERTY(EditDefaultsOnly, Replicated)
	FVector PrevTargetLocation = FVector::ZeroVector;
	
	UPROPERTY(EditDefaultsOnly, Replicated)
	FVector PrevTargetLocationBuffer = FVector::ZeroVector;
	
	UPROPERTY(EditDefaultsOnly, Replicated)
	bool bPrevTargetLocationValid = false;
	
	UPROPERTY(EditDefaultsOnly, Replicated)
	float LaserLerpRatio = 0.0f;

	UPROPERTY(EditDefaultsOnly, Replicated)
	float LaserLerpRate = 25.0f;
		
	UPROPERTY(EditDefaultsOnly, Replicated)
	int32 PatternMatchCount = 0;

	UPROPERTY(EditDefaultsOnly)
	bool bIsSetupPlayerRef= false;
};
