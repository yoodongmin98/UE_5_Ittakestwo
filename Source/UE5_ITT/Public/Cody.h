// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Cody.generated.h"

UCLASS()
class UE5_ITT_API ACody : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACody();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


//캐릭터 이동
private:
	void Cody_MoveFB(float _AxisValue);
	void Cody_MoveLR(float _AxisValue);
	void Cody_LookUD(float _AxisValue);
	void Cody_LookLR(float _AxisValue);

	float UpRotationSpeed = 70.0f;
	float RightRotationSpeed = 70.0f;

	//Animation
	UAnimMontage* Cody_Loco_Jog_Montage;
};
