// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OverlapCheckActor.generated.h"

UCLASS()
class UE5_ITT_API AOverlapCheckActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOverlapCheckActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	const bool IsOverlapPlayerActor() const { return bIsOverlapping; }

	UFUNCTION(BlueprintCallable)
	void SetOverlapActorNameTag(const FName ActorName) { OverlapActorTag = ActorName; }

	UFUNCTION(BlueprintCallable)
	APlayerBase* GetCurrentOverlapPlayer() const { return CurrentOverlapPlayer; }


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when an overlap begins
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called when an overlap ends
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:	
	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* StaticMeshComp = nullptr;

	UPROPERTY(EditAnywhere)
	bool bIsOverlapping = false;

	UPROPERTY(EditAnywhere)
	FName OverlapActorTag = "None";

	UPROPERTY(EditDefaultsOnly)
	class APlayerBase* CurrentOverlapPlayer = nullptr;
};
