// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CodyAnimNotify_Dash.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Cody.generated.h"



UENUM(BlueprintType)
enum class Cody_State : uint8
{
	IDLE UMETA(DisPlayName = "IDLE"),
	MOVE UMETA(DisPlayName = "MOVE"),
	SIT UMETA(DisPlayName = "SIT"),
	JUMP UMETA(DisPlayName = "JUMP"),
	DASH UMETA(DisPlayName = "DASH"),
};


UCLASS()
class UE5_ITT_API ACody : public ACharacter
{
	GENERATED_BODY()

public:
	//basic
	ACody();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//friend
	friend class UCodyAnimNotify_Dash;


	//Cody관련Function
	UFUNCTION(BlueprintCallable)
	inline Cody_State GetCodyState() const
	{
		return CodyState;
	}
	UFUNCTION(BlueprintCallable)
	bool GetIsMoveEnd() const
	{
		return IsMoveEnd;
	}
	void SetIsMoveEndT()
	{
		IsMoveEnd = true;
	}
	UFUNCTION(BlueprintCallable)
	bool GetbIsDashing()
	{
		return bIsDashing;
	}

	//Input
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* CodyMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LeftMAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* RightMAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* SitAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DashAction;




	//Test
	void ChangeState(Cody_State _State);


	
protected:
	virtual void BeginPlay() override;
	//Key Bind Function
	void Idle(const FInputActionInstance& _Instance);
	void Move(const FInputActionInstance& _Instance);
	void Look(const FInputActionInstance& _Instance);
	void Dash(const FInputActionInstance& _Instance);
	void DashEnd();

private:
	/////////////////Controller///////////////////
	APlayerController* CodyController = nullptr;
	//////////////////////////////////////////////
	 
	///////////////////State/////////////////////
	UPROPERTY(EditAnywhere, Category = State)
	Cody_State CodyState = Cody_State::IDLE;
	//////////////////////////////////////////////
public:
	///////////////////Player/////////////////////
	UPROPERTY(EditAnywhere,BlueprintReadWrite ,Category = Player)
	int32 PlayerHP = 0;
	//////////////////////////////////////////////

	//////////////////Movement////////////////////
	 // 대쉬 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
	float DashSpeed = 4000.0f;

	// 대쉬 지속 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
	float DashDuration = 0.5f;
	// 기본 이동 속도
	float DefaultWalkSpeed;
	float DashStartTime;
	bool bIsDashing;
	bool bIsMove;
	
	

private:
	//Test
	bool IsMoveEnd = true;
	UPROPERTY(EditAnywhere, Category = Player)
	float RotationInterpSpeed = 2.0f;
};
