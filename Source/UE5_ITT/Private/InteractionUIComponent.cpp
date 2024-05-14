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

    bOnlyMay = false;
    bOnlyCody = false;
}


// Called when the game starts
void UInteractionUIComponent::BeginPlay()
{
	Super::BeginPlay();
    InitComponent();
}

void UInteractionUIComponent::InitComponent()
{
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


    // Check if player controller pawn is valid
    APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
    if (!PlayerPawn || PlayerPawn->IsPendingKill())
    {
        return;
    }

    // 태그 대상에 따라 설정
    if (true == SetVisibilityBasedOnTage(PlayerPawn))
    {
        return;
    }

    // 거리에 따라 설정
    SetVisibilityBasedOnDistance(PlayerPawn);

}

bool UInteractionUIComponent::SetVisibilityBasedOnTage(APawn* PlayerPawn)
{
    if (true == bOnlyMay && false == bOnlyCody)
    {
        const TArray<FName>& Tags = PlayerPawn->Tags;

        // 태그를 순회하며 "Cody" 태그 확인
        for (const FName& Tag : Tags)
        {
            if (Tag == FName("Cody"))
            {
                SetVisibility(false, true);
                return true; // "Cody" 태그를 찾았을 경우 비활성화
            }
        }
    }
    if (false == bOnlyMay && true == bOnlyCody)
    {
        const TArray<FName>& Tags = PlayerPawn->Tags;

        // 태그를 순회하며 "May" 태그 확인
        for (const FName& Tag : Tags)
        {
            if (Tag == FName("May"))
            {
                SetVisibility(false, true);
                return true; // "May" 태그를 찾았을 경우 비활성화
            }
        }
    }
    return false;
}

void UInteractionUIComponent::SetVisibilityBasedOnDistance(APawn* PlayerPawn)
{
    if (false == IsVisible())
    {
        NearWidgetComponent->SetVisibility(false);
        FarWidgetComponent->SetVisibility(false);
        return;
    }

    // Calculate distance between player controller and this scene component
    FVector PlayerLocation = PlayerPawn->GetActorLocation();
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