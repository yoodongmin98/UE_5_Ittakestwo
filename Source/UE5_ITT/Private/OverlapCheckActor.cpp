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

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Comp"));
	SetRootComponent(StaticMeshComp);
	StaticMeshComp->SetVisibility(false);
}

// Called when the game starts or when spawned
void AOverlapCheckActor::BeginPlay()
{
	Super::BeginPlay();

	StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AOverlapCheckActor::OnOverlapBegin);
	StaticMeshComp->OnComponentEndOverlap.AddDynamic(this, &AOverlapCheckActor::OnOverlapEnd);


	GetRootComponent()->SetWorldScale3D(FVector(250.0f, 250.0f, 250.0f));
	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		SetReplicates(true);
		SetReplicateMovement(true);
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
		// player 태그를 가진 액터가 오버랩 되어있다면 true
		if (true == OtherActor->ActorHasTag(OverlapActorTag))
		{
			bIsOverlapping = false;
			CurrentOverlapPlayer = nullptr;
		}
	}

}

// Called every frame
void AOverlapCheckActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		float SphereRadius = 100.0f;
		int32 Segments = 12;
		float LifeTime = 0.1f;
		float Thickness = 2.0f;

		FVector Scale = GetRootComponent()->GetComponentScale();
		DrawDebugBox(GetWorld(), GetActorLocation(), Scale, FColor::Green, false, LifeTime, 0);
	}

	/*DrawDebugSphere(
		GetWorld(),
		GetComponentLocation(),
		SphereRadius,
		Segments,
		FColor::Red,
		false,
		LifeTime,
		0,
		Thickness
	);*/
}

