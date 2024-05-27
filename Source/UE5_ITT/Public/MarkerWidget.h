// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "PlayerBase.h"
#include "MarkerWidget.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = Custom, meta = (BlueprintSpawnableComponent))
class UE5_ITT_API UMarkerWidget : public UWidgetComponent
{
	GENERATED_BODY()
public:
    // Constructor
    UMarkerWidget(const FObjectInitializer& ObjectInitializer);

    // Æ½ ÇÔ¼ö ¼±¾ð
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


    UFUNCTION(BlueprintCallable)
    inline float GetDistanceFromTarget() const
    {
        return Distance;
    }
    // The widget to display
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> UIWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    float DistanceThreshold;



protected:

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    UUserWidget* MarkerInstance;
private:

    // The widget instance 

    bool bChecked;
    void SetIsCody();
    bool bIsCody;
    float Distance;
    int32 ViewportSizex, ViewportSizey;

    class APlayerBase* TargetActor;
    void FindTargetActor();

    UPROPERTY(Replicated)
    FVector2D RenderPosition;

    UPROPERTY(Replicated)
    bool bIsRender;
    FVector2D GetRenderPosition();

    //APlayerBase* MyOwner;

    APlayerController* MyViewController;

    FDateTime SpawnTime;

    //UFUNCTION(NetMulticast, Reliable, WithValidation)
    //void SetCodyWidget(const FVector2D _Pos, const bool isvisible);
};
