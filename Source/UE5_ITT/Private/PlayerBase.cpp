// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"
#include "RespawnTrigger.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "HomingRocket.h"
#include "GameFramework/SpringArmComponent.h"
#include "Logging/LogMacros.h"
//#include "OnlineSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "MarkerWidget.h"

// Sets default values
APlayerBase::APlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(FName("Player"));

	BigLength = 2000.0f;
	NormalLength = 1200.0f;
	SmallLength = 200.0f;

	// ĳ�����̵�
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//������ ��
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmDefaultFunction();

	//ī�޶� ����
	PlayerCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	PlayerCameraComponent->bUsePawnControlRotation = false;

	PrimaryActorTick.bStartWithTickEnabled = true;

	MarkerUIWidget = CreateDefaultSubobject<UMarkerWidget>(TEXT("MarkerWidgetCom"));
	MarkerUIWidget->SetupAttachment(RootComponent);
}

void APlayerBase::SettingMarkerWidget()
{
	//MarkerUIWidget->SettingCustomVisible();
}


//void APlayerBase::GetOnlineSubsystem()
//{
//
//	// OnlineSubsystem�� Access
//	IOnlineSubsystem* CurOnlineSubsystem = IOnlineSubsystem::Get();
//	if (CurOnlineSubsystem)
//	{
//		// �¶��� ���� �޾ƿ���
//		OnlineSeesioninterface = CurOnlineSubsystem->GetSessionInterface();
//
//		if (GEngine)
//		{
//			// OnlineSubsystem �̸� ����ϱ�
//			//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Found subsystem %s"), *CurOnlineSubsystem->GetSubsystemName().ToString()));
//		}
//	}
//}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	CustomPlayerCapsuleComponent = GetCapsuleComponent();
	CustomPlayerCapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &APlayerBase::OnOverlapBegin);
	CustomPlayerCapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &APlayerBase::OnOverlapEnd);
	

	//GetOnlineSubsystem();
	//�Է�
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
	PlayerHP = FullHP; //Player�⺻ Hp����
	DashDuration = 0.7f; //Dash �ð�
	DefaultGroundFriction = GetCharacterMovement()->GroundFriction; //�⺻ ���� ������
	DefaultGravityScale = GetCharacterMovement()->GravityScale; //�⺻ �߷� ������
	PlayerDefaultSpeed = GetCharacterMovement()->MaxWalkSpeed; //�⺻ �̵��ӵ�

	NowPlayerGravityScale = DefaultGravityScale;
	PlayerJumpZVelocity = GetCharacterMovement()->JumpZVelocity;
	PlayerDefaultJumpHeight = GetCharacterMovement()->JumpZVelocity;

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
	CustomPlayerJumpCount = ACharacter::JumpMaxCount;
	FlyingSpeed = 600.0f;
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	////////////////����ȭ�� ���� Tick Check/////////////////////
	 
	//���� Ƚ�� Ȯ��
	CharacterJumpCount = JumpCurrentCount;
	//�߷»���Ȯ��
	GetCharacterMovement()->GravityScale = NowPlayerGravityScale;
	//�̵��ӵ� üũ
	GetCharacterMovement()->MaxWalkSpeed = NowPlayerSpeed;
	GetCharacterMovement()->JumpZVelocity = PlayerJumpZVelocity;
	//�÷��̾� �������� Ȯ��
	PlayerDeathCheck();

	///////////////////////////////////////////////////////////


	if (GetCharacterMovement()->GravityScale <=5.5f)
	{
		IsDGravity = true;
	}
	else
	{
		IsDGravity = false;
	}
	//�뽬�� ���ӽð��� Tick���� ���������� Ȯ��
	if (bIsDashing && bCanDash)
	{
		DashCurrentTime = GetWorld()->GetTimeSeconds();
		if (DashCurrentTime >= DashStartTime + DashDuration)
		{
			// ��� ���� �ð��� ������ ��� ����
			DashEnd();
		}
	}

	if (JumplocationSet)
	{
		JumpLocationDeltas += GetWorld()->DeltaTimeSeconds;
		CustomTargetLocations = FMath::Lerp(CunstomStartLocation.X, CunstomEndLocation.X, JumpLocationDeltas);
		CustomTargetLocationsY = FMath::Lerp(CunstomStartLocation.Y, CunstomEndLocation.Y, JumpLocationDeltas);
		ResultTargetLocations = FVector(CustomTargetLocations, CustomTargetLocationsY, GetActorLocation().Z);
		SetActorLocation(ResultTargetLocations);
		if (JumpLocationDeltas >= 1.0f)
		{
			JumpLocationDeltas = 1.0f;
		}
	}
	UpdateCamTrans();
}

// Called to bind functionality to input
void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	Input = PlayerInputComponent;
	PlayerInputComponent->BindAction(TEXT("Player_Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	


	PlayerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput != nullptr)
	{
		PlayerInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerBase::CustomMove);
		PlayerInput->BindAction(MoveAction, ETriggerEvent::None, this, &APlayerBase::Idle);

		PlayerInput->BindAction(FlyMoveAction, ETriggerEvent::Triggered, this, &APlayerBase::CustomFlyMove);
		PlayerInput->BindAction(FlyMoveAction, ETriggerEvent::None, this, &APlayerBase::CustomFlyNoneMove);

		PlayerInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);
		PlayerInput->BindAction(DashAction, ETriggerEvent::Triggered, this, &APlayerBase::DashInput);
		PlayerInput->BindAction(DashAction, ETriggerEvent::None, this, &APlayerBase::DashNoneInput);
		PlayerInput->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerBase::InteractInput);
		PlayerInput->BindAction(InteractAction, ETriggerEvent::None, this, &APlayerBase::InteractNoneInput);
		PlayerInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerBase::SprintInput);
		PlayerInput->BindAction(SprintAction, ETriggerEvent::None, this, &APlayerBase::SprintNoneInput);

		
	}
}


//////////////////////////////ĳ���� �̵�����///////////////////////////
//���ο� �Է� Action
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
	if (bCanDash == false && !IsPlayerDeath && !IsFly)
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
	if (bCanDash == false && !IsPlayerDeath)
		ChangeState(Cody_State::IDLE);
}
void APlayerBase::CustomMove(const FInputActionInstance& _Instance)
{
	if (!IsFly && !CodyHoldEnemy)
	{
		if (bCanDash == false && ChangeIdle)
		{
			ChangeState(Cody_State::MOVE);
			// ��Ʈ�ѷ��� ȸ�� ������ ������
			ControllerRotation = Controller->GetControlRotation();
			CustomTargetRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);
			// Move�� ������ �� ī�޶��� ��ġ�� �ݿ��Ͽ� SetRotation��(�� �������� ��������)
			// ��Ʈ�ѷ��� ȸ�� ���⿡�� Yaw ������ ����Ͽ� ĳ���͸� ȸ����Ŵ

			// ��Ʈ�ѷ��� ȸ�� ������ �������� ���� ���� ���͸� ���
			WorldForwardVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Y);
			// ��Ʈ�ѷ��� ȸ�� ������ �������� ���� ������ ���͸� ���
			WorldRightVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::X);

			// ĳ���͸� �Է� �������� �̵���Ŵ
			MoveInput = _Instance.GetValue().Get<FVector2D>();
			if (!MoveInput.IsNearlyZero())
			{
				// �Է� ���� �븻������
				MoveInput = MoveInput.GetSafeNormal();

				// MoveDirection�������� Yaw�κи� Player Rotation�� ����
				MoveDirection = WorldForwardVector * MoveInput.Y + WorldRightVector * MoveInput.X;
				FRotator CodyRotation(0.0f, MoveDirection.Rotation().Yaw, 0.0f);
				CustomTargetRotation = CodyRotation;

				// �Է� ���⿡ ���� ĳ���͸� �̵���Ŵ
				AddMovementInput(MoveDirection);
			}
		}
		if (HasAuthority())
		{
			ChangeClientDir(CustomTargetRotation);
		}
		else
		{
			ChangeServerDir(CustomTargetRotation);
		}
	}
}

void APlayerBase::CustomFlyMove(const FInputActionInstance& _Instance)
{
	if (IsFly && !CodyHoldEnemy)
	{
		ChangeState(Cody_State::FLYING);
		if (bCanDash == false && ChangeIdle)
		{
			// ��Ʈ�ѷ��� ȸ�� ������ ������
			ControllerRotation = Controller->GetControlRotation();
			CustomTargetRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);
			// Move�� ������ �� ī�޶��� ��ġ�� �ݿ��Ͽ� SetRotation��(�� �������� ��������)
			// ��Ʈ�ѷ��� ȸ�� ���⿡�� Yaw ������ ����Ͽ� ĳ���͸� ȸ����Ŵ

			// ��Ʈ�ѷ��� ȸ�� ������ �������� ���� ���� ���͸� ���
			WorldForwardVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Y);
			WorldRightVector = FRotationMatrix(ControllerRotation).GetScaledAxis(EAxis::Z);

			// ĳ���͸� �Է� �������� �̵���Ŵ
			MoveInput = _Instance.GetValue().Get<FVector2D>();
			if (!MoveInput.IsNearlyZero())
			{
				// �Է� ���� �븻������
				MoveInput = MoveInput.GetSafeNormal();

				// MoveDirection�������� Yaw�κи� Player Rotation�� ����
				MoveDirection = WorldForwardVector * MoveInput.Y + WorldRightVector * MoveInput.X;
				// ��Ʈ�ѷ��� ȸ�� ������ �������� ���� ������ ���͸� ���.X;

					// �Է� ���⿡ ���� ĳ���͸� �̵���Ŵ
				if (MoveInput.X >= 0)
					AddMovementInput(FVector(MoveDirection.X, MoveDirection.Y, MoveDirection.Z * 0.5f));
				else
					AddMovementInput(FVector(-MoveDirection.X, -MoveDirection.Y, MoveDirection.Z * 0.5f));
				///////////////////////////////////////////////////////////////////////////////////////////
			}
			if (HasAuthority() == true)
			{
				ChangeClientFlyDir(CustomTargetRotation);
			}
			else
			{
				ChangeServerFlyDir(CustomTargetRotation);
			}
		}
	}
}

void APlayerBase::CustomFlyNoneMove(const FInputActionInstance& _Instance)
{
	
}

void APlayerBase::ChangeClientDir_Implementation(FRotator _Rotator)
{
	IsMoveEnd = true;
	SetActorRotation(_Rotator);
}

bool APlayerBase::ChangeServerDir_Validate(FRotator _Rotator)
{
	return true;
}

void APlayerBase::ChangeServerDir_Implementation(FRotator _Rotator)
{
	IsMoveEnd = true;
	SetActorRotation(_Rotator);
}


void APlayerBase::ChangeClientFlyDir_Implementation(FRotator _Rotator)
{
	SetActorRotation(_Rotator);
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	FlyForwardVector = GetActorForwardVector();
	GetCharacterMovement()->MaxFlySpeed = 1500.0f;
	AddActorWorldOffset(FlyForwardVector * FlyingSpeed * (GetWorld()->DeltaRealTimeSeconds));
}
bool APlayerBase::ChangeServerFlyDir_Validate(FRotator _Rotator)
{

	return true;
}
void APlayerBase::ChangeServerFlyDir_Implementation(FRotator _Rotator)
{
	SetActorRotation(_Rotator);
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	FlyForwardVector = GetActorForwardVector();
	GetCharacterMovement()->MaxFlySpeed = 1500.0f;
	AddActorWorldOffset(FlyForwardVector * FlyingSpeed * (GetWorld()->DeltaRealTimeSeconds));
}




void APlayerBase::Look(const FInputActionInstance& _Instance)
{
	if (Controller != nullptr)
	{
		const TArray<FName>& CheckTag = Tags;
		for (const FName& V : CheckTag)
		{
			if (V == FName("Cody") || V == FName("May"))
			{
				// 1. �̽����� �ٶ󺸴� ������ �÷��̾��� ���溤�Ͱ� �Ǿ���� �椷
				// 2. Move�� �Է����϶� ������-> Move�� ���� ������ ����Ǿ����(idle)
				// 3. Move�߿��� ���ʹ� �����ǰ�, ���� ������ �޶������� �椷
				// 4. ���� ���� -> ��ġ,�� �Ѵ� ���� �椷
				// 5. ���⺤�� -> �丸 ���� �椷

				CameraLookVector = _Instance.GetValue().Get<FVector2D>();

				if (IsFly)
				{
					AddControllerYawInput(CameraLookVector.X * 0.2f);
					//UE_LOG(LogTemp, Warning, TEXT("Look function called"));
					AddControllerPitchInput(-CameraLookVector.Y * 0.2f);
				}
				else
				{
					AddControllerYawInput(CameraLookVector.X * 0.7f);
					AddControllerPitchInput(-CameraLookVector.Y * 0.7f);
				}
			}
		}
	}
}

void APlayerBase::DashInput()
{
	IsSprint = false;
	if (HasAuthority() && !CodyHoldEnemy)
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
		//�뽬 ���۽ð��� üũ
		DashStartTime = GetWorld()->GetTimeSeconds();
		//���鿡 ����ִ����� üũ�Ͽ� ������ �Լ� ����
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
		//�뽬 ���۽ð��� üũ
		DashStartTime = GetWorld()->GetTimeSeconds();
		//���鿡 ����ִ����� üũ�Ͽ� ������ �Լ� ����
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
	// ������ ����
	GetCharacterMovement()->GroundFriction = 0.0f;
	// Cody�� ���溤��
	DashDirection = GetActorForwardVector();
	// ����Normalize
	DashDirection.Normalize();
	// �Ÿ� x ���� ���
	DashVelocity = DashDirection * DashDistance;
	// �ð������� �ӵ�����
	GetCharacterMovement()->Velocity = DashVelocity;
}

void APlayerBase::JumpDash()
{
	// �߷� ����
	GetCharacterMovement()->GravityScale = 0.0f;
	// Cody�� ���溤��
	DashDirection = GetActorForwardVector();
	// ���⺤��normalize
	DashDirection.Normalize();
	// �Ÿ� x ���� ���
	DashVelocity = DashDirection * DashDistance * 0.7f;
	// �ð������� �ӵ�����
	GetCharacterMovement()->Velocity = DashVelocity;
}



void APlayerBase::InteractInput_Implementation()
{
	IsInteract = true;
	if (IsPlayerDeath)
	{
		PlayerHP += 1;
	}
}
void APlayerBase::InteractNoneInput_Implementation()
{
	IsInteract = false;
}

void APlayerBase::PlayerDeathCheck()
{
	if (IsPlayerDeath == false && 0 >= PlayerHP)
	{
		IsPlayerDeath = true;
	}
	if (IsPlayerDeath)
	{
		ChangeState(Cody_State::PlayerDeath);
		if (FullHP == PlayerHP)
		{
			IsPlayerDeath = false;
			ChangeState(Cody_State::IDLE);
		}
	}
}

void APlayerBase::PlayerToHomingRocketJumpStart()
{
	IsFly = true;
	if (HasAuthority())
	{
		CustomClientRideJump();
	}
	else
	{
		CustomServerRideJump();
	}
}


void APlayerBase::CustomClientRideJump_Implementation()
{
	Jump();
	JumplocationSet = true;
	ChangeState(Cody_State::FLYING);
	SpringArm->TargetArmLength = NormalLength;
	SpringArm->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
}

bool APlayerBase::CustomServerRideJump_Validate()
{
	return true;
}
void APlayerBase::CustomServerRideJump_Implementation()
{
	Jump();
	JumplocationSet = true;
	ChangeState(Cody_State::FLYING);
	SpringArm->TargetArmLength = NormalLength;
	SpringArm->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
}

void APlayerBase::PlayerToHomingRoketJumpFinished()
{
	ClientPlayerToHomingRoketJumpFinished();
	ServerPlayerToHomingRoketJumpFinished();
}

void APlayerBase::ClientPlayerToHomingRoketJumpFinished_Implementation()
{
	JumplocationSet = false; 
	JumpLocationDeltas = 0.0f;
}
bool APlayerBase::ServerPlayerToHomingRoketJumpFinished_Validate()
{
	return true;
}
void APlayerBase::ServerPlayerToHomingRoketJumpFinished_Implementation()
{
	JumplocationSet = false;
	JumpLocationDeltas = 0.0f;
}

void APlayerBase::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("HomingRocket"))
	{
		CunstomEndLocation = OtherComp->GetComponentLocation();
		CunstomStartLocation = GetActorLocation();
	}
}

void APlayerBase::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void APlayerBase::SpringArmDefaultFunction()
{
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->TargetArmLength = NormalLength;
	SpringArm->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = true;
}

void APlayerBase::SetTriggerActors(ARespawnTrigger* _Other)
{
	ResPawnTriggers = _Other;
	
}

void APlayerBase::SetRespawnPosition()
{
	if (HasAuthority())
	{
		ResPawnPosition = ResPawnTriggers->GetRespawnPosition();
	}
}

void APlayerBase::AttackPlayer(const int att)
 {
	AttackPlayerServer(att);
}

bool APlayerBase::AttackPlayerServer_Validate(const int att)
{
	return true;
}
void APlayerBase::AttackPlayerServer_Implementation(const int att)
{
	if (PlayerHP<=0)
	{
		return;
	}
	PlayerHP -= att;
}

void APlayerBase::AttackPlayerClient_Implementation(const int att)
{
	if (PlayerHP <= 0)
	{
		return;
	}
	PlayerHP -= att;
}

void APlayerBase::UpdateCamTrans()
{
	if (true == HasAuthority())
	{
		CurControllerLoc = PlayerCameraComponent->GetComponentLocation();
		CurControllerRot = PlayerCameraComponent->GetComponentRotation();
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
	DOREPLIFETIME(APlayerBase, MoveDirection);
	DOREPLIFETIME(APlayerBase, MoveInput);
	DOREPLIFETIME(APlayerBase, ControllerRotation);
	DOREPLIFETIME(APlayerBase, CustomTargetRotation);
	DOREPLIFETIME(APlayerBase, WorldForwardVector);
	DOREPLIFETIME(APlayerBase, WorldRightVector);
	DOREPLIFETIME(APlayerBase, DashDirection);
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
	DOREPLIFETIME(APlayerBase, FlyForwardVector);
	DOREPLIFETIME(APlayerBase, FlyingSpeed);
	DOREPLIFETIME(APlayerBase, CodyHoldEnemy);
	DOREPLIFETIME(APlayerBase, CunstomEndLocation);
	DOREPLIFETIME(APlayerBase, CunstomStartLocation);
	DOREPLIFETIME(APlayerBase, JumplocationSet);
	DOREPLIFETIME(APlayerBase, JumpLocationDeltas);
	DOREPLIFETIME(APlayerBase, CustomTargetLocations);
	DOREPLIFETIME(APlayerBase, CustomTargetLocationsY);
	DOREPLIFETIME(APlayerBase, ResultTargetLocations);
	DOREPLIFETIME(APlayerBase, IsSprint);
	DOREPLIFETIME(APlayerBase, IsPlayerDeath);
	DOREPLIFETIME(APlayerBase, PlayerHP);
	DOREPLIFETIME(APlayerBase, CurControllerRot);
	DOREPLIFETIME(APlayerBase, CurControllerLoc);
	DOREPLIFETIME(APlayerBase, PlayerDefaultSpeed);
	DOREPLIFETIME(APlayerBase, NowPlayerSpeed);
	DOREPLIFETIME(APlayerBase, CurCodySize);
	DOREPLIFETIME(APlayerBase, NextCodySize);
	DOREPLIFETIME(APlayerBase, NowPlayerGravityScale);
	DOREPLIFETIME(APlayerBase, PlayerJumpZVelocity);
	DOREPLIFETIME(APlayerBase, CurrentSitTime);
	DOREPLIFETIME(APlayerBase, SitStartTime);
	DOREPLIFETIME(APlayerBase, SitDuration);
}