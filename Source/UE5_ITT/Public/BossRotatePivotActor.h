// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossRotatePivotActor.generated.h"

UCLASS()
class UE5_ITT_API ABossRotatePivotActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossRotatePivotActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* SceneComp = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* StaticMeshComp = nullptr;
};
