// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Logging/LogMacros.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APlayerBase::APlayerBase()
	: CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete))
	, FindSessionCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete))
	, JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplate))
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
	SpringArm->bDoCollisionTest = true;

	//카메라 생성
	PlayerCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCameraComponent->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	PlayerCameraComponent->bUsePawnControlRotation = false;

	PrimaryActorTick.bStartWithTickEnabled = true;
	

	//// OnlineSubsystem에 Access
	//IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	//if (OnlineSubsystem)
	//{
	//	// 온라인 세션 받아오기
	//	OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

	//	if (GEngine)
	//	{
	//		// OnlineSubsystem 이름 출력하기
	//		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Found subsystem %s"), *OnlineSubsystem->GetSubsystemName().ToString()));
	//	}
	//}



}

void APlayerBase::GetOnlineSubsystem()
{
	// OnlineSubsystem에 Access
	IOnlineSubsystem* CurOnlineSubsystem = IOnlineSubsystem::Get();

	if (CurOnlineSubsystem)
	{
		// 온라인 세션 받아오기
		OnlineSessionInterface = CurOnlineSubsystem->GetSessionInterface();

		if (GEngine)
		{
			// OnlineSubsystem 이름 출력하기
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Found subsystem %s"), *CurOnlineSubsystem->GetSubsystemName().ToString()));
		}
	}
}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	GetOnlineSubsystem();
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


	IsMoveEnd = true;

	bIsDashing = false;
	bIsDashingStart = false;
	bCanDash = false;
	BigCanDash = true;

	CurrentAnimationEnd = false;
	IsSprint = false;

	CanSit = true;
	SitDuration = 0.5f;
	ChangeIdle = true;
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime >= DashStartTime + DashDuration)
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
		PlayerInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerBase::Move_Implementation);
		PlayerInput->BindAction(MoveAction, ETriggerEvent::None, this, &APlayerBase::Idle);
		PlayerInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);
		PlayerInput->BindAction(DashAction, ETriggerEvent::Triggered, this, &APlayerBase::DashInput);
		PlayerInput->BindAction(DashAction, ETriggerEvent::None, this, &APlayerBase::DashNoneInput);
		PlayerInput->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerBase::InteractInput);
		PlayerInput->BindAction(InteractAction, ETriggerEvent::None, this, &APlayerBase::InteractNoneInput);
		PlayerInput->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerBase::SprintInput);
		PlayerInput->BindAction(SprintAction, ETriggerEvent::None, this, &APlayerBase::SprintNoneInput);
	}
}


//////////////////////////////캐릭터 이동관련///////////////////////////
//새로운 입력 Action
void APlayerBase::Idle(const FInputActionInstance& _Instance)
{
	IsSprint = false;
	IsMoveEnd = false;
	if (bCanDash == false)
		ChangeState(Cody_State::IDLE);
}

void APlayerBase::Move_Implementation(const FInputActionInstance& _Instance)
{
	//UE_LOG(LogTemp, Warning, TEXT("Move function called"));
	IsMoveEnd = true;

	if (bCanDash == false && ChangeIdle)
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

				AddControllerYawInput(CameraLookVector.X);

				// 카메라의 피치 각도 제한
				// 90도 넘어가면 스프링암 타겟길이에 영향을 미쳐야함.
				float CurrentPitch = GetControlRotation().Pitch;
				float NewPitch = FMath::ClampAngle(CurrentPitch + CameraLookVector.Y, -90.0f, 0.0f); // -90도부터 0도 사이로 제한
				FRotator NewRotation = FRotator(NewPitch, GetControlRotation().Yaw, GetControlRotation().Roll);
				Controller->SetControlRotation(NewRotation);
			}
		}
		
	}
}

void APlayerBase::DashInput()
{
	IsSprint = false;
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
	FVector DashVelocity = DashDirection * DashDistance * 0.8f;
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
void APlayerBase::InteractInput()
{
	IsInteract = true;
}
void APlayerBase::InteractNoneInput()
{
	IsInteract = false;
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
}



void APlayerBase::CreateGameSession()
{
	// Called when pressing the 1key
	if (!OnlineSessionInterface.IsValid())
	{
		// log
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Game Session Interface is invailed")));
		}
		return;
	}

	// 이미 세션이 존재한다면 기존 세션을 삭제한다
	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);

		// Log
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Black, FString::Printf(TEXT("Destroy session : %s"), NAME_GameSession));
		}
	}

	// 세션 생성 완료 후 호출될 delegate 리스트에 CreateSessionCompleteDelegate 추가
	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// 세션 세팅하기
	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());

	SessionSettings->bIsLANMatch = false;			// LAN 연결
	SessionSettings->NumPublicConnections = 4;		// 최대 접속 가능 수
	SessionSettings->bAllowJoinInProgress = true;	// Session 진행중에 접속 허용
	SessionSettings->bAllowJoinViaPresence = true;  // 세션 참가 지역을 현재 지역으로 제한 (스팀의 presence 사용)
	SessionSettings->bShouldAdvertise = true;		// 현재 세션을 광고할지 (스팀의 다른 플레이어에게 세션 홍보 여부)
	SessionSettings->bUsesPresence = true;			// 현재 지역에 세션 표시
	SessionSettings->bUseLobbiesIfAvailable = true; // 플랫폼이 지원하는 경우 로비 API 사용
	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing); // 세션의 MatchType을 모두에게 무료, 온라인서비스와 핑을 통해 세션 홍보 옵션으로 설정

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
}

void APlayerBase::JoinGameSession()
{
	// 세션 인터페이스 유효성 검사
	if (!OnlineSessionInterface.IsValid())
	{
		// log
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Game Session Interface is invailed")));
		}
		return;
	}

	// Find Session Complete Delegate 등록
	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegate);

	// Find Game Session
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000;	// 검색 결과로 나오는 세션 수 최대값
	SessionSearch->bIsLanQuery = false;			// LAN 사용 여부
	SessionSearch->QuerySettings.Set(FName(TEXT("PRESENCESEARCH")), true, EOnlineComparisonOp::Equals); // 찾을 세션 쿼리를 현재로 설정한다

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void APlayerBase::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// 세션 생성 성공!
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Created session : %s"), *SessionName.ToString()));
		}

		// 로비 맵으로 이동
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(FString("/Game/ThirdPerson/Maps/Lobby?listen"));
		}
	}

	// 세선 생성 실패
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Failed to create session!")));
		}
	}
}

void APlayerBase::OnFindSessionComplete(bool bWasSuccessful)
{
	if (!OnlineSessionInterface.IsValid()
		|| !bWasSuccessful)
		return;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("======== Search Result ========")));
	}

	for (auto Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;

		// 매치 타입 확인하기
		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

		// 찾은 세션의 정보 출력하기
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("Session ID : %s / Owner : %s"), *Id, *User));
		}

		// 세션의 매치 타입이 "FreeForAll"일 경우 세션 참가
		if (MatchType == FString("FreeForAll"))
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("Joining Match Type : %s"), *MatchType));
			}

			// Join Session Complete Delegate 등록 
			OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);


			const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
			OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
		}
	}
}

void APlayerBase::OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid())
		return;

	// 세션에 조인했다면 IP Address얻어와서 해당 서버에 접속
	FString Address;
	if (OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Connect String : %s"), *Address));
		}

		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}
