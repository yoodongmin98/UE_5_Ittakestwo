// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "RotatingFan.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API ARotatingFan : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	ARotatingFan();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
