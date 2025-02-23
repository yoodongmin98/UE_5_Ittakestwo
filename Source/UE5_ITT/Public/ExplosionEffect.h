// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosionEffect.generated.h"

UCLASS()
class UE5_ITT_API AExplosionEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosionEffect();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void EffectDestroy();
	void SetupDestroyTimerEvent();
	void SetupComponent();

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* SceneComp = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* ExplosionEffectComp = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float DestroyDelayTime = 3.0f;
};
