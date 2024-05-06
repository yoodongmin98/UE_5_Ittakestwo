// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Logging/LogMacros.h"
//#include "OnlineSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "PlayerMarkerUI.h"

// Sets default values
APlayerBase::APlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(FName("Player"));

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

	MarkerUIWidget = CreateDefaultSubobject<UPlayerMarkerUI>(TEXT("WidgetComponent"));
	MarkerUIWidget->SetupAttachment(RootComponent);
}

//void APlayerBase::GetOnlineSubsystem()
//{
//
//	// OnlineSubsystem에 Access
//	IOnlineSubsystem* CurOnlineSubsystem = IOnlineSubsystem::Get();
//	if (CurOnlineSubsystem)
//	{
//		// 온라인 세션 받아오기
//		OnlineSeesioninterface = CurOnlineSubsystem->GetSessionInterface();
//
//		if (GEngine)
//		{
//			// OnlineSubsystem 이름 출력하기
//			//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Found subsystem %s"), *CurOnlineSubsystem->GetSubsystemName().ToString()));
//		}
//	}
//}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	//GetOnlineSubsystem();
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
	ITTPlayerState = Cody_State::IDLE;
	//Set
	PlayerHP = FullHP; //Player기본 Hp설정
	DashDuration = 0.7f; //Dash 시간
	DefaultGroundFriction = GetCharacterMovement()->GroundFriction; //기본 지면 마찰력
	DefaultGravityScale = GetCharacterMovement()->GravityScale; //기본 중력 스케일
	PlayerDefaultSpeed = GetCharacterMovement()->MaxWalkSpeed; //기본 이동속도


	IsMoveEnd = false;

	bIsDashing = false;
	bIsDashingStart = false;
	bCanDash = false;
	BigCanDash = true;

	CurrentAnimationEnd = false;
	IsSprint = false;

	CanSit = true;
	SitDuration = 0.5f;
	ChangeIdle = true;
	TestRotator = FRotator::ZeroRotator;


	CustomPlayerJumpCount = ACharacter::JumpMaxCount;


	FlyingSpeed = 600.0f;
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bOnGravityPath==true)
	{
		TestRotator = GravityRotator;
	}
	/*SetActorRotation(TestRotator);*/
	//점프 횟수 확인
	CharacterJumpCount = JumpCurrentCount;
	//중력상태확인(Sit)
	if (GetCharacterMovement()->GravityScale <=5.5f)
	{
		IsDGravity = true;
	}
	else
	{
		IsDGravity = false;
	}
	//플레이어 생존여부 확인
	PlayerDeathCheck();
	//대쉬의 지속시간을 Tick에서 지속적으로 확인
	if (bIsDashing && bCanDash)
	{
		DashCurrentTime = GetWorld()->GetTimeSeconds();
		if (DashCurrentTime >= DashStartTime + DashDuration)
		{
			// 대시 지속 시간이 지나면 대시 종료
			DashEnd();
		}
	}
	if (!CanSit)
	{
		CurrentSitTime = GetWorld()->GetTimeSeconds();
		if (CurrentSitTime >= SitStartTime + SitDuration)
		{
			GetCharacterMovement()->GravityScale = 10.0f;
			if (!GetCharacterMovement()->IsFalling())
			{
				SitEnd();
			}
		}
	}


	//FlyTest
	if (IsFly)
	{
		FVector ForwardVector = GetActorForwardVector();
		AddActorWorldOffset(ForwardVector * FlyingSpeed * DeltaTime);
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
		PlayerInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerBase::CustomMove);
		PlayerInput->BindAction(MoveAction, ETriggerEvent::None, this, &APlayerBase::Idle);

		PlayerInput->BindAction(FlyMoveAction, ETriggerEvent::Triggered, this, &APlayerBase::CustomFlyMove);

		PlayerInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);
		PlayerInput->BindAction(DashAction, ETriggerEvent::Triggered, this, &APlayerBase::DashInput);
		PlayerInput->BindAction(DashAction, ETriggerEvent::None, this, &APlayerBase::DashNoneInput);
		PlayerInput->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerBase::InteractInput);
		PlayerInput->BindAction(InteractAction, ETriggerEvent::None, this, &APlayerBase::InteractNoneInput);
		PlayerInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerBase::SprintInput);
		PlayerInput->BindAction(SprintAction, ETriggerEvent::None, this, &APlayerBase::SprintNoneInput);

		PlayerInput->BindAction(TestAction, ETriggerEvent::Triggered, this, &APlayerBase::TestFunction);
	}
}


//////////////////////////////캐릭터 이동관련///////////////////////////
//새로운 입력 Action
void APlayerBase::Idle(const FInputActionInstance& _Instance)
{
	if (HasAuthority() == true)
	{
		CustomClientIdle();
	}
	else
	{
		CustomServerIdle();
	}
}

void APlayerBase::CustomClientIdle_Implementation()
{
	IsSprint = false;
	IsMoveEnd = false;
	if (bCanDash == false)
		ChangeState(Cody_State::IDLE);
}

bool APlayerBase::CustomServerIdle_Validate()
{
	return true;
}
void APlayerBase::CustomServerIdle_Implementation()
{
	OnRep_IsMoveEnd();
}

void APlayerBase::OnRep_IsMoveEnd()
{
	IsSprint = false;
	IsMoveEnd = false;
	if (bCanDash == false)
		ChangeState(Cody_State::IDLE);
}
void APlayerBase::CustomMove(const FInputActionInstance& _Instance)
{
	if (IsFly == false)
	{
		if (bCanDash == false && ChangeIdle)
		{
			ChangeState(Cody_State::MOVE);
			// 컨트롤러의 회전 방향을 가져옴
			ControllerRotation = Controller->GetControlRotation();
			CustomTargetRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);
			// Move를 시작할 때 카메라의 위치를 반영하여 SetRotation함(그 전까지는 자유시점)
			// 컨트롤러의 회전 방향에서 Yaw 각도만 사용하여 캐릭터를 회전시킴

			// 컨트롤러의 회전 방향을 기준으로 월드 전방 벡터를 계산
			WorldForwardVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Y);
			// 컨트롤러의 회전 방향을 기준으로 월드 오른쪽 벡터를 계산
			WorldRightVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::X);

			// 캐릭터를 입력 방향으로 이동시킴
			MoveInput = _Instance.GetValue().Get<FVector2D>();
			if (!MoveInput.IsNearlyZero())
			{
				// 입력 방향 노말라이즈
				MoveInput = MoveInput.GetSafeNormal();

				// MoveDirection기준으로 Yaw부분만 Player Rotation에 적용
				MoveDirection = WorldForwardVector * MoveInput.Y + WorldRightVector * MoveInput.X;
				FRotator CodyRotation(0.0f, MoveDirection.Rotation().Yaw, 0.0f);
				CustomTargetRotation = CodyRotation;

				// 입력 방향에 따라 캐릭터를 이동시킴
				AddMovementInput(MoveDirection);
			}
		}
		if (HasAuthority() == true)
		{
			ChangeClientDir(_Instance, CustomTargetRotation);
		}
		else
		{
			ChangeServerDir(_Instance, CustomTargetRotation);
		}
	}
}

void APlayerBase::CustomFlyMove(const FInputActionInstance& _Instance)
{
	if (HasAuthority() == true)
	{
		CustomClientFly(_Instance);
	}
	else
	{
		CustomServerFly(_Instance);
	}
}

void APlayerBase::CustomClientFly_Implementation(const FInputActionInstance& _Instance)
{
	if (IsFly)
	{
		ChangeState(Cody_State::FLYING);
		GetCharacterMovement()->MaxFlySpeed = 1500.0f;
		if (bCanDash == false && ChangeIdle)
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			// 컨트롤러의 회전 방향을 가져옴
			ControllerRotation = Controller->GetControlRotation();
			CustomTargetRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);
			// Move를 시작할 때 카메라의 위치를 반영하여 SetRotation함(그 전까지는 자유시점)
			// 컨트롤러의 회전 방향에서 Yaw 각도만 사용하여 캐릭터를 회전시킴

			// 컨트롤러의 회전 방향을 기준으로 월드 전방 벡터를 계산
			WorldForwardVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Y);
			WorldRightVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Z);

			// 캐릭터를 입력 방향으로 이동시킴
			MoveInput = _Instance.GetValue().Get<FVector2D>();
			if (!MoveInput.IsNearlyZero())
			{
				// 입력 방향 노말라이즈
				MoveInput = MoveInput.GetSafeNormal();

				// MoveDirection기준으로 Yaw부분만 Player Rotation에 적용
				MoveDirection = WorldForwardVector * MoveInput.Y + WorldRightVector * MoveInput.X;
				// 컨트롤러의 회전 방향을 기준으로 월드 오른쪽 벡터를 계산.X;

					// 입력 방향에 따라 캐릭터를 이동시킴
				if (MoveInput.X >= 0)
					AddMovementInput(FVector(MoveDirection.X, MoveDirection.Y, MoveDirection.Z * 0.5f));
				else
					AddMovementInput(FVector(-MoveDirection.X, -MoveDirection.Y, MoveDirection.Z * 0.5f));
				///////////////////////////////////////////////////////////////////////////////////////////
			}
			if (HasAuthority() == true)
			{
				ChangeClientDir(_Instance, CustomTargetRotation);
			}
			else
			{
				ChangeServerDir(_Instance, CustomTargetRotation);
			}
		}
	}
}

bool APlayerBase::CustomServerFly_Validate(const FInputActionInstance& _Instance)
{
	return true;
}

void APlayerBase::CustomServerFly_Implementation(const FInputActionInstance& _Instance)
{
	if (IsFly)
	{
		ChangeState(Cody_State::FLYING);
		GetCharacterMovement()->MaxFlySpeed = 1500.0f;
		if (bCanDash == false && ChangeIdle)
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			// 컨트롤러의 회전 방향을 가져옴
			ControllerRotation = Controller->GetControlRotation();
			CustomTargetRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);
			// Move를 시작할 때 카메라의 위치를 반영하여 SetRotation함(그 전까지는 자유시점)
			// 컨트롤러의 회전 방향에서 Yaw 각도만 사용하여 캐릭터를 회전시킴

			// 컨트롤러의 회전 방향을 기준으로 월드 전방 벡터를 계산
			WorldForwardVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Y);
			WorldRightVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Z);

			// 캐릭터를 입력 방향으로 이동시킴
			MoveInput = _Instance.GetValue().Get<FVector2D>();
			if (!MoveInput.IsNearlyZero())
			{
				// 입력 방향 노말라이즈
				MoveInput = MoveInput.GetSafeNormal();

				// MoveDirection기준으로 Yaw부분만 Player Rotation에 적용
				MoveDirection = WorldForwardVector * MoveInput.Y + WorldRightVector * MoveInput.X;
				// 컨트롤러의 회전 방향을 기준으로 월드 오른쪽 벡터를 계산.X;

					// 입력 방향에 따라 캐릭터를 이동시킴
				if (MoveInput.X >= 0)
					AddMovementInput(FVector(MoveDirection.X, MoveDirection.Y, MoveDirection.Z * 0.5f));
				else
					AddMovementInput(FVector(-MoveDirection.X, -MoveDirection.Y, MoveDirection.Z * 0.5f));
				///////////////////////////////////////////////////////////////////////////////////////////
			}
			if (HasAuthority() == true)
			{
				ChangeClientDir(_Instance, CustomTargetRotation);
			}
			else
			{
				ChangeServerDir(_Instance, CustomTargetRotation);
			}
		}
	}
}

void APlayerBase::ChangeClientDir_Implementation(const FInputActionInstance& _Instance, FRotator _Rotator)
{
	IsMoveEnd = true;
	SetActorRotation(_Rotator);
}

bool APlayerBase::ChangeServerDir_Validate(const FInputActionInstance& _Instance, FRotator _Rotator)
{
	return true;
}

void APlayerBase::ChangeServerDir_Implementation(const FInputActionInstance& _Instance, FRotator _Rotator)
{
	IsMoveEnd = true;
	SetActorRotation(_Rotator);
}




void APlayerBase::Look(const FInputActionInstance& _Instance)
{
	//UE_LOG(LogTemp, Warning, TEXT("Look function called"));
	if (Controller != nullptr)
	{
		const TArray<FName>& CheckTag = Tags;
		for (const FName& V : CheckTag)
		{
			if (V == FName("Cody") || V == FName("May"))
			{
				// 1. 이쉐끼가 바라보는 방향이 플레이어의 전방벡터가 되어야함 →ㅇ
				// 2. Move가 입력중일땐 미적용-> Move가 끝난 시점에 적용되어야함(idle)
				// 3. Move중에도 벡터는 유지되고, 보는 방향은 달라져야함 →ㅇ
				// 4. 보는 방향 -> 피치,요 둘다 적용 →ㅇ
				// 5. 방향벡터 -> 요만 적용 →ㅇ

				CameraLookVector = _Instance.GetValue().Get<FVector2D>();

				if (IsFly)
				{
					AddControllerYawInput(CameraLookVector.X * 0.2f);
				}
				else
				{
					AddControllerYawInput(CameraLookVector.X);
					AddControllerPitchInput(-CameraLookVector.Y);
				}
				

				//// 카메라의 피치 각도 제한
				//// 90도 넘어가면 스프링암 타겟길이에 영향을 미쳐야함.
				//float CurrentPitch = GetControlRotation().Pitch;
				//float NewPitch = FMath::ClampAngle(CurrentPitch + CameraLookVector.Y, 360.0f, 0.0f); // -90도부터 0도 사이로 제한
				//FRotator NewRotation = FRotator(NewPitch, GetControlRotation().Yaw, GetControlRotation().Roll);
				//Controller->SetControlRotation(NewRotation);
			}
		}
	}
}

void APlayerBase::DashInput()
{
	IsSprint = false;
	if (HasAuthority() == true)
	{
		CustomClientDash();
	}
	else
	{
		CustomServerDash();
	}
}


void APlayerBase::CustomClientDash_Implementation()
{
	if (!bIsDashing && !bCanDash && BigCanDash && ChangeIdle)
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
			DashDuration = 0.2f;
			JumpDash();
		}
		bIsDashing = true;
		bCanDash = true;
	}
}
bool APlayerBase::CustomServerDash_Validate()
{
	return true;
}
void APlayerBase::CustomServerDash_Implementation()
{
	if (!bIsDashing && !bCanDash && BigCanDash && ChangeIdle)
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
			DashDuration = 0.2f;
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
	DashDirection = GetActorForwardVector();
	// 벡터Normalize
	DashDirection.Normalize();
	// 거리 x 방향 계산
	DashVelocity = DashDirection * DashDistance;
	// 시간에따른 속도설정
	GetCharacterMovement()->Velocity = DashVelocity;
}

void APlayerBase::JumpDash()
{
	// 중력 없앰
	GetCharacterMovement()->GravityScale = 0.0f;
	// Cody의 전방벡터
	DashDirection = GetActorForwardVector();
	// 방향벡터normalize
	DashDirection.Normalize();
	// 거리 x 방향 계산
	DashVelocity = DashDirection * DashDistance * 0.7f;
	// 시간에따른 속도설정
	GetCharacterMovement()->Velocity = DashVelocity;
}



void APlayerBase::Sit()
{
	SitStartTime = GetWorld()->GetTimeSeconds();
	if (GetCharacterMovement()->IsFalling() && CanSit && !bIsDashing)
	{
		ChangeState(Cody_State::SIT);
		IsSit = true;		
		ChangeIdle = false;
		//일단 중력없애
		GetCharacterMovement()->GravityScale = 0.0f;
		//일단 멈춰
		GetCharacterMovement()->Velocity = FVector::ZeroVector;

		CanSit = false;
	}
}


void APlayerBase::SitEnd()
{
	IsSit = false;
	CanSit = true;
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
}
void APlayerBase::InteractInput_Implementation()
{
	IsInteract = true;
}
void APlayerBase::InteractNoneInput_Implementation()
{
	IsInteract = false;
	if (IsPlayerDeath)
	{
		PlayerHP += 1;
	}
}

//////////////FSM//////////
void APlayerBase::ChangeState(Cody_State _State)
{
	ITTPlayerState = _State;
}

void APlayerBase::PlayerDeathCheck()
{
	ChangeState(Cody_State::PlayerDeath);
	//플레이어hp가 0보다 작거나 death함수가 호출되었을때 실행됩니다.
	if (0 <= PlayerHP || IsPlayerDeath == true)
	{
		IsPlayerDeath = true;
	}
	if (FullHP == PlayerHP)
	{
		IsPlayerDeath = false;
		ChangeState(Cody_State::IDLE);
	}
}

void APlayerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerBase, ITTPlayerState);
	DOREPLIFETIME(APlayerBase, IsMoveEnd);
	DOREPLIFETIME(APlayerBase, CurrentAnimationEnd);
	DOREPLIFETIME(APlayerBase, bCanDash);
	DOREPLIFETIME(APlayerBase, bIsDashingStart);
	DOREPLIFETIME(APlayerBase, IsSit);
	DOREPLIFETIME(APlayerBase, CanSit);
	DOREPLIFETIME(APlayerBase, CharacterJumpCount);
	DOREPLIFETIME(APlayerBase, IsBig);
	DOREPLIFETIME(APlayerBase, ChangeIdle);
	DOREPLIFETIME(APlayerBase, TestRotator);
	DOREPLIFETIME(APlayerBase, MoveDirection);
	DOREPLIFETIME(APlayerBase, MoveInput);
	DOREPLIFETIME(APlayerBase, ControllerRotation);
	DOREPLIFETIME(APlayerBase, CustomTargetRotation);
	DOREPLIFETIME(APlayerBase, WorldForwardVector);
	DOREPLIFETIME(APlayerBase, WorldRightVector);
	DOREPLIFETIME(APlayerBase, DashDirection)
	DOREPLIFETIME(APlayerBase, DashDistance);
	DOREPLIFETIME(APlayerBase, DashVelocity);
	DOREPLIFETIME(APlayerBase, bIsDashing);
	DOREPLIFETIME(APlayerBase, bIsDashingStart);
	DOREPLIFETIME(APlayerBase, bCanDash);
	DOREPLIFETIME(APlayerBase, DashDuration);
	DOREPLIFETIME(APlayerBase, DashStartTime);
	DOREPLIFETIME(APlayerBase, DashCurrentTime);
	DOREPLIFETIME(APlayerBase, CustomPlayerJumpCount);
	DOREPLIFETIME(APlayerBase, IsInteract);
	DOREPLIFETIME(APlayerBase, IsFly);
}






void APlayerBase::TestFunction()
{
	ChangeState(Cody_State::FLYING);
	IsFly = !IsFly;
}

