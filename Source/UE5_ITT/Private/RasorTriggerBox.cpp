// Fill out your copyright notice in the Description page of Project Settings.


#include "RasorTriggerBox.h"

// Sets default values
ARasorTriggerBox::ARasorTriggerBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    // Create trigger volume
    TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
    RootComponent = TriggerVolume;

    // Set collision profile and size
    TriggerVolume->SetCollisionProfileName("Trigger");
    TriggerVolume->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));

    // Bind overlap events
    TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ARasorTriggerBox::OnOverlapBegin);
    TriggerVolume->OnComponentEndOverlap.AddDynamic(this, &ARasorTriggerBox::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ARasorTriggerBox::BeginPlay()
{
	Super::BeginPlay();

    StartPos = GetActorLocation();
    DestPos = StartPos + DestFar;
    UnitTime = 0.0f;
    bIsTouch = false;
	
}

// Called every frame
void ARasorTriggerBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (true == bIsTouch)
    {
        //print("Overlap Begin");

        UnitTime += DeltaTime * 0.5f;
    }
    else
    {
        UnitTime -= DeltaTime * 0.5f;
        //print("Overlap Ended");

    }

    if (UnitTime < 0)
    {
        UnitTime = 0.0f;
    }
    else if (UnitTime > 1.0f)
    {
        UnitTime = 1.0f;
    }

    FVector NextPos = FMath::Lerp(StartPos, DestPos, UnitTime);
    SetActorLocation(NextPos);
}

void ARasorTriggerBox::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Code to execute when overlap starts
    if (OtherActor && (OtherActor != this)) {
		UE_LOG(LogTemp, Warning, TEXT("Overlap started with actor: %s"), *OtherActor->GetName());
        // print to screen using above defined method when actor enters trigger box
        //print("Overlap Begin");
        //printFString("Overlapped Actor = %s", *OtherActor->GetName());
        bIsTouch = true;
    }
}

void ARasorTriggerBox::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // Code to execute when overlap ends
    if (OtherActor && (OtherActor != this)) {
		UE_LOG(LogTemp, Warning, TEXT("Overlap ended with actor: %s"), *OtherComp->GetName());
        // print to screen using above defined method when actor leaves trigger box
        //print("Overlap Ended");
        //printFString("%s has left the Trigger Box", *OtherComp->GetName());
        bIsTouch = false;
    }
}
