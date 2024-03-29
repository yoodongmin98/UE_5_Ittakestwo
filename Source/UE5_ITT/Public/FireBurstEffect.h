// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FireBurstEffect.generated.h"

UCLASS()
class UE5_ITT_API AFireBurstEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireBurstEffect();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* FireBurstMeshComp = nullptr;

	UPROPERTY(EditAnywhere)
	float MaxScale = 50.0f;
};
