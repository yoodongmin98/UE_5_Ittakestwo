// Fill out your copyright notice in the Description page of Project Settings.


#include "Cody.h"

// Sets default values
ACody::ACody()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACody::BeginPlay()
{
	Super::BeginPlay();

	CodyController = Cast<APlayerController>(Controller);
	if (CodyController != nullptr)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = 
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(CodyController->GetLocalPlayer());
		if (Subsystem != nullptr)
		{
			Subsystem->AddMappingContext(CodyMappingContext, 0);
		}
	}


	//Set
	PlayerHP = 12; //Player기본 Hp설정
	DefaultGroundFriction = GetCharacterMovement()->GroundFriction; //기본 지면 마찰력
	DefaultGravityScale = GetCharacterMovement()->GravityScale;
}

// Called every frame
void ACody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void ACody::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	Input = PlayerInputComponent;
	PlayerInputComponent->BindAction(TEXT("Player_Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);


	UEnhancedInputComponent* CodyInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (CodyInput != nullptr)
	{
		CodyInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACody::Move);
		CodyInput->BindAction(MoveAction, ETriggerEvent::None, this, &ACody::Idle);
		CodyInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACody::Look);
		CodyInput->BindAction(DashAction, ETriggerEvent::Triggered, this, &ACody::DashInput);
	}
}


//////////////////////////////캐릭터 이동관련///////////////////////////
//새로운 입력 Action
void ACody::Idle(const FInputActionInstance& _Instance)
{
	IsMoveEnd = false;
	ChangeState(Cody_State::IDLE);
}
void ACody::Move(const FInputActionInstance& _Instance)
{
	IsMoveEnd = true;
	ChangeState(Cody_State::MOVE);
	FVector2D MoveVector = _Instance.GetValue().Get<FVector2D>();
	if (Controller != nullptr)
	{
		// 이동
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 회전 각도 계산
		float Yaw = FMath::RadiansToDegrees(FMath::Atan2(MoveVector.Y, MoveVector.X));
		const float InterpolatedYaw = FMath::FInterpTo(Rotation.Yaw, Yaw, GetWorld()->GetDeltaSeconds(), RotationInterpSpeed);
		const FRotator InterpolatedRotation(0, InterpolatedYaw, 0);
		SetActorRotation(InterpolatedRotation);

		AddMovementInput(ForwardVector, MoveVector.X);
		AddMovementInput(RightVector, MoveVector.Y);
	}
}

void ACody::Look(const FInputActionInstance& _Instance)
{
	FVector2D LookVector = _Instance.GetValue().Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void ACody::DashInput()
{
	ChangeState(Cody_State::DASH);
	if (!bIsDashing)
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			GroundDash();
		}
		else
		{
			JumpDash();
		}
	}
}

void ACody::GroundDash()
{
	GetCharacterMovement()->GroundFriction = 0.0f; //마찰력 없앰

	FVector DashDirection = GetActorForwardVector(); // 전방벡터
	DashDirection.Normalize(); // 방향벡터normalize

	FVector DashVelocity = DashDirection * DashDistance; //거리x방향
	GetCharacterMovement()->Velocity = DashVelocity; // 시간에따른 속도설정

	bIsDashing = true;
	bIsDashingStart = false;
	// 시간지나면 end호출
	GetWorldTimerManager().SetTimer(DashTimerHandle, this, &ACody::DashEnd, DashDuration, false);
}

void ACody::JumpDash()
{
	GetCharacterMovement()->GravityScale = 0.0f; //중력 없앰

	FVector DashDirection = GetActorForwardVector(); // 전방벡터
	DashDirection.Normalize(); // 방향벡터normalize

	FVector DashVelocity = DashDirection * DashDistance * 0.75f; //거리x방향
	GetCharacterMovement()->Velocity = DashVelocity; // 시간에따른 속도설정

	bIsDashing = true;
	bIsDashingStart = false;
	// 시간지나면 end호출
	GetWorldTimerManager().SetTimer(DashTimerHandle, this, &ACody::DashEnd, DashDuration, false);
}

void ACody::DashEnd()
{
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	bIsDashing = false;
}

//////////////FSM//////////
void ACody::ChangeState(Cody_State _State)
{
	CodyState = _State;
}
