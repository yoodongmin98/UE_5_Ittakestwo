// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMarkerUI.h"
#include "Components/WidgetComponent.h"
#include "Cody.h"

// Constructor
UPlayerMarkerUI::UPlayerMarkerUI()
{
    // Set default values or initialize as needed
    PrimaryComponentTick.bCanEverTick = true;
    PlayerMarkerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    bIsOwner = true;
    DistanceThreshold = 2500.0f;

    WidgetDefaultLocation = FVector(0.0f, 0.0f, 0.0f);
}
void UPlayerMarkerUI::BeginPlay()
{

    Super::BeginPlay();
    bIsOwner = false;

    PlayerMarkerWidget->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

    // Set widget classes
    if (PlayerMarkerWidget)
    {
        PlayerMarkerWidget->SetWidgetClass(PlayerMarkerType);
    }
    // Set widget space to screen
    PlayerMarkerWidget->SetWidgetSpace(EWidgetSpace::Screen);
    //PlayerMarkerWidget->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
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

    WidgetDefaultLocation = GetRelativeLocation();
}
// Override Tick function
void UPlayerMarkerUI::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (true == bIsOwner)
    {
        WidgetDefaultLocation = FVector(0.0f, 0.0f, 0.0f);
        return;
    }
    // Check if the player marker widget is valid
    

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    APawn* PlayerPawn = PlayerController->GetPawn();
    if (!PlayerPawn || PlayerPawn->IsPendingKill())
    {
        return;
    }

    FVector PlayerLocation = PlayerPawn->GetActorLocation();
    FVector SceneComponentLocation = GetComponentLocation();
    float Distance = FVector::Distance(PlayerLocation, SceneComponentLocation);

    // Toggle widget visibility based on distance threshold
    //if (Distance < DistanceThreshold)
    //{
    //    // Player is near, show near widget and hide far widget
    //    PlayerMarkerWidget->SetVisibility(false);
    //}
    //else
    {
        // Player is far, hide near widget and show far widget
        PlayerMarkerWidget->SetVisibility(true);
    }

    IsRenderingWidget(PlayerController);
}

bool UPlayerMarkerUI::IsRenderingWidget(APlayerController* _PlayerController)
{
    if (_PlayerController == nullptr)
    {
        return false;
    }

    // Get the player's viewport size
    int32 ViewportX, ViewportY;
	_PlayerController->GetViewportSize(ViewportX, ViewportY);

    // Convert the actor's location to screen space
    FVector2D ActorScreenPosition;
    if (!_PlayerController->ProjectWorldLocationToScreen(GetComponentLocation(), ActorScreenPosition))
    {
        return false; // Failed to project actor's location to screen space
    }

    ACody* CodyPlayer = Cast<ACody>(_PlayerController->GetCharacter());


    // 화면 안에 존재할 때
    if ((ActorScreenPosition.X >= 0 && ActorScreenPosition.X <= ViewportX &&
        ActorScreenPosition.Y >= 0 && ActorScreenPosition.Y <= ViewportY))
    {
		UE_LOG(LogTemp, Display, TEXT("rendering"));
        CodyPlayer->MarkerUIWidget->SetVisibility(false, true);
        return true;
    }
    else // 화면 밖으로 나갔을 때
    {
		if (ActorScreenPosition.X < 0)
		{
			ActorScreenPosition.X = 10.0f;
		}
		else if (ActorScreenPosition.X > ViewportX)
		{
			ActorScreenPosition.X = ViewportX - 10.0f;
		}

		if (ActorScreenPosition.Y < 0)
		{
			ActorScreenPosition.Y = 10.0f;
		}
		else if (ActorScreenPosition.Y > ViewportY)
		{
			ActorScreenPosition.Y = ViewportY - 10.0f;
		}

        // 잘못된 코드
        ActorScreenPosition.X -= ViewportX/2;
        ActorScreenPosition.Y = ViewportY/2- ActorScreenPosition.Y;

        CodyPlayer->MarkerUIWidget->SetVisibility(true, true);

        CodyPlayer->MarkerUIWidget->SetRelativeLocation(FVector(0.0f, ActorScreenPosition.X, ActorScreenPosition.Y));
		//SetWorldLocation(FVector(ActorScreenPosition.X, ActorScreenPosition.Y, 0.0f));
		//PlayerMarkerWidget->SetRelative2dlocation(ActorScreenPosition);
		UE_LOG(LogTemp, Display, TEXT("not rendering%f %f"), ActorScreenPosition.X, ActorScreenPosition.Y);
		// Check if the actor's screen position is inside the viewport
		return false;

    }


}
