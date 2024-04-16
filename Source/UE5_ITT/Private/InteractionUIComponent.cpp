// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionUIComponent.h"

// Sets default values for this component's properties
UInteractionUIComponent::UInteractionUIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

    DistanceThreshold = 1000.f; // Default distance threshold

    // Create widget components
    NearWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NearWidgetComponent"));
    FarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("FarWidgetComponent"));
}


// Called when the game starts
void UInteractionUIComponent::BeginPlay()
{
	Super::BeginPlay();

    // Attach widget components to this scene component
    NearWidgetComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
    FarWidgetComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

    // Set widget classes
    if (NearWidgetClass)
    {
        NearWidgetComponent->SetWidgetClass(NearWidgetClass);
    }
    if (FarWidgetClass)
    {
        FarWidgetComponent->SetWidgetClass(FarWidgetClass);
    }
    
    // Set widget space to screen
    NearWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    FarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
    // Set widget size
    FVector WidgetScale = FVector(0.5f, 0.5f, 0.5f); // Example scale (adjust as needed)
    NearWidgetComponent->SetWorldScale3D(WidgetScale);
    FarWidgetComponent->SetWorldScale3D(WidgetScale);
    // Initially hide far widget
    FarWidgetComponent->SetVisibility(false);
    
}


// Called every frame
void UInteractionUIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Check if player controller is valid
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (!PlayerController)
    {
        return;
    }

    if (false == IsVisible())
    {
        NearWidgetComponent->SetVisibility(false);
        FarWidgetComponent->SetVisibility(false);
        return;
    }

    // Calculate distance between player controller and this scene component
    FVector PlayerLocation = PlayerController->GetPawn()->GetActorLocation();
    FVector SceneComponentLocation = GetComponentLocation();
    float Distance = FVector::Distance(PlayerLocation, SceneComponentLocation);

    // Toggle widget visibility based on distance threshold
    if (Distance < DistanceThreshold)
    {
        // Player is near, show near widget and hide far widget
        NearWidgetComponent->SetVisibility(true);
        FarWidgetComponent->SetVisibility(false);
    }
    else
    {
        // Player is far, hide near widget and show far widget
        NearWidgetComponent->SetVisibility(false);
        FarWidgetComponent->SetVisibility(true);
    }
}

