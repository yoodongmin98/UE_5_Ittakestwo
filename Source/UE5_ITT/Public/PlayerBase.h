// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RespawnTrigger.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
//#include "Interfaces/OnlineSessionInterface.h"


#include "PlayerBase.generated.h"

UENUM(BlueprintType)
enum class CodySize : uint8
{
	NONE UMETA(DisPlayName = "NONE"),
	BIG UMETA(DisPlayName = "BIG"),
	NORMAL UMETA(DisPlayName = "NORMAL"),
	SMALL UMETA(DisPlayName = "SMALL"),
};
UENUM(BlueprintType)
enum class Cody_State : uint8
{
	IDLE UMETA(DisPlayName = "IDLE"),
	MOVE UMETA(DisPlayName = "MOVE"),
	SIT UMETA(DisPlayName = "SIT"),
	JUMP UMETA(DisPlayName = "JUMP"),
	DASH UMETA(DisPlayName = "DASH"),
	DASHEND UMETA(DisPlayName = "DASHEND"),
	PlayerDeath UMETA(DisPlayName = "PlayerDeath"),
	HOLDENEMY UMETA(DisPlayName = "HoldEnemy"),
	FLYING UMETA(DisPlayName = "FLYING"),
	CUTSCENE UMETA(DisPlayName = "CUTSCENE"),
};

UCLASS()
class UE5_ITT_API APlayerBase : public ACharacter
{
	GENERATED_BODY()
	UInputComponent* Input;
	ARespawnTrigger* ResPawnTriggers = nullptr;
public:
	UEnhancedInputComponent* PlayerInput;
	// Sets default values for this character's properties
	APlayerBase();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	///////////////////State///////////////////
	UFUNCTION(BlueprintCallable)
	inline void ChangeState(Cody_State _State)
	{
		ITTPlayerState = _State;
	}
	///////////////////////////////////////////
	
	///////////////////Input///////////////////
	//Ű�� ���ε� Context�Դϴ�
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
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* TestAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* FlyMoveAction;
	///////////////////////////////////////////






	/////////////////Function/////////////////
	//Cody�� ���� state�� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline Cody_State GetITTPlayerState() const
	{
		return ITTPlayerState;
	}
	//ĳ���� �������Ͽ� ���յ� ī�޶� ������Ʈ�� ��ȯ�մϴ�.
	FORCEINLINE class UCameraComponent* GetCodyCameraComponent() const
	{
		return PlayerCameraComponent;
	}
	//ĳ������ ���������� ��ȯ�մϴ�.
	FORCEINLINE class USpringArmComponent* GetSpringArm() const
	{
		return SpringArm;
	}
	//�÷��̾��� �̵��� ���������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsMoveEnd() const
	{
		return IsMoveEnd;
	}
	//�̵��� �������� ȣ��˴ϴ�(CodyAnimNotify.cpp)
	inline void SetIsMoveEndT()
	{
		IsMoveEnd = true;
	}
	//�뽬 Ű�� ���������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetbIsDashing() const
	{
		return bIsDashing;
	}
	//�뽬�� ���ӽð��� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline float GetDashDuration()
	{
		return DashDuration;
	}
	//�뽬���� �ִϸ��̼��� ���������� Ȯ���մϴ�
	UFUNCTION(BlueprintCallable)
	inline bool GetbIsDashingStart() const
	{
		return bIsDashingStart;
	}
	//�뽬 ���� �ִϸ��̼��� ������ �� ȣ��˴ϴ�(CodyNotify_DashStart.cpp)
	inline void SetbIsDashingStart()
	{
		bIsDashingStart = true;
	}
	// �뽬�� �ٽ� �Է°����� ���������� Ȯ���մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetbCanDash() const
	{
		return bCanDash;
	}
	//Player�� HP�� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline int32 GetPlayerHP() const
	{
		return PlayerHP;
	}
	//Player�� �ִ�ü���� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline int32 GetPlayerFullHP() const
	{
		return FullHP;
	}

	//Player�� �����ϴ� �Լ��Դϴ� �μ�=������
	UFUNCTION(BlueprintCallable)
	void AttackPlayer(const int att);
	
	UFUNCTION(Client, Reliable)
	void AttackPlayerClient(const int att);
	UFUNCTION(Server, Reliable, WithValidation)
	void AttackPlayerServer(const int att);

	//ī�޶��� Vector2D�� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline FVector2D GetCameraVector() const
	{
		return CameraLookVector;
	}
	//ĳ���Ϳ� ����Ǿ��ִ� �⺻ ���� �������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline float GetPlayerGroundFriction() const
	{
		return DefaultGroundFriction;
	}
	//ĳ���Ϳ� ����Ǿ��ִ� �⺻ �߷� �������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline float GetPlayerDefaultGravityScale() const
	{
		return DefaultGravityScale;
	}
	//ĳ���Ϳ� ���� ����Ǿ��ִ� �߷� �������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline float GetPlayerGravityScale() const
	{
		return GetCharacterMovement()->GravityScale;
	}
	//���� �߷��� ĳ���� Scale�� ������� ���� ����ǰ��ִ����� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline float GetIsDGravityScale() const
	{
		return IsDGravity;
	}
	//�ִϸ��̼��� ������ �� ȣ��˴ϴ�
	UFUNCTION(BlueprintCallable)
	inline void SetCurrentAnimationEndT()
	{
		CurrentAnimationEnd = true;
	}
	//�ִϸ��̼��� ������ ���� ���·� �����մϴ�.
	UFUNCTION(BlueprintCallable)
	inline void SetCurrentAnimationEndF()
	{
		CurrentAnimationEnd = false;
	}
	//���� �ִϸ��̼��� ���������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetCurrentAnimationEnd() const
	{
		return CurrentAnimationEnd;
	}
	//��ȣ�ۿ� Ű�� �������� true�� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsInteract() const
	{
		return IsInteract;
	}
	// Player�� �޸����ִ� ���������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsSprint() const
	{
		return IsSprint;
	}
	//Player�� �������������� ��ȯ�մϴ�. ������UI visible���δ� �̰ɷ� Ȯ��.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsPlayerDeath() const
	{
		return IsPlayerDeath;
	}
	//�÷��̾ �׾��ִ� ���·� ����ϴ�.
	UFUNCTION(BlueprintCallable)
	inline void SetPlayerDeath(bool IsDeath = true)
	{
		IsPlayerDeath = IsDeath;
	}
	//Sit���� �������� �ð��� �Ǿ������� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetCurrentSitTime() const
	{
		return CurrentSitTime >= SitStartTime + SitDuration - 0.15f;
	}
	//ĳ���Ͱ� ���鿡 �ִ����� ��ȯ�մϴ�
	UFUNCTION(BlueprintCallable)
	inline bool GetIsCharacterGround() const
	{
		return !GetMovementComponent()->IsFalling();
	}
	//Idle�� ���ư� �� �ִ� ���·� ����ϴ�.(Sit)
	UFUNCTION(BlueprintCallable)
	inline void SetChangeIdle()
	{
		ChangeIdle = true;
	}
	//Idle�� ���ư� �� �ִ� ���������� ��ȯ�մϴ�.(Sit)
	UFUNCTION(BlueprintCallable)
	inline bool GetChangeIdle() const
	{
		return ChangeIdle;
	}
	//���� ĳ���Ͱ� ���° ������ �ϰ��ִ����� ��ȯ�մϴ�
	UFUNCTION(BlueprintCallable)
	inline int32 GetCharacterJumpCount() const
	{
		return CharacterJumpCount;
	}
	//�ɱ�����ϴ¸���� �������� ���θ� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsSit() const
	{
		return IsSit;
	}
	//Cody�� ū ������������ ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline bool GetIsBig() const
	{
		return IsBig;
	}
	//Player�� Fly ���������� ��ȯ�մϴ�
	UFUNCTION(BlueprintCallable)
	inline bool GetIsFly() const
	{
		return IsFly;
	}
	//�÷��̾ �������� ���� ���·� ����ϴ�.
	UFUNCTION(BlueprintCallable)
	inline void SetFlyFalse()
	{
		IsFly = false;
	}
	UFUNCTION(BlueprintCallable)
	inline void OverlapHomingFunc()
	{
		OverlapHoming = true;
	}
	UFUNCTION(BlueprintCallable)
	inline void SetOverlapHoming()
	{
		OverlapHoming = false;
	}
	UFUNCTION(BlueprintCallable)
	inline bool GetOverlapHoming() const
	{
		return OverlapHoming;
	}
	//Cody�� EnemyBoss�� ����ִ� ���·� ����ϴ�.(�����ӿ� �����մϴ�)
	UFUNCTION(BlueprintCallable)
	inline void SetCodyHoldEnemyTrue()
	{
		CodyHoldEnemy = true;
	}
	//Cody�� EnemyBoss�� ������� ���� ���·� ����ϴ�.
	UFUNCTION(BlueprintCallable)
	inline void SetCodyHoldEnemyFalse()
	{
		CodyHoldEnemy = false;
	}
	//ĳ���Ͱ� �������Ǵ� ��ġ�� Vector���� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable)
	inline FVector GetResPawnPosition()
	{
		return ResPawnPosition;
	}
	//ResPawn���� TriggerActor�� �浹������ ȣ��˴ϴ�. �μ�=�浹��TriggerActor
	UFUNCTION(BlueprintCallable)
	inline void SetTriggerActors(ARespawnTrigger* _Other);
	//�浹�� TriggerActor�� Position�� �޾� Respawn������ �����ϴ� �Լ�.
	UFUNCTION(BlueprintCallable)
	inline void SetRespawnPosition();



	


	///////////////////Movement+Replicate Function///////////////////
	UFUNCTION(Client, Reliable)
	void CustomClientIdle();
	UFUNCTION(Server, Reliable, WithValidation)
	void CustomServerIdle();

	UFUNCTION(Client,Reliable)
	void ChangeClientDir(FRotator _Rotator);
	UFUNCTION(Server,Reliable, WithValidation)
	void ChangeServerDir(FRotator _Rotator);

	UFUNCTION(Client, Reliable)
	void ChangeClientFlyDir(FRotator _Rotator);
	UFUNCTION(Server, Reliable, WithValidation)
	void ChangeServerFlyDir(FRotator _Rotator);

	UFUNCTION(Client, Reliable)
	void CustomClientDash();
	UFUNCTION(Server, Reliable, WithValidation)
	void CustomServerDash();
	
	UFUNCTION(Server, Reliable)
	void InteractInput();
	UFUNCTION(Server, Reliable)
	void InteractNoneInput();


	void Idle(const FInputActionInstance& _Instance);
	void Look(const FInputActionInstance& _Instance);
	virtual void SprintInput() {};
	virtual void SprintNoneInput() {};
	void CustomMove(const FInputActionInstance& _Instance);
	void CustomFlyMove(const FInputActionInstance& _Instance);
	void CustomFlyNoneMove(const FInputActionInstance& _Instance);
	void DashInput();
	void DashNoneInput();
	void GroundDash();
	virtual void DashEnd() {};
	void JumpDash();
	///////////////////////////////////////////////////////

	//void GetOnlineSubsystem();
	//IOnlineSessionPtr OnlineSeesioninterface;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	///////////////////Widget//////////////////
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class UMarkerWidget* MarkerUIWidget;

	UFUNCTION(BlueprintCallable)
	void SettingMarkerWidget();
	///////////////////////////////////////////



	///////////////////Overlap//////////////////
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	////////////////////////////////////////////



	///////////////////Rocket//////////////////
	void PlayerToHomingRocketJumpStart();

	UFUNCTION(BlueprintCallable)
	inline void PlayerToHomingRoketJumpFinished();
	UFUNCTION(Client, Reliable)
	inline void ClientPlayerToHomingRoketJumpFinished();
	UFUNCTION(Server, Reliable, WithValidation)
	inline void ServerPlayerToHomingRoketJumpFinished();

	UFUNCTION(Client, Reliable)
	void CustomClientRideJump();
	UFUNCTION(Server, Reliable, WithValidation)
	void CustomServerRideJump();
	///////////////////////////////////////////
	

	

	///////////////////??????//////////////////

	UPROPERTY(Replicated)
	FRotator CurControllerRot;

	UFUNCTION(BlueprintCallable)
	inline FRotator GetCurControllerRot() const
	{
		return CurControllerRot;
	}
	UPROPERTY(Replicated)
	FVector CurControllerLoc;

	UFUNCTION(BlueprintCallable)
	inline FVector GetCurControllerLoc() const
	{
		return CurControllerLoc;
	}
	void UpdateCamTrans();
	///////////////////////////////////////////


protected:
	///////////////////Player/////////////////////
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Player)
	int32 PlayerHP = 12;
	int32 FullHP = 12;
	UPROPERTY(Replicated)
	bool IsPlayerDeath = false;


	UFUNCTION(BlueprintCallable)
	void PlayerDeathCheck();
	UPROPERTY(Replicated)
	float NowPlayerGravityScale;
	UPROPERTY(Replicated)
	float PlayerJumpZVelocity;
	float PlayerDefaultJumpHeight;
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
	UPROPERTY(Replicated)
	FVector ResPawnPosition = FVector(1000.0f, 1000.0f, 100.0f); //�ӽ�

	//SpringArm�⺻ ���� �Լ��Դϴ�.
	UFUNCTION(BlueprintCallable)
	void SpringArmDefaultFunction();
	//////////////////////////////////////////////
	//////////////////Movement////////////////////
	UPROPERTY(Replicated)
	float DashDistance = 2500.0f; // �ձ����� �Ÿ�
	bool IsDGravity;
	UPROPERTY(Replicated)
	float PlayerDefaultSpeed;
	UPROPERTY(Replicated)
	float NowPlayerSpeed;
	UPROPERTY(Replicated = OnRep_IsMoveEnd)
	bool IsMoveEnd; //�������� ��������
	UFUNCTION()
	void OnRep_IsMoveEnd();
	UPROPERTY(Replicated)
	float DashDuration; // �ձ����� ���� �ð�
	UPROPERTY(Replicated)
	float DashStartTime;//�ձ����� ���۽ð� üũ
	UPROPERTY(Replicated)
	bool bIsDashing; // �ձ����� �� ���θ� ��Ÿ���� �÷���
	UPROPERTY(Replicated)
	bool bIsDashingStart; //�ձ����� ���۴ܰ踦 ��Ÿ���� �÷���
	UPROPERTY(Replicated)
	bool bCanDash; //�뽬�� ������ ��������
	FTimerHandle DashTimerHandle; // �ձ����� Ÿ�̸� �ڵ�
	float DefaultGroundFriction; // �⺻ ���� ������
	UPROPERTY(Replicated)
	float DefaultGravityScale; //�⺻ �߷�
	bool BigCanDash; //Ŀ�����¿��� �뽬�� �������� ����
	UPROPERTY(Replicated)
	float DashCurrentTime;
	UPROPERTY(Replicated)
	bool IsSprint; //�޸��� ����

	UPROPERTY(Replicated)
	float SitStartTime;
	UPROPERTY(Replicated)
	float SitDuration;
	UPROPERTY(Replicated)
	bool CanSit;
	UPROPERTY(Replicated)
	bool IsSit;
	UPROPERTY(Replicated)
	float CurrentSitTime;
	UPROPERTY(Replicated)
	bool ChangeIdle;
	UPROPERTY(Replicated)
	int32 CharacterJumpCount;
	UPROPERTY(Replicated)
	FVector MoveDirection;
	UPROPERTY(Replicated)
	FVector2D MoveInput;
	UPROPERTY(Replicated)
	FRotator ControllerRotation;
	UPROPERTY(Replicated)
	FRotator CustomTargetRotation;
	UPROPERTY(Replicated)
	FVector WorldForwardVector;
	UPROPERTY(Replicated)
	FVector WorldRightVector;
	UPROPERTY(Replicated)
	FVector DashDirection;
	UPROPERTY(Replicated)
	FVector DashVelocity;

	UPROPERTY(Replicated)
	int32 CustomPlayerJumpCount;

	UPROPERTY(Replicated)
	FVector FlyForwardVector;

	//Fly
	UPROPERTY(Replicated)
	bool IsFly = false;
	FVector CurrentDirection;
	UPROPERTY(Replicated)
	float FlyingSpeed;
	//////////////////////////////////////////////

	///////////////////Cody//////////////////
	UPROPERTY(Replicated)
	CodySize CurCodySize = CodySize::NONE;
	UPROPERTY(Replicated)
	CodySize NextCodySize = CodySize::NONE;

	//CodySize
	UPROPERTY(Replicated)
	bool IsBig;
	UPROPERTY(Replicated)
	bool CodyHoldEnemy = false;
	/////////////////////////////////////////
private:
	///////////////////Widget//////////////////
	UCapsuleComponent* CustomPlayerCapsuleComponent;

	///////////////////Rocket//////////////////
	UPROPERTY(Replicated)
	FVector CunstomEndLocation;
	UPROPERTY(Replicated)
	FVector CunstomStartLocation;
	UPROPERTY(Replicated)
	bool JumplocationSet = false;
	UPROPERTY(Replicated)
	float JumpLocationDeltas = 0.0f;
	UPROPERTY(Replicated)
	double CustomTargetLocations;
	UPROPERTY(Replicated)
	double CustomTargetLocationsY;
	UPROPERTY(Replicated)
	FVector ResultTargetLocations;
	bool OverlapHoming = false;

	
	///////////////////Animation//////////////////
	UPROPERTY(Replicated)
	bool CurrentAnimationEnd;
	//////////////////////////////////////////////

	/////////////////Controller///////////////////
	APlayerController* CodyController = nullptr;
	//////////////////////////////////////////////

	///////////////////State/////////////////////
	UPROPERTY(Replicated)
	Cody_State ITTPlayerState;
	//////////////////////////////////////////////

	//////////////////////////////////////////
	//���콺 ���ư��� ���ǵ�
	float RotationInterpSpeed = 2.0f;
	//��ȣ�ۿ�
	UPROPERTY(Replicated)
	bool IsInteract = false;
	//////////////////////////////////////////
};
