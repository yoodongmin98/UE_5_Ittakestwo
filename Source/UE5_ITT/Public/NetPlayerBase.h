// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "NetPlayerBase.generated.h"

UCLASS()
class UE5_ITT_API ANetPlayerBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANetPlayerBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// about session
	UFUNCTION(BlueprintCallable)
	void CreateGameSession();

	UFUNCTION(BlueprintCallable)
	void JoinGameSession();

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionComplete(bool bWasSuccessful);
	void OnJoinSessionComplate(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void GetOnlineSubsystem();


	IOnlineSessionPtr OnlineSessionInterface;


private:
	TSharedPtr<FOnlineSessionSearch>	SessionSearch;

	FOnCreateSessionCompleteDelegate	CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate		FindSessionCompleteDelegate;
	FOnJoinSessionCompleteDelegate		JoinSessionCompleteDelegate;
	
};
