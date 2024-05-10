// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GroundPoundEffect.generated.h"

UCLASS()
class UE5_ITT_API AGroundPoundEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGroundPoundEffect();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Component")
	class USceneComponent* SceneComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	class UNiagaraComponent* GroundPoundEffectComp = nullptr;
};
