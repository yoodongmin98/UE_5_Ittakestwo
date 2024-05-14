// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerIndicator.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerBase.h"


void UPlayerIndicator::Setting(APawn* _Target, int32 _x, int32 _y, int32 _xs, int32 _ys)
{
    TargetPlayer = _Target;
    StartViewX = _x, StartViewY = _y;
    SizeViewX = _xs, SizeViewY = _ys;
}

void UPlayerIndicator::FindMyTag()
{
    APlayerController* FirstPlayer = GetWorld()->GetFirstPlayerController();
    // PlayerActors를 순회하며 각 플레이어의 처리 수행
    if(FirstPlayer)
    {
        APawn* CurPawn = FirstPlayer->GetPawn();
        FirstPlayer->GetViewportSize(SizeViewX, SizeViewY);
        SizeViewX /= 2;

		const TArray<FName>& CheckTag = CurPawn->Tags;
		for (const FName& V : CheckTag)
		{
			if (V == FName("Cody"))
			{
                MyTag = "Cody";
                StartViewX = SizeViewX/2;
                break;
			}
            else if (V == FName("May"))
            {
                MyTag = "May";
                break;
            }
		}
    }
}

void UPlayerIndicator::FindTarget()
{
    if(MyTag == "Cody" || MyTag == "May")
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
                if (FName("Cody") == MyTag && FName("May") == V)
                {
					TargetPlayer = NextPlayer;
					return;
                }
                else if (FName("May") == MyTag && FName("Cody") == V)
                {
                    TargetPlayer = NextPlayer;
                    return;
                }
            }
        }
    }
    else
    {
        FindMyTag();
    }
}

void UPlayerIndicator::SetTagName(FString _Name)
{
    MyTag = FName(_Name);
}
void UPlayerIndicator::SetTarget(APawn* _Tar)
{
    TargetPlayer = _Tar;
}



FVector2D UPlayerIndicator::GetRenderPosition()
{
    APlayerController* OwnerPlayer = GetOwningPlayer();
    if (!TargetPlayer || !OwnerPlayer)
    {
        //FindTarget();
        return FVector2D(-1, -1);
    }


    FVector2D ActorScreenPosition;
    FVector TargetLocation = TargetPlayer->GetActorLocation();

    if (!OwnerPlayer->ProjectWorldLocationToScreen(TargetLocation, ActorScreenPosition))
    {
        return FVector2D(-1, -1); // Failed to project actor's location to screen space
    }

    if ((ActorScreenPosition.X >= StartViewX && ActorScreenPosition.X <= StartViewX + SizeViewX &&
        ActorScreenPosition.Y >= StartViewY && ActorScreenPosition.Y <= StartViewY + SizeViewY)) // 화면 내에 존재
    {
        float Distance = FVector::Distance(OwnerPlayer->GetPawn()->GetActorLocation(), TargetLocation);

        //임시 확인용
        //return ActorScreenPosition;

        if (DistanceThreshold < Distance)
        {
            return FVector2D(-1, -1);
        }
        else
        {
            if (MyTag == FName("Cody"))
            {
                ActorScreenPosition.X += SizeViewX / 2;
            }
            else
            {
                ActorScreenPosition.X -= SizeViewX / 2;
            }
            return ActorScreenPosition;
        }
        //SetWorldLocationAndRotation(TargetLocation + WidgetDefaultLocation, FRotator::ZeroRotator);
        //SetWorldLocation(TargetPlayer->GetActorLocation()+ WidgetDefaultLocation);
    }
    else // 화면 밖에 존재
    {
        //SetVisibility(true, true);
        if (ActorScreenPosition.X < StartViewX)
        {
            ActorScreenPosition.X = StartViewX + 10.0f;
        }
        else if (ActorScreenPosition.X > StartViewX + SizeViewX)
        {
            ActorScreenPosition.X = StartViewX + SizeViewX - 10.0f;
        }

        if (ActorScreenPosition.Y < StartViewY)
        {
            ActorScreenPosition.Y = StartViewY + 10.0f;
        }
        else if (ActorScreenPosition.Y > StartViewY + SizeViewY)
        {
            ActorScreenPosition.Y = StartViewY + SizeViewY - 10.0f;
        }
        if (MyTag == FName("Cody"))
        {
            ActorScreenPosition.X += SizeViewX / 2;
        }
        else
        {
            ActorScreenPosition.X -= SizeViewX / 2;
        }

        return ActorScreenPosition;

    }

    //APlayerController* FirstPlayer = GetWorld()->GetFirstPlayerController();
    //FVector2D ActorScreenPosition;
    //FVector TargetLocation = TargetPlayer->GetActorLocation();

    //if (!FirstPlayer->ProjectWorldLocationToScreen(TargetLocation, ActorScreenPosition))
    //{
    //    return FVector2D(-1, -1); // Failed to project actor's location to screen space
    //}

    //if ((ActorScreenPosition.X >= StartViewX && ActorScreenPosition.X <= StartViewX + SizeViewX &&
    //    ActorScreenPosition.Y >= StartViewY && ActorScreenPosition.Y <= StartViewY + SizeViewY)) // 화면 내에 존재
    //{
    //    float Distance = FVector::Distance(FirstPlayer->GetPawn()->GetActorLocation(), TargetLocation);

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
				//ActorScreenPosition.X += SizeViewX / 2;
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
    //    //FVector WorldLocation, WorldDirection;
    //    //FirstPlayer->DeprojectScreenPositionToWorld(ActorScreenPosition.X, ActorScreenPosition.Y, WorldLocation, WorldDirection);

    //    //// 카메라 스페이스 좌표로 변환한 후, 카메라와 WidgetComponent의 거리 계산
    //    //FVector CameraLocation = FirstPlayer->PlayerCameraManager->GetCameraLocation();
    //    //FVector WidgetLocation = CameraLocation + WorldDirection * 1200.0f; // 예시로 거리를 100.0f로 설정

    //    //SetWorldLocationAndRotation(WidgetLocation + WidgetDefaultLocation, FRotator::ZeroRotator);

    //    //SetWorldLocation(WidgetLocation + WidgetDefaultLocation);
    //}
}
