// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerBase.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

// Sets default values
ANetPlayerBase::ANetPlayerBase()
	: CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ANetPlayerBase::OnCreateSessionComplete))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//GetOnlineSubsystem();

}

// Called when the game starts or when spawned
void ANetPlayerBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANetPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANetPlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANetPlayerBase::GetOnlineSubsystem()
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

void ANetPlayerBase::CreateGameSession()
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

void ANetPlayerBase::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
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
			//World->ServerTravel(FString("/Game/ThirdPerson/Maps/Lobby?listen"));
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