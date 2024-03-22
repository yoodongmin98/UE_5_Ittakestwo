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
	ACody();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//Cody°ü·ÃFunction
	UFUNCTION(BlueprintCallable)
	inline Cody_State GetCodyState() const
	{
		return CodyState;
	}
	//FSM
	void ChangeState(Cody_State _State);

	void Cody_MoveFB(float _AxisValue);
	void Cody_MoveLR(float _AxisValue);

	//Animation Set
	UFUNCTION(BlueprintCallable)
	bool GetIsMoveEnd()
	{
		return IsMoveEnd;
	}
	UFUNCTION(BlueprintCallable)
	void SetIsMoveEndT()
	{
		IsMoveEnd = true;
	}
	UFUNCTION(BlueprintCallable)
	void SetIsMoveEndF()
	{
		IsMoveEnd = false;
	}
protected:
	virtual void BeginPlay() override;

private:
	//Mouse
	void Cody_LookUD(float _AxisValue);
	void Cody_LookLR(float _AxisValue);

	Cody_State CodyState = Cody_State::IDLE;


	float UpRotationSpeed = 70.0f;
	float RightRotationSpeed = 70.0f;

	//Animation Bool
	bool IsMoveEnd = false;
};
