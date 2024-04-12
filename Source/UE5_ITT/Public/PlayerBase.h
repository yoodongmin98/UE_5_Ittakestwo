// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "PlayerBase.generated.h"


UENUM(BlueprintType)
enum class Cody_State : uint8
{
	IDLE UMETA(DisPlayName = "IDLE"),
	MOVE UMETA(DisPlayName = "MOVE"),
	SIT UMETA(DisPlayName = "SIT"),
	JUMP UMETA(DisPlayName = "JUMP"),
	DASH UMETA(DisPlayName = "DASH"),
	DASHEND UMETA(DisPlayName = "DASHEND"),
};

UCLASS()
class UE5_ITT_API APlayerBase : public ACharacter
{
	GENERATED_BODY()
	UInputComponent* Input;
public:
	UEnhancedInputComponent* PlayerInput;
	// Sets default values for this character's properties
	APlayerBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* InteractAction;


	//Cody의 현재 state를 반환합니다.
	UFUNCTION(BlueprintCallable)
	inline Cody_State GetITTPlayerState() const
	{
		return ITTPlayerState;
	}
	//캐릭터 스프링암에 포합된 카메라 컴포넌트를 반환합니다.
	FORCEINLINE class UCameraComponent* GetCodyCameraComponent() const
	{
		return PlayerCameraComponent;
	}
	//캐릭터의 스프링암을 반환합니다.
	FORCEINLINE class USpringArmComponent* GetSpringArm() const
	{
		return SpringArm;
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
	//대쉬의 지속시간을 반환합니다.
	UFUNCTION(BlueprintCallable)
	inline float GetDashDuration()
	{
		return DashDuration;
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
	// 대쉬가 다시 입력가능한 상태인지를 확인합니다.
	UFUNCTION(BlueprintCallable)
	inline bool GetbCanDash() const
	{
		return bCanDash;
	}
	//Player의 HP를 반환합니다.
	UFUNCTION(BlueprintCallable)
	inline int32 GetPlayerHP() const
	{
		return PlayerHP;
	}
	//카메라의 Vector2D를 반환합니다.
	UFUNCTION(BlueprintCallable)
	inline FVector2D GetCameraVector() const
	{
		return CameraLookVector;
	}
	//캐릭터에 적용되어있는 기본 지면 마찰력을 반환합니다.
	UFUNCTION(BlueprintCallable)
	inline float GetPlayerGroundFriction() const
	{
		return DefaultGroundFriction;
	}
	//캐릭터에 적용되어있는 기본 중력 스케일을 반환합니다.
	UFUNCTION(BlueprintCallable)
	inline float GetPlayerGravityScale() const
	{
		return DefaultGravityScale;
	}
	//애니메이션이 끝났을 때 호출됩니다
	UFUNCTION(BlueprintCallable)
	inline void SetCurrentAnimationEndT()
	{
		CurrentAnimationEnd = true;
	}
	UFUNCTION(BlueprintCallable)
	inline void SetCurrentAnimationEndF()
	{
		CurrentAnimationEnd = false;
	}
	//이전 애니메이션이 끝났는지를 반환합니다.
	UFUNCTION(BlueprintCallable)
	inline bool GetCurrentAnimationEnd() const
	{
		return CurrentAnimationEnd;
	}
	//상호작용 키를 눌렀을때 true를 반환합니다.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsInteract() const
	{
		return IsInteract;
	} 




	///////////////////Test///////////////////
	void ChangeState(Cody_State _State);
	//Test
	bool IsMoveEnd = true;
	float RotationInterpSpeed = 2.0f;

	bool IsInteract = false;





	//////////////////////////////////////////




	///////////////////Key Bind Function///////////////////
	void Idle(const FInputActionInstance& _Instance);
	void Move(const FInputActionInstance& _Instance);
	void Look(const FInputActionInstance& _Instance);
	void DashInput();
	void GroundDash();
	void JumpDash();
	void DashEnd();
	void DashNoneInput();
	void InteractInput();
	void InteractNoneInput();
	void Sit();





	/////////////////Controller///////////////////
	APlayerController* CodyController = nullptr;
	//////////////////////////////////////////////

	///////////////////State/////////////////////
	UPROPERTY(EditAnywhere, Category = State)
	Cody_State ITTPlayerState = Cody_State::IDLE;
	//////////////////////////////////////////////

	///////////////////Player/////////////////////
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Player)
	int32 PlayerHP = 0;
	//////////////////////////////////////////////

	//////////////////Camera////////////////////
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PlayerCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	float BigLength;
	float NormalLength;
	float SmallLength;

	FVector2D CameraLookVector = FVector2D::ZeroVector;
	//////////////////////////////////////////////

	//////////////////Movement////////////////////
	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashDistance = 2500.0f; // 앞구르기 거리
	UPROPERTY(EditAnywhere, Category = "Dash")
	float DashDuration; // 앞구르기 지속 시간
	float DashStartTime;
	bool bIsDashing; // 앞구르기 중 여부를 나타내는 플래그
	bool bIsDashingStart; //앞구르기 시작단계를 나타내는 플래그
	bool bCanDash;
	FTimerHandle DashTimerHandle; // 앞구르기 타이머 핸들
	float DefaultGroundFriction; // 기본 지면 마찰력
	float DefaultGravityScale; //기본 중력
	bool BigCanDash;
	//////////////////////////////////////////////

	///////////////////Animation//////////////////
	bool CurrentAnimationEnd;
	//////////////////////////////////////////////
};
