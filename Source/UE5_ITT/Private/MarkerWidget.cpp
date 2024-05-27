// Fill out your copyright notice in the Description page of Project Settings.


#include "MarkerWidget.h"
#include "Kismet/GameplayStatics.h"

UMarkerWidget::UMarkerWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // Default initialization

	

    bChecked = false;

    DistanceThreshold = 4000.0f; // Default distance threshold
    Distance = 0.0f;

    ViewportSizex = 0;
    ViewportSizey = 0;

    SpawnTime = FDateTime::Now();
}

void UMarkerWidget::SetIsCody()
{
    if(GetOwner())
    {
        const TArray<FName>& CheckTag = GetOwner()->Tags;
        for (const FName& V : CheckTag)
        {
            if (V == FName("Cody") || V == FName("May"))
            {
                if (V == FName("Cody"))
                {
                    bIsCody = true;
                    bChecked = true;
                }
                else if(V == FName("May"))
                {
                    bIsCody = false;
                    bChecked = true;
                }
                return;
            }
        }
    }
}
// 틱 함수 구현
void UMarkerWidget::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (false == bChecked)
    {
        SetIsCody();
        return;
    }
    if (!TargetActor)
    {
        FindTargetActor();
        return;
    }

    if (!MyViewController)
    {
        FDateTime CurTime = FDateTime::Now();
        if ((CurTime - SpawnTime).GetTotalSeconds() < 10)
        {
            return;
        }
		FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();
        if(true == bIsCody)
        {
            int32 NumofPlayer = GetWorld()->GetNumPlayerControllers();
            if (NumofPlayer == 2)
            {
                It++;
            }
            else if (NumofPlayer == 4)
            {
                It++;
                It++;
            }
            else
            {
                return;
            }
        }


        if (!It)
        {
            return;
        }
        if (It->Get())
        {
            MyViewController = It->Get();
            MyViewController->GetViewportSize(ViewportSizex, ViewportSizey);
            ViewportSizex /= 2;
        }
        return;
    }
    if (!MarkerInstance)
    {
		MarkerInstance = CreateWidget<UUserWidget>(GetWorld(), UIWidgetClass);
        if (true == bIsCody)
        {
			MarkerInstance->AddToViewport();
        }
        else
        {
			MarkerInstance->AddToViewport();
        }
        MarkerInstance->SetVisibility(ESlateVisibility::Hidden);
        return;
    }
    if (true == bIsCody)
    {
        FVector2D ActorScreenPosition;
        FVector TargetLocation = TargetActor->GetActorLocation();
        if (!MyViewController->ProjectWorldLocationToScreen(TargetLocation, ActorScreenPosition))
        {
            return; // Failed to project actor's location to screen space
        }
        if ((ActorScreenPosition.X >= ViewportSizex && ActorScreenPosition.X <= ViewportSizex*2 &&
            ActorScreenPosition.Y >= 0 && ActorScreenPosition.Y <= ViewportSizey )) // 화면 내에 존재
        {
            Distance = FVector::Distance(GetOwner()->GetActorLocation(), TargetLocation);

            if (DistanceThreshold < Distance)
            {
                MarkerInstance->SetVisibility(ESlateVisibility::Visible);
            }
            else
            {
                MarkerInstance->SetVisibility(ESlateVisibility::Hidden);
                return;
            }
        }
        else // 화면 밖에 존재
        {
            MarkerInstance->SetVisibility(ESlateVisibility::Visible);
            if (ActorScreenPosition.X < ViewportSizex)
            {
                ActorScreenPosition.X = ViewportSizex + 30.0f;
            }
			else if (ActorScreenPosition.X > ViewportSizex * 2)
            {
				ActorScreenPosition.X = ViewportSizex * 2 - 10.0f;
            }

            if (ActorScreenPosition.Y < 0)
            {
                ActorScreenPosition.Y = 30.0f;
            }
            else if (ActorScreenPosition.Y > ViewportSizey)
            {
                ActorScreenPosition.Y = ViewportSizey - 10.0f;
            }

        }
		ActorScreenPosition.X -= 23.0f;
		ActorScreenPosition.Y -= 60.0f;
		MarkerInstance->SetPositionInViewport(ActorScreenPosition);
        UE_LOG(LogTemp, Display, TEXT("MayPos in view %f %f"), ActorScreenPosition.X, ActorScreenPosition.Y);
    }
    else
    {

        FVector2D ActorScreenPosition;
        FVector TargetLocation = TargetActor->GetActorLocation();
        if (!MyViewController->ProjectWorldLocationToScreen(TargetLocation, ActorScreenPosition))
        {
            return; // Failed to project actor's location to screen space
        }
        if ((ActorScreenPosition.X >= 0 && ActorScreenPosition.X <= ViewportSizex &&
            ActorScreenPosition.Y >= 0 && ActorScreenPosition.Y <= ViewportSizey)) // 화면 내에 존재
        {
            Distance = FVector::Distance(GetOwner()->GetActorLocation(), TargetLocation);

            if (DistanceThreshold < Distance)
            {
                MarkerInstance->SetVisibility(ESlateVisibility::Visible);
            }
            else
            {
                MarkerInstance->SetVisibility(ESlateVisibility::Hidden);
                return;
            }
            //MarkerInstance->SetPositionInViewport(ActorScreenPosition);
            //SetWorldLocation(TargetPlayer->GetActorLocation() + WidgetDefaultLocation);
        }
        else // 화면 밖에 존재
        {
            MarkerInstance->SetVisibility(ESlateVisibility::Visible);
            if (ActorScreenPosition.X < 0)
            {
                ActorScreenPosition.X = 30.0f;
            }
            else if (ActorScreenPosition.X > ViewportSizex)
            {
                ActorScreenPosition.X = ViewportSizex - 10.0f;
            }

            if (ActorScreenPosition.Y < 0)
            {
                ActorScreenPosition.Y = 30.0f;
            }
            else if (ActorScreenPosition.Y > ViewportSizey)
            {
                ActorScreenPosition.Y = ViewportSizey - 10.0f;
            }
        }
		ActorScreenPosition.X -= 23.0f;
		ActorScreenPosition.Y -= 60.0f;
		MarkerInstance->SetPositionInViewport(ActorScreenPosition);

    }
}

void UMarkerWidget::FindTargetActor()
{
    TArray<AActor*> AllPlayerActors;

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerBase::StaticClass(), AllPlayerActors);

    // PlayerActors를 순회하며 각 플레이어의 처리 수행
    for (AActor* Player : AllPlayerActors)
    {
		APlayerBase* NextPlayer = Cast<APlayerBase>(Player);
		const TArray<FName>& CheckTag = NextPlayer->Tags;
		for (const FName& V : CheckTag)
		{
			if (V == FName("Cody") || V == FName("May"))
			{
				if (false == bIsCody && V == FName("Cody")) // 본인이 May라면 target은 Cody
				{
					TargetActor = NextPlayer;
					return;
				}
				else if (true == bIsCody && V == FName("May")) // 본인이 Cody라면 Target은 May
				{
					TargetActor = NextPlayer;
					return;
				}
			}
		}
    }
}

FVector2D UMarkerWidget::GetRenderPosition()
{

    //FVector2D ActorScreenPosition;
    //FVector TargetLocation = TargetPlayer->GetActorLocation();

    //if (!OwnerPlayer->ProjectWorldLocationToScreen(TargetLocation, ActorScreenPosition))
    //{
    //    return FVector2D(-1, -1); // Failed to project actor's location to screen space
    //}

    //if ((ActorScreenPosition.X >= StartViewX && ActorScreenPosition.X <= StartViewX + SizeViewX &&
    //    ActorScreenPosition.Y >= StartViewY && ActorScreenPosition.Y <= StartViewY + SizeViewY)) // 화면 내에 존재
    //{
    //    float Distance = FVector::Distance(OwnerPlayer->GetPawn()->GetActorLocation(), TargetLocation);

    //    //임시 확인용
    //    //return ActorScreenPosition;

    //    if (DistanceThreshold < Distance)
    //    {
    //        return FVector2D(-1, -1);
    //    }
    //    else
    //    {
    //        if (MyTag == FName("Cody"))
    //        {
    //            ActorScreenPosition.X += SizeViewX / 2;
    //        }
    //        else
    //        {
    //            ActorScreenPosition.X -= SizeViewX / 2;
    //        }
    //        return ActorScreenPosition;
    //    }
    //    //SetWorldLocationAndRotation(TargetLocation + WidgetDefaultLocation, FRotator::ZeroRotator);
    //    //SetWorldLocation(TargetPlayer->GetActorLocation()+ WidgetDefaultLocation);
    //}
    //else // 화면 밖에 존재
    //{
    //    //SetVisibility(true, true);
    //    if (ActorScreenPosition.X < StartViewX)
    //    {
    //        ActorScreenPosition.X = StartViewX + 10.0f;
    //    }
    //    else if (ActorScreenPosition.X > StartViewX + SizeViewX)
    //    {
    //        ActorScreenPosition.X = StartViewX + SizeViewX - 10.0f;
    //    }

    //    if (ActorScreenPosition.Y < StartViewY)
    //    {
    //        ActorScreenPosition.Y = StartViewY + 10.0f;
    //    }
    //    else if (ActorScreenPosition.Y > StartViewY + SizeViewY)
    //    {
    //        ActorScreenPosition.Y = StartViewY + SizeViewY - 10.0f;
    //    }
    //    if (MyTag == FName("Cody"))
    //    {
    //        ActorScreenPosition.X += SizeViewX / 2;
    //    }
    //    else
    //    {
    //        ActorScreenPosition.X -= SizeViewX / 2;
    //    }

    //    return ActorScreenPosition;

    //}
    return FVector2D(0.0f, 0.0f);
}

void UMarkerWidget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}