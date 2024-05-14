// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerIndicator.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API UPlayerIndicator : public UUserWidget
{
	GENERATED_BODY()
public:

    UFUNCTION(BlueprintCallable)
    void Setting(APawn* _Target, int32 _x, int32 _y, int32 _xs, int32 _ys);

    UFUNCTION(BlueprintCallable)
    void SetTagName(FString _Name);

    
    UFUNCTION(BlueprintCallable)
    void SetTarget(APawn* _Tar);

    UFUNCTION(BlueprintCallable)
    FVector2D GetRenderPosition();

    UPROPERTY(EditAnywhere)
    float DistanceThreshold = 2500.0f;

    

    //UPROPERTY(EditAnywhere)
    //FName TargetTag;
    UPROPERTY(EditAnywhere)
    FName MyTag;


protected:

private:

    APawn* TargetPlayer;
    int32 StartViewX, StartViewY;
    int32 SizeViewX, SizeViewY;
    
    void FindMyTag();

    void FindTarget();
};
