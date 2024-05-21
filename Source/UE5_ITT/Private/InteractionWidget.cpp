// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionWidget.h"
#include "PlayerBase.h"
#include "Kismet/GameplayStatics.h"

UInteractionWidget::UInteractionWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Default initialization
    NearWidgetInstance = nullptr;
    FarWidgetInstance = nullptr;
    DistanceThreshold = 1000.f; // Default distance threshold


    bOnlyMay = false;
    bOnlyCody = false;
    delaytime = 1.0f;
}


// 틱 함수 구현
void UInteractionWidget::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (!NearWidgetInstance)
    {
        NearWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), NearWidgetClass);
        SetWidget(NearWidgetInstance);
        return;
    }
    if (!FarWidgetInstance)
    {
        FarWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), FarWidgetClass);
        SetWidget(FarWidgetInstance);
        return;
    }
    if (!TargetActor)
    {
        FindTargetActor();
        return;
    }

    if (delaytime > 0.0f)
    {
        delaytime -= DeltaTime;
        return;
    }

    FVector PlayerLocation = TargetActor->GetActorLocation();
    FVector SceneComponentLocation = GetComponentLocation();
    float Distance = FVector::Distance(PlayerLocation, SceneComponentLocation);

    // Toggle widget visibility based on distance threshold
    if (Distance < DistanceThreshold)
    {
        // Player is near, show near widget and hide far widget
       
        //SetWidget(NearWidgetInstance);
        delaytime = 0.5f;
        //NearWidgetComponent->SetVisibility(true);
        //FarWidgetComponent->SetVisibility(false);
    }
    else
    {
        // Player is far, hide near widget and show far widget
        //SetWidget(FarWidgetInstance);
        delaytime = 0.5f;
        //NearWidgetComponent->SetVisibility(false);
        //FarWidgetComponent->SetVisibility(true);
    }
    //SetVisibilityBasedOnDistance();
}
//
//void UInteractionWidget::SetNearWidget(TSubclassOf<UUserWidget> WidgetClass)
//{
//    NearWidgetClass = WidgetClass;
//}
//
//void UInteractionWidget::SetFarWidget(TSubclassOf<UUserWidget> WidgetClass)
//{
//    FarWidgetClass = WidgetClass;
//}



void UInteractionWidget::SetVisibilityBasedOnDistance()
{
    FVector PlayerLocation = TargetActor->GetActorLocation();
    FVector SceneComponentLocation = GetComponentLocation();
    float Distance = FVector::Distance(PlayerLocation, SceneComponentLocation);

    // Toggle widget visibility based on distance threshold
    if (Distance < DistanceThreshold)
    {
        // Player is near, show near widget and hide far widget
        //SetWidget(NearWidgetInstance);
        //NearWidgetComponent->SetVisibility(true);
        //FarWidgetComponent->SetVisibility(false);
    }
    else
    {
        // Player is far, hide near widget and show far widget
        //SetWidget(FarWidgetInstance);
        //NearWidgetComponent->SetVisibility(false);
        //FarWidgetComponent->SetVisibility(true);
    }
}




void UInteractionWidget::FindTargetActor()
{
    TArray<AActor*> AllPlayerActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerBase::StaticClass(), AllPlayerActors);

    APlayerBase* me = Cast<APlayerBase>(GetOwner());
    // PlayerActors를 순회하며 각 플레이어의 처리 수행
    for (AActor* Player : AllPlayerActors)
    {
        if (me != Cast<APlayerBase>(Player))
        {
            APlayerBase* NextPlayer = Cast<APlayerBase>(Player);
            const TArray<FName>& CheckTag = NextPlayer->Tags;
            for (const FName& V : CheckTag)
            {
                if (V == FName("Cody") || V == FName("May"))
                {
                    if (true == bOnlyMay && V == FName("May"))
                    {
                        TargetActor = NextPlayer;
                        //SetWidget(NearWidgetInstance);
                        //SetWidget(FarWidgetInstance);
                        return;
                    }
                    else if (true == bOnlyCody && V == FName("Cody"))
                    {
                        TargetActor = NextPlayer;
                        //SetWidget(NearWidgetInstance);
                        //SetWidget(FarWidgetInstance);
                        return;
                    }
                }
            }
        }
    }
    // Player를 사용하여 플레이어 처리
}