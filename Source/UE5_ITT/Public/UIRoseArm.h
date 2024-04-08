// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UIRoseArm.generated.h"

UCLASS()
class UE5_ITT_API AUIRoseArm : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUIRoseArm();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	

	// ·ÎÁî ÆÈ ¸ð¾ç ½ºÄÌ·¹Å» ¸Þ½Ã
	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* SkeletalMeshComp = nullptr;


};
