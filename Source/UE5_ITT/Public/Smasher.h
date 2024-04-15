// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Smasher.generated.h"

UCLASS()
class UE5_ITT_API ASmasher : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASmasher();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	enum class Fsm
	{
		SmashWait,
		Smash,
		ReleaseWait,
		Release,
	};
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SmasherMesh = nullptr;
	UPROPERTY(EditAnywhere)
	class USceneComponent* SceneComp = nullptr;
	class UFsmComponent* FsmComp = nullptr;

	float SmashSize = 25.f;
	float SmashRatio =0.f;

	UPROPERTY(EditAnywhere)
	FVector ReleasePos = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	FVector SmashPos = FVector::ZeroVector;

	void SetupFsm();
};
