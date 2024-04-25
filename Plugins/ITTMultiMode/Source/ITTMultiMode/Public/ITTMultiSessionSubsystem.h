// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "ITTMultiSessionSubsystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiOnCreateSessionComplete, bool, bWasSuccesssful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiOnFindSessionComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bwasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiOnStartSessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiOnDestroySessionComplete, bool, bWasSuccessful);

UCLASS()
class ITTMULTIMODE_API UITTMultiSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UITTMultiSessionSubsystem();

	void CreateSession(int32 NumPublicConnections, FString MatchType);
	void FindSession(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void StartSession();
	void DestroySession();

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

	FMultiOnCreateSessionComplete	MultiOnCreateSessionComplete;
	FMultiOnFindSessionComplete		MultiOnFindSessionComplete;
	FMultiOnJoinSessionComplete		MultiOnJoinSessionComplete;
	FMultiOnStartSessionComplete	MultiOnStartSessionComplete;
	FMultiOnDestroySessionComplete	MultiOnDestroySessionComplete;

private:
	IOnlineSessionPtr ITTSessionInterface;
	TSharedPtr<FOnlineSessionSettings>	LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch>	LastSessionSearch;

	bool	bCreateSessionOnDestroy{ false };
	int32	LastNumPublicConnections;
	FString LastMatchType;

	FOnCreateSessionCompleteDelegate	CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate		FindSessionCompleteDelegate;
	FOnJoinSessionCompleteDelegate		JoinSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate	DestroySessionCompleteDelegate;
	FOnStartSessionCompleteDelegate		StartSessionCompletetDelegate;
	

	FDelegateHandle						CreateSessionCompleteDelegateHandle;
	FDelegateHandle						FindSessionCompleteDelegateHandle;
	FDelegateHandle						JoinSessionCompleteDelegateHandle;
	FDelegateHandle						StartSessionCompleteDelegateHandle;
	FDelegateHandle						DestroySessionCompleteDelegateHandle;
};
