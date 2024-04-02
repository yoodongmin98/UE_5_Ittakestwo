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
		// Move를 시작할 때 카메라의 위치를 반영하여 SetRotation함(그 전까지는 자유시점)
		// 컨트롤러의 회전 방향을 가져옴
		FRotator ControllerRotation = Controller->GetControlRotation();
		// 컨트롤러의 회전 방향에서 Yaw 각도만 사용하여 캐릭터를 회전시킴
		FRotator TargetRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);
		SetActorRotation(TargetRotation);


		// 컨트롤러의 회전 방향을 기준으로 월드 전방 벡터를 계산
		FVector WorldForwardVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Y);
		// 컨트롤러의 회전 방향을 기준으로 월드 오른쪽 벡터를 계산
		FVector WorldRightVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::X);
	 
		// 캐릭터를 입력 방향으로 이동시킴
		FVector2D MoveInput = _Instance.GetValue().Get<FVector2D>();
		if (!MoveInput.IsNearlyZero())
		{
			// 입력 방향 노말라이즈
			MoveInput = MoveInput.GetSafeNormal();


			/////////////////////////////////////////////////////////
			//회전 구현해야함
			/////////////////////////////////////////////////////////
			 
			 
			// 컨트롤러의 회전 방향을 기준으로 계산된 월드 전방 벡터와 월드 오른쪽 벡터를 사용하여 이동 방향을 설정
			FVector MoveDirection = WorldForwardVector * MoveInput.Y + WorldRightVector * MoveInput.X;


			// 입력 방향을 캐릭터의 로컬 XY 평면에 정사영하여 캐릭터의 앞뒤좌우 이동을 구현
			MoveDirection = FVector::VectorPlaneProject(MoveDirection, FVector::UpVector);
			MoveDirection.Normalize();
			// 입력 방향에 따라 캐릭터를 이동시킴
			
			AddMovementInput(MoveDirection);
		}
	}
}


void ACody::Look(const FInputActionInstance& _Instance)
{
	UE_LOG(LogTemp, Warning, TEXT("Look function called"));
	if (Controller != nullptr)
	{
		// 1. 이쉐끼가 바라보는 방향이 플레이어의 전방벡터가 되어야함
		// 2. Move가 입력중일땐 미적용-> Move가 끝난 시점에 적용되어야함(idle)
		// 3. Move중에도 벡터는 유지되고, 보는 방향은 달라져야함
		// 4. 보는 방향 -> 피치,요 둘다 적용
		// 5. 방향벡터 -> 요만 적용

		CameraLookVector = _Instance.GetValue().Get<FVector2D>();

		AddControllerYawInput(CameraLookVector.X);
		
		// 카메라의 피치 각도 제한
		float CurrentPitch = GetControlRotation().Pitch;
		float NewPitch = FMath::ClampAngle(CurrentPitch + CameraLookVector.Y, -90.0f, 0.0f); // -90도부터 0도 사이로 제한
		FRotator NewRotation = FRotator(NewPitch, GetControlRotation().Yaw, GetControlRotation().Roll);
		Controller->SetControlRotation(NewRotation);
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
