// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LaserExplosionEffect.generated.h"

UCLASS()
class UE5_ITT_API ALaserExplosionEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaserExplosionEffect();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void EffectDestroy();
	void SetupDestroyTimerEvent();

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	class USceneComponent* SceneComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	class UNiagaraComponent* ExplosionEffectComp = nullptr;

	float DestroyDelayTime = 0.65f;
};
