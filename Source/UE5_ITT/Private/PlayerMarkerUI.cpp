// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMarkerUI.h"
#include "Components/WidgetComponent.h"

// Constructor
UPlayerMarkerUI::UPlayerMarkerUI()
{
    // Set default values or initialize as needed
    PrimaryComponentTick.bCanEverTick = true;
    PlayerMarkerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NearWidgetComponent"));
    bIsOwner = false;
}
void UPlayerMarkerUI::BeginPlay()
{
    Super::BeginPlay();

    PlayerMarkerWidget->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

    // Set widget classes
    if (PlayerMarkerWidget)
    {
        PlayerMarkerWidget->SetWidgetClass(PlayerMarkerType);
    }
    // Set widget space to screen
    PlayerMarkerWidget->SetWidgetSpace(EWidgetSpace::Screen);
}
// Override Tick function
void UPlayerMarkerUI::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (true == bIsOwner)
    {
        return;
    }
    // Check if the player marker widget is valid
    if (PlayerMarkerWidget)
    {

        APlayerController* OwningPlayerController = Cast<APlayerController>(GetOwner()->GetOwner());

        // Get the player controller of the controlling player
        APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
        if (PlayerController != nullptr && OwningPlayerController != nullptr)
        {
            // Check if the owner of the widget component is the same as the player controller
            if (OwningPlayerController == PlayerController)
            {
                // Hide the widget if the owner is the same as the player controller
                SetVisibility(false, true);
                bIsOwner = true;
            }
        }
        // Perform your custom logic here
        // For example, update the widget based on some condition
        // PlayerMarkerWidget->SetVisibility(ESlateVisibility::Visible); // Example code

    }
}