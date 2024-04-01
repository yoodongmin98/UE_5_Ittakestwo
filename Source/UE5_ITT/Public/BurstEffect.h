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
	UPROPERTY(EditAnywhere)
	class USceneComponent* SceneComp = nullptr;
	
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* StaticMeshComp = nullptr;



	UPROPERTY(EditAnywhere)
	float MaxScale = 10.0f;

	UPROPERTY(EditAnywhere)
	float BurstSpeed = 50.0f;

};
