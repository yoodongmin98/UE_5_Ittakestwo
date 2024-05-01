// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ITTBaseMenu.generated.h"

/**
 * 
 */
UCLASS()
class ITTMULTIMODE_API UITTBaseMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));

	UFUNCTION(BlueprintCallable)
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSession(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bwasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	//UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	//UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);



	UFUNCTION(BlueprintCallable)
	void CreateSubsystemSession();

	UFUNCTION(BlueprintCallable)
	void JoinSubsystemSession();

private:

	class UITTMultiSessionSubsystem* ITTSessionSubsystem; // The Subsystem designed to handle all online session functionality
	int32	NumPublicConnections{ 2 };
	FString MatchType{ TEXT("FreeForAll") };
	FString PathToLevel{ TEXT("/Game/ThirdPerson/Maps/Lobby") };
	

	void MenuTearDown();
};
