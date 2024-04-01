// Fill out your copyright notice in the Description page of Project Settings.


#include "Cody.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Logging/LogMacros.h"

// Sets default values
ACody::ACody()
{
	PrimaryActorTick.bCanEverTick = true;

	// 캐릭터이동
	GetCharacterMovement()->bOrientRotationToMovement = true; 
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//스프링 암
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetUsingAbsoluteRotation(true); 
	SpringArm->TargetArmLength = 1000.f;
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false; 

	//카메라 생성
	CodyCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CodyCamera"));
	CodyCameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	CodyCameraComponent->bUsePawnControlRotation = false;

	PrimaryActorTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void ACody::BeginPlay()
{
	Super::BeginPlay();
	//입력
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
	DashDuration = 1.0f; //Dash 시간
	DefaultGroundFriction = GetCharacterMovement()->GroundFriction; //기본 지면 마찰력
	DefaultGravityScale = GetCharacterMovement()->GravityScale; //기본 중력 스케일

	bIsDashing = false;
	bIsDashingStart = false;
	bCanDash = false;
	PressDashKey = false;
}

// Called every frame
void ACody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsDashing && bCanDash)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime >= DashStartTime + DashDuration)
		{
			DashEnd(); // 대시 지속 시간이 지나면 대시 종료
		}
	}
}

// Called to bind functionality to input
void ACody::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	Input = PlayerInputComponent;
	PlayerInputComponent->BindAction(TEXT("Player_Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Player_Sit"), EInputEvent::IE_Pressed, this, &ACody::Sit);


	UEnhancedInputComponent* CodyInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (CodyInput != nullptr)
	{
		CodyInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACody::Move);
		CodyInput->BindAction(MoveAction, ETriggerEvent::None, this, &ACody::Idle);
		CodyInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACody::Look);
		CodyInput->BindAction(DashAction, ETriggerEvent::Triggered, this, &ACody::DashInput);
		CodyInput->BindAction(DashAction, ETriggerEvent::None, this, &ACody::DashNoneInput);
	}
}


//////////////////////////////캐릭터 이동관련///////////////////////////
//새로운 입력 Action
void ACody::Idle(const FInputActionInstance& _Instance)
{
	IsMoveEnd = false;
	if(bCanDash==false)
		ChangeState(Cody_State::IDLE);
}
void ACody::Move(const FInputActionInstance& _Instance)
{
	UE_LOG(LogTemp, Warning, TEXT("Move function called"));
	IsMoveEnd = true;
	if (bCanDash == false)
	{
		ChangeState(Cody_State::MOVE);

		// 캐릭터의 이동 입력 값을 가져옴
		FVector2D MoveInput = _Instance.GetValue().Get<FVector2D>();
		if (!MoveInput.IsNearlyZero())
		{
			// 입력 방향 노말라이즈
			MoveInput = MoveInput.GetSafeNormal();
			// 입력 방향 벡터
			FVector ForwardVector = FVector(MoveInput.X, MoveInput.Y, 0.0f);

			// 입력 방향으로 캐릭터를 이동시킴
			AddMovementInput(ForwardVector);

			// 입력 방향으로 캐릭터를 회전시킴
			if (!ForwardVector.IsNearlyZero())
			{
				// 입력 방향 벡터를 사용하여 캐릭터를 회전시킴
				FRotator TargetRotation = ForwardVector.Rotation();
				SetActorRotation(TargetRotation);
			}
		}
	}
}


void ACody::Look(const FInputActionInstance& _Instance)
{
	UE_LOG(LogTemp, Warning, TEXT("Look function called"));
	if (Controller != nullptr)
	{
		FVector2D LookVector = _Instance.GetValue().Get<FVector2D>();
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void ACody::DashInput()
{
	if (!bIsDashing && !bCanDash)
	{
		ChangeState(Cody_State::DASH);
		DashStartTime = GetWorld()->GetTimeSeconds();
		if (!GetCharacterMovement()->IsFalling())
		{
			GroundDash();
		}
		else
		{
			JumpDash();
		}
		bIsDashing = true;
		bCanDash = true;
	}
}

void ACody::DashNoneInput()
{
	
}

void ACody::GroundDash()
{
	GetCharacterMovement()->GroundFriction = 0.0f; //마찰력 없앰

	FVector DashDirection = GetActorForwardVector(); // 전방벡터
	DashDirection.Normalize(); // 방향벡터normalize

	FVector DashVelocity = DashDirection * DashDistance; //거리x방향
	GetCharacterMovement()->Velocity = DashVelocity; // 시간에따른 속도설정
}

void ACody::JumpDash()
{
	GetCharacterMovement()->GravityScale = 0.0f; //중력 없앰

	FVector DashDirection = GetActorForwardVector(); // 전방벡터
	DashDirection.Normalize(); // 방향벡터normalize

	FVector DashVelocity = DashDirection * DashDistance * 0.75f; //거리x방향
	GetCharacterMovement()->Velocity = DashVelocity; // 시간에따른 속도설정
}

void ACody::DashEnd()
{
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	bIsDashing = false;
	bIsDashingStart = false;
	bCanDash = false;
}

void ACody::Sit()
{
	ChangeState(Cody_State::SIT);
	if (GetCharacterMovement()->IsFalling())
	{
		//GetCharacterMovement()->GravityScale = 10.0f;
	}
	else
	{
		//GetCharacterMovement()->GravityScale = DefaultGravityScale;
	}
	//작성중
}

//////////////FSM//////////
void ACody::ChangeState(Cody_State _State)
{
	CodyState = _State;
}
