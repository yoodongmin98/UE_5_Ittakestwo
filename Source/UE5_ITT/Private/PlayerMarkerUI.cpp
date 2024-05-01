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
    DistanceThreshold = 2500.0f;
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
                PrimaryComponentTick.bCanEverTick = false;

            }
        }

    }
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
    
    APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
    if (!PlayerPawn || PlayerPawn->IsPendingKill())
    {
        return;
    }

    FVector PlayerLocation = PlayerPawn->GetActorLocation();
    FVector SceneComponentLocation = GetComponentLocation();
    float Distance = FVector::Distance(PlayerLocation, SceneComponentLocation);

    // Toggle widget visibility based on distance threshold
    if (Distance < DistanceThreshold)
    {
        // Player is near, show near widget and hide far widget
        PlayerMarkerWidget->SetVisibility(false);
    }
    else
    {
        // Player is far, hide near widget and show far widget
        PlayerMarkerWidget->SetVisibility(true);
    }
}