// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BurstEffect.generated.h"

UCLASS()
class UE5_ITT_API ABurstEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABurstEffect();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SetupComponent();
	void EffectDestroy();
	void SetupDestroyTimerEvent();

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* SceneComp = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* StaticMeshComp = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInstanceDynamic* DynamicMaterialInstance = nullptr;


	UPROPERTY(EditAnywhere)
	float MaxScale = 10.0f;

	UPROPERTY(EditAnywhere)
	float BurstSpeed = 50.0f;

	UPROPERTY(EditAnywhere)
	bool bDestroyValue = false;
};
