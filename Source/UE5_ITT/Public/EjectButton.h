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
		OnWait,
		On,
		Off,
		Push,
		OffWait,
		End
	};
	
	UFUNCTION(BlueprintImplementableEvent)
	void ButtonPressedEvent();

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
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* SM_CollisionComp = nullptr;
	class UFsmComponent* FsmComp = nullptr;

	bool bOnPlayer = false;
	bool bPush = false;

	float MoveRatio = 0.f;
	FVector PlayerWaitPos = FVector::Zero();
	FVector MovePos = FVector::Zero();
	FVector PushPos = FVector::Zero();


	void SetupFsm();

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
