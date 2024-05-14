// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ParentShutter.generated.h"

UCLASS()
class UE5_ITT_API AParentShutter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AParentShutter();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	//각 셔터 날개에게 오픈 전달
	void SetShutterOpen();
	void SetDone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	TArray<class ACoreShutter*> ArrayCoreShutter;

	UPROPERTY(EditAnywhere)
	class ACoreShutter* Shutter0= nullptr;
	UPROPERTY(EditAnywhere)
	class ACoreShutter* Shutter1= nullptr;
	UPROPERTY(EditAnywhere)
	class ACoreShutter* Shutter2= nullptr;
	UPROPERTY(EditAnywhere)
	class ACoreShutter* Shutter3= nullptr;

	float MovingSize = 170.f;
};
