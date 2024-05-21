// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractionWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API UInteractionWidget : public UWidgetComponent
{
	GENERATED_BODY()
public:

    // Constructor
    UInteractionWidget(const FObjectInitializer& ObjectInitializer);

    // Æ½ ÇÔ¼ö ¼±¾ð
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, Category = "UI")
    bool bOnlyMay;

    UPROPERTY(EditAnywhere, Category = "UI")
    bool bOnlyCody;

protected:


    // The widget to display when the interaction is near
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction Widget")
    TSubclassOf<UUserWidget> NearWidgetClass;

    // The widget to display when the interaction is far
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction Widget")
    TSubclassOf<UUserWidget> FarWidgetClass;

    UPROPERTY(EditAnywhere, Category = "UI")
    float DistanceThreshold;

    // The widget instance for the near interaction
    UUserWidget* NearWidgetInstance;

    // The widget instance for the far interaction
    UUserWidget* FarWidgetInstance;

protected:
    void SetVisibilityBasedOnDistance();


    void FindTargetActor();
    class APlayerBase* TargetActor;

    float delaytime;
};
