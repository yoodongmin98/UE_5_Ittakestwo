// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GravityPath.generated.h"

UCLASS()
class UE5_ITT_API AGravityPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGravityPath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshBot = nullptr;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshTop = nullptr;


	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ColBot = nullptr;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ColTop = nullptr;

	class AMay* PlayerMay = nullptr;

	FHitResult HitResult;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
