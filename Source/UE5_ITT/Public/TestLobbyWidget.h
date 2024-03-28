// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestLobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API UTestLobbyWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Transient, Category = "TestUI", meta = (BindWidgetAnim))
	class UWidgetAnimation* TestWidgetAnimation;

	UFUNCTION(BlueprintCallable) 
	void SetUser1Cody() const;
	
	UFUNCTION(BlueprintCallable)
	void SetReleaseUser1FromCody() const;

	
};
