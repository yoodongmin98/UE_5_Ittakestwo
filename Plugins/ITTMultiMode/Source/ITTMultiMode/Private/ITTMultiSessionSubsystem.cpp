// Fill out your copyright notice in the Description page of Project Settings.


#include "ITTMultiSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"


UITTMultiSessionSubsystem::UITTMultiSessionSubsystem()
	: CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &UITTMultiSessionSubsystem::OnCreateSessionComplete))
	, FindSessionCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &UITTMultiSessionSubsystem::OnFindSessionComplete))
	, JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &UITTMultiSessionSubsystem::OnJoinSessionComplete))
	, StartSessionCompletetDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &UITTMultiSessionSubsystem::OnCreateSessionComplete))
	, DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &UITTMultiSessionSubsystem::OnDestroySessionComplete))

{
	IOnlineSubsystem* CurOnlineSubsystem = IOnlineSubsystem::Get();
	if (CurOnlineSubsystem)
	{
		// 온라인 세션 받아오기
		ITTSessionInterface = CurOnlineSubsystem->GetSessionInterface();
	}
}



void UITTMultiSessionSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!ITTSessionInterface.IsValid())
		return;

	auto ExistingSession = ITTSessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;

		DestroySession();
	}

	// Add CreateSessionCompleteDelegate to the session interface delegate list.
	CreateSessionCompleteDelegateHandle = ITTSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// Setting options for a new session
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false; // NULL일 경우 LAN 연결
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->bUseLobbiesIfAvailable = true;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->BuildUniqueId = 1;

	// Create Session
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!ITTSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		// Remove the delegate from the delegate list using the handle.
		ITTSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		// Broadcast that the session creation has failed.
		MultiOnCreateSessionComplete.Broadcast(false);
	}
}

void UITTMultiSessionSubsystem::FindSession(int32 MaxSearchResults)
{
	if (!ITTSessionInterface.IsValid())
		return;

	// Add FindSessionCompleteDelegate to the session interface delegate list.
	FindSessionCompleteDelegateHandle = ITTSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegate);

	// Setting session search options
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch);
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// Find sessions
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!ITTSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		// Clear the delegate from the delegate list
		ITTSessionInterface->ClearOnCancelFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegateHandle);
		// Broadcast the result 
		MultiOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);

	}

}

void UITTMultiSessionSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if (!ITTSessionInterface.IsValid())
	{
		MultiOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	// Add JoinSessionCompleteDelegate to the session interface delegate list.
	JoinSessionCompleteDelegateHandle = ITTSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!ITTSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		// Clear the delegate from the delegate list
		ITTSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		// Broadcast the result 
		MultiOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}

}

void UITTMultiSessionSubsystem::StartSession()
{
}

void UITTMultiSessionSubsystem::DestroySession()
{
	if (!ITTSessionInterface.IsValid())
	{
		MultiOnDestroySessionComplete.Broadcast(false);
		return;
	}

	// Add DestroySessionCompleteDelegate to the session interface delegate list.
	DestroySessionCompleteDelegateHandle = ITTSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!ITTSessionInterface->DestroySession(NAME_GameSession))
	{
		// Clear the delegate from the delegate list
		ITTSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		// Broadcast the result 
		MultiOnDestroySessionComplete.Broadcast(false);
	}
}


void UITTMultiSessionSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (ITTSessionInterface)
	{
		// Claer the delegate from the delegate list using the handle
		ITTSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// Broadcast a success message for the session creation.
	MultiOnCreateSessionComplete.Broadcast(true);
}

void UITTMultiSessionSubsystem::OnFindSessionComplete(bool bWasSuccessful)
{
	if (ITTSessionInterface)
	{
		// Claer the delegate from the delegate list using the handle
		ITTSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionCompleteDelegateHandle);
	}

	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		// Session results was empty => false
		MultiOnFindSessionComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	// Broadcast a success message for the session finding.
	MultiOnFindSessionComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
}

void UITTMultiSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (ITTSessionInterface)
	{
		// Claer the delegate from the delegate list using the handle
		ITTSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}


	// Broadcast a message for the session joining with the result.
	MultiOnJoinSessionComplete.Broadcast(Result);
}

void UITTMultiSessionSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}

void UITTMultiSessionSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (ITTSessionInterface)
	{
		ITTSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession(LastNumPublicConnections, LastMatchType);
	}

	MultiOnDestroySessionComplete.Broadcast(bWasSuccessful);
}
