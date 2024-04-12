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

	// 로즈 팔 모양 스켈레탈 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* ArmMeshComponent;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	

	// 메이, 코디 인형 메시
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* CodyMeshComponent;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* MayMeshComponent;

};
