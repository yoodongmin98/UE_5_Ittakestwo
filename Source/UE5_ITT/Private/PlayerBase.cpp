// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Logging/LogMacros.h"

// Sets default values
APlayerBase::APlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	BigLength = 2000.0f;
	NormalLength = 1200.0f;
	SmallLength = 200.0f;

	// 캐릭터이동
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//스프링 암
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->TargetArmLength = NormalLength;
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false;

	//카메라 생성
	PlayerCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	PlayerCameraComponent->bUsePawnControlRotation = false;

	PrimaryActorTick.bStartWithTickEnabled = true;

}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
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
	BigCanDash = true;
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//대쉬의 지속시간을 Tick에서 지속적으로 확인
	if (bIsDashing && bCanDash)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime >= DashStartTime + DashDuration)
		{
			// 대시 지속 시간이 지나면 대시 종료
			DashEnd();
		}
	}
}

// Called to bind functionality to input
void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	Input = PlayerInputComponent;
	PlayerInputComponent->BindAction(TEXT("Player_Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Player_Sit"), EInputEvent::IE_Pressed, this, &APlayerBase::Sit);


	PlayerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput != nullptr)
	{
		PlayerInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerBase::Move);
		PlayerInput->BindAction(MoveAction, ETriggerEvent::None, this, &APlayerBase::Idle);
		PlayerInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);
		PlayerInput->BindAction(DashAction, ETriggerEvent::Triggered, this, &APlayerBase::DashInput);
		PlayerInput->BindAction(DashAction, ETriggerEvent::None, this, &APlayerBase::DashNoneInput);
	}
}


//////////////////////////////캐릭터 이동관련///////////////////////////
//새로운 입력 Action
void APlayerBase::Idle(const FInputActionInstance& _Instance)
{
	IsMoveEnd = false;
	if (bCanDash == false)
		ChangeState(Cody_State::IDLE);
}

void APlayerBase::Move(const FInputActionInstance& _Instance)
{
	//UE_LOG(LogTemp, Warning, TEXT("Move function called"));
	IsMoveEnd = true;

	if (bCanDash == false)
	{
		ChangeState(Cody_State::MOVE);
		// Move를 시작할 때 카메라의 위치를 반영하여 SetRotation함(그 전까지는 자유시점)
		// 
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

			// MoveDirection기준으로 Yaw부분만 Player Rotation에 적용
			FVector MoveDirection = WorldForwardVector * MoveInput.Y + WorldRightVector * MoveInput.X;
			FRotator CodyRotation(0.0f, MoveDirection.Rotation().Yaw, 0.0f);
			SetActorRotation(CodyRotation);


			// 입력 방향을 캐릭터의 로컬 XY 평면에 정사영하여 캐릭터의 이동구현
			MoveDirection = FVector::VectorPlaneProject(MoveDirection, FVector::UpVector);
			MoveDirection.Normalize();

			// 입력 방향에 따라 캐릭터를 이동시킴
			AddMovementInput(MoveDirection);
		}
	}
}


void APlayerBase::Look(const FInputActionInstance& _Instance)
{
	//UE_LOG(LogTemp, Warning, TEXT("Look function called"));
	if (Controller != nullptr)
	{
		// 1. 이쉐끼가 바라보는 방향이 플레이어의 전방벡터가 되어야함 →ㅇ
		// 2. Move가 입력중일땐 미적용-> Move가 끝난 시점에 적용되어야함(idle)
		// 3. Move중에도 벡터는 유지되고, 보는 방향은 달라져야함 →ㅇ
		// 4. 보는 방향 -> 피치,요 둘다 적용 →ㅇ
		// 5. 방향벡터 -> 요만 적용 →ㅇ

		CameraLookVector = _Instance.GetValue().Get<FVector2D>();

		AddControllerYawInput(CameraLookVector.X);

		// 카메라의 피치 각도 제한
		// 90도 넘어가면 스프링암 타겟길이에 영향을 미쳐야함.
		float CurrentPitch = GetControlRotation().Pitch;
		float NewPitch = FMath::ClampAngle(CurrentPitch + CameraLookVector.Y, -90.0f, 0.0f); // -90도부터 0도 사이로 제한
		FRotator NewRotation = FRotator(NewPitch, GetControlRotation().Yaw, GetControlRotation().Roll);
		Controller->SetControlRotation(NewRotation);
	}
}

void APlayerBase::DashInput()
{
	if (!bIsDashing && !bCanDash && BigCanDash)
	{
		ChangeState(Cody_State::DASH);
		//대쉬 시작시간을 체크
		DashStartTime = GetWorld()->GetTimeSeconds();
		//지면에 닿아있는지를 체크하여 실행할 함수 변경
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

void APlayerBase::DashNoneInput()
{

}

void APlayerBase::GroundDash()
{
	// 마찰력 없앰
	GetCharacterMovement()->GroundFriction = 0.0f;
	// Cody의 전방벡터
	FVector DashDirection = GetActorForwardVector();
	// 벡터Normalize
	DashDirection.Normalize();
	// 거리 x 방향 계산
	FVector DashVelocity = DashDirection * DashDistance;
	// 시간에따른 속도설정
	GetCharacterMovement()->Velocity = DashVelocity;
}

void APlayerBase::JumpDash()
{
	// 중력 없앰
	GetCharacterMovement()->GravityScale = 0.0f;
	// Cody의 전방벡터
	FVector DashDirection = GetActorForwardVector();
	// 방향벡터normalize
	DashDirection.Normalize();
	// 거리 x 방향 계산
	FVector DashVelocity = DashDirection * DashDistance * 0.75f;
	// 시간에따른 속도설정
	GetCharacterMovement()->Velocity = DashVelocity;
}

void APlayerBase::DashEnd()
{
	//대쉬가 끝나면 기본으로 적용된 중력,지면 마찰력으로 다시 적용
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	bIsDashing = false;
	bIsDashingStart = false;
	bCanDash = false;
}

void APlayerBase::Sit()
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
void APlayerBase::ChangeState(Cody_State _State)
{
	ITTPlayerState = _State;
}
