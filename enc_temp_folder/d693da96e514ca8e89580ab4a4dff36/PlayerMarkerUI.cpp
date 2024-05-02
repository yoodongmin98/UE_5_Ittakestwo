// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMarkerUI.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
// Constructor
UPlayerMarkerUI::UPlayerMarkerUI()
{
    // Set default values or initialize as needed
    PrimaryComponentTick.bCanEverTick = true;
    PlayerMarkerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    bIsOwner = true;
    DistanceThreshold = 2500.0f;

    WidgetDefaultLocation = FVector(0.0f, 0.0f, 180.0f);
    TargetPlayer = nullptr;
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
    if(Cast<APawn>(GetOwner()) == GetWorld()->GetFirstPlayerController()->GetPawn())
    {
        SetVisibility(true, true);
    }
    else
    {
        SetVisibility(false, true);
        bIsOwner = true;
    }

}

void UPlayerMarkerUI::FindTarget()
{
    TArray<AActor*> AllPlayerActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerBase::StaticClass(), AllPlayerActors);

    APlayerBase* me = Cast<APlayerBase>(GetOwner());
    // PlayerActors를 순회하며 각 플레이어의 처리 수행
    for (AActor* Player : AllPlayerActors)
    {
        if (me != Cast<APlayerBase>(Player))
        {
            TargetPlayer = Cast<APlayerBase>(Player);
            return;
        }
        // Player를 사용하여 플레이어 처리
    }
}

void UPlayerMarkerUI::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (true == bIsOwner)
    {
        return;
    }

    if (TargetPlayer == nullptr)
    {
        FindTarget();
        return;
    }

    

    APlayerController* FirstPlayer = GetWorld()->GetFirstPlayerController();

    FVector2D ActorScreenPosition;
    if (!FirstPlayer->ProjectWorldLocationToScreen(TargetPlayer->GetActorLocation(), ActorScreenPosition))
    {
        return; // Failed to project actor's location to screen space
    }

    int32 ViewportX, ViewportY;
    FirstPlayer->GetViewportSize(ViewportX, ViewportY);

    if ((ActorScreenPosition.X >= 0 && ActorScreenPosition.X <= ViewportX &&
        ActorScreenPosition.Y >= 0 && ActorScreenPosition.Y <= ViewportY)) // 화면 내에 존재
    {
        float Distance = FVector::Distance(FirstPlayer->GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());

        if (DistanceThreshold < Distance)
        {
            SetVisibility(true, true);
        }
        else
        {
            SetVisibility(false, true);
        }
        SetWorldLocation(TargetPlayer->GetActorLocation());
    }
    else // 화면 밖에 존재
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

        FVector WorldLocation, WorldDirection;
        FirstPlayer->DeprojectScreenPositionToWorld(ActorScreenPosition.X, ActorScreenPosition.Y, WorldLocation, WorldDirection);

        // 카메라 스페이스 좌표로 변환한 후, 카메라와 WidgetComponent의 거리 계산
        FVector CameraLocation = FirstPlayer->PlayerCameraManager->GetCameraLocation();
        FVector WidgetLocation = CameraLocation + WorldDirection * 1200.0f; // 예시로 거리를 100.0f로 설정
        SetWorldLocation(WidgetLocation);
    }

}
