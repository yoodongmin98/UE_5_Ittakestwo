// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EjectButton.generated.h"

UCLASS()
class UE5_ITT_API AEjectButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEjectButton();

	enum class Fsm
	{
		Wait,
		On,
		Push,
		End
	};
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SM_BaseComp = nullptr;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SM_PushComp = nullptr;
	class UFsmComponent* FsmComp = nullptr;

	void SetupFsm();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
