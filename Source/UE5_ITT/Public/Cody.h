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

	UInputComponent* Input;
	//friend
	friend class UCodyAnimNotify_Dash;


	//Cody의 현재 state를 반환합니다.
	UFUNCTION(BlueprintCallable) 
	inline Cody_State GetCodyState() const
	{
		return CodyState;
	}
	//플레이어의 이동이 끝났는지를 반환합니다.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsMoveEnd() const
	{
		return IsMoveEnd;
	}
	//이동이 끝났을때 호출됩니다(CodyAnimNotify.cpp)
	inline void SetIsMoveEndT()
	{
		IsMoveEnd = true;
	}
	//대쉬 키를 눌렀는지를 반환합니다.
	UFUNCTION(BlueprintCallable)
	inline bool GetbIsDashing() const
	{
		return bIsDashing;
	}
	//대쉬시작 애니메이션이 끝났는지를 확인합니다
	UFUNCTION(BlueprintCallable)
	inline bool GetbIsDashingStart() const
	{
		return bIsDashingStart;
	}
	//대쉬 시작 애니메이션이 끝났을 때 호출됩니다(CodyNotify_DashStart.cpp)
	inline void SetbIsDashingStart()
	{
		bIsDashingStart = true;
	}

	///////////////////Input///////////////////

	//키가 매핑된 Context입니다
	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* CodyMappingContext;

	//IA_Jump
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;
	//IA_Move
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveAction;
	//IA_Look
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;
	//IA_LeftM
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LeftMAction;
	//IA_RightM
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* RightMAction;
	//IA_Sit
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* SitAction;
	//IA_Dash
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DashAction;




	///////////////////Test///////////////////
	void ChangeState(Cody_State _State);








	//////////////////////////////////////////

protected:
	
	virtual void BeginPlay() override;

	///////////////////Key Bind Function///////////////////
	void Idle(const FInputActionInstance& _Instance);
	void Move(const FInputActionInstance& _Instance);
	void Look(const FInputActionInstance& _Instance);
	void DashInput();
	void GroundDash();
	void JumpDash();
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
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDistance = 2000.0f; // 앞구르기 거리
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashDuration = 0.7f; // 앞구르기 지속 시간

	bool bIsDashing; // 앞구르기 중 여부를 나타내는 플래그
	bool bIsDashingStart; //앞구르기 시작단계를 나타내는 플래그
	FTimerHandle DashTimerHandle; // 앞구르기 타이머 핸들
	float DefaultGroundFriction; // 기본 지면 마찰력
	float DefaultGravityScale;

private:
	//Test
	bool IsMoveEnd = true;
	UPROPERTY(EditAnywhere, Category = Player)
	float RotationInterpSpeed = 2.0f;
};
