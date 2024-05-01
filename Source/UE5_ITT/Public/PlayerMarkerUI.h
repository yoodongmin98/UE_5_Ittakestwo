// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PlayerMarkerUI.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE5_ITT_API UPlayerMarkerUI : public UWidgetComponent
{
	GENERATED_BODY()

public:
    // Constructor
    UPlayerMarkerUI();

    // Called when the game starts
    virtual void BeginPlay() override;
    // Override Tick function
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
    // Reference to the player marker widget
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> PlayerMarkerType;
private:
    UWidgetComponent* PlayerMarkerWidget;

    bool bIsOwner;
};