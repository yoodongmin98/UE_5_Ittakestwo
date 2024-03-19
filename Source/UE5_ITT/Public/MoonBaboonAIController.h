// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MoonBaboonAIController.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API AMoonBaboonAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AMoonBaboonAIController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
