// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnergyCore.generated.h"

UCLASS()
class UE5_ITT_API AEnergyCore : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnergyCore();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool IsExplode() const
	{
		return bExplode;
	}

	UFUNCTION(BlueprintCallable)
	void SetExplode()
	{
		bExplode = true;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* CoreMesh = nullptr;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* GlassMesh = nullptr;

	bool bExplode = false;

	


};
