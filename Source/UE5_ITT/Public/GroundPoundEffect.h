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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	class USceneComponent* SceneComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	class UNiagaraComponent* GroundPoundEffectComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	class UStaticMeshComponent* StaticMeshComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Replicated)
	bool bIsPlayerOverlap = false;

	UPROPERTY(EditDefaultsOnly)
	class APlayerBase* OverlapPlayer = nullptr;

	UPROPERTY(EditDefaultsOnly)
	int32 DamageToPlayer = 1;
};
