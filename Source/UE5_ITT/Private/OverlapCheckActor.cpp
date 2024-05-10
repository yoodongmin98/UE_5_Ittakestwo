// Fill out your copyright notice in the Description page of Project Settings.


#include "OverlapCheckActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "PlayerBase.h"

// Sets default values
AOverlapCheckActor::AOverlapCheckActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);
		StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Comp"));
		SetRootComponent(StaticMeshComp);
		StaticMeshComp->SetVisibility(false);
	}
}

// Called when the game starts or when spawned
void AOverlapCheckActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (true == HasAuthority())
	{
		GetRootComponent()->SetWorldScale3D(FVector(250.0f, 250.0f, 250.0f));
		SetupOverlapEvent();
	}
}

// Called every frame
void AOverlapCheckActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
	}
}

void AOverlapCheckActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (nullptr != OtherActor)
	{
		// player 태그를 가진 액터가 오버랩 되어있다면 true
		if (true == OtherActor->ActorHasTag(OverlapActorTag))
		{
			bIsOverlapping = true;
			CurrentOverlapPlayer = Cast<APlayerBase>(OtherActor);
		}
	}
}

void AOverlapCheckActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (nullptr != OtherActor)
	{
		if (true == OtherActor->ActorHasTag(OverlapActorTag))
		{
			bIsOverlapping = false;
			CurrentOverlapPlayer = nullptr;
		}
	}
}

void AOverlapCheckActor::SetupOverlapEvent()
{
	StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AOverlapCheckActor::OnOverlapBegin);
	StaticMeshComp->OnComponentEndOverlap.AddDynamic(this, &AOverlapCheckActor::OnOverlapEnd);
}

