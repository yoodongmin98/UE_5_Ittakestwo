// Fill out your copyright notice in the Description page of Project Settings.


#include "ITTBaseMenu.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "ITTMultiSessionSubsystem.h"

void UITTBaseMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	PathToLevel = FString::Printf(TEXT("%s?listen"), *LobbyPath);


	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		ITTSessionSubsystem = GameInstance->GetSubsystem<UITTMultiSessionSubsystem>();
	}

	// Binding callbacks to Delegates of the MultiplayerSessionSubsystem class
	if (ITTSessionSubsystem)
	{
		ITTSessionSubsystem->MultiOnCreateSessionComplete.AddDynamic(this, &UITTBaseMenu::OnCreateSession);
		ITTSessionSubsystem->MultiOnFindSessionComplete.AddUObject(this, &UITTBaseMenu::OnFindSession);
		ITTSessionSubsystem->MultiOnJoinSessionComplete.AddUObject(this, &UITTBaseMenu::OnJoinSession);
		ITTSessionSubsystem->MultiOnStartSessionComplete.AddDynamic(this, &UITTBaseMenu::OnStartSession);
		ITTSessionSubsystem->MultiOnDestroySessionComplete.AddDynamic(this, &UITTBaseMenu::OnDestroySession);
	}
}


void UITTBaseMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString(TEXT("Session created successfully!")));
		}

		UWorld* World = GetWorld();
		if (World)
		{
			// 세션을 생성하면 Lobby로 이동
			World->ServerTravel(PathToLevel);
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Failed to create session!")));
		}

	}
}

void UITTBaseMenu::OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bwasSuccessful)
{
	if (ITTSessionSubsystem == nullptr)
		return;

	for (auto Result : SessionResults)
	{
		// 매치 타입 확인하기
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);

		// 매치 타입이 요건에 맞다면 JoinSession
		if (SettingsValue == MatchType)
		{
			ITTSessionSubsystem->JoinSession(Result);
			return;
		}
	}
}

void UITTBaseMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	// SessionInterface
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			/* Join the Session */
			FString Address;
			if (SessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
			{
				APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
				if (PlayerController)
				{
					PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
				}
			}
		}
	}
}

void UITTBaseMenu::OnStartSession(bool bWasSuccessful)
{
}

void UITTBaseMenu::OnDestroySession(bool bWasSuccessful)
{
}