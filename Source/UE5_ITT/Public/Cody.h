// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Cody.generated.h"

UENUM(BlueprintType)
enum class Cody_State : uint8
{
	IDLE,
	MOVE,
	TURN,
	JUMP,
	DASH
};

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

	//Mouse
	void Cody_LookUD(float _AxisValue);
	void Cody_LookLR(float _AxisValue);

	Cody_State CodyState = Cody_State::IDLE;

	float UpRotationSpeed = 70.0f;
	float RightRotationSpeed = 70.0f;

public:

	//Cody관련Function
	UFUNCTION(BlueprintCallable)
	inline Cody_State GetCodyState() const
	{
		return CodyState;
	}
	//FSM
	void ChangeState(Cody_State _State);

	void Cody_MoveFB(float _AxisValue);
	void Cody_MoveLR(float _AxisValue);
};
