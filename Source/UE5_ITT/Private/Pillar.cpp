// Fill out your copyright notice in the Description page of Project Settings.


#include "Pillar.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
APillar::APillar()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarMesh"));
	RootComponent = PillarMesh;

	GlassMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GlassMesh"));
	
	GlassMesh->SetupAttachment(PillarMesh);

	CoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMesh"));
	CoreMesh->SetupAttachment(GlassMesh);
	
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(GlassMesh);

}

// Called when the game starts or when spawned
void APillar::BeginPlay()
{
	Super::BeginPlay();

	DefaultPos = GetActorLocation();

	//test code
	bShutterOpen = true;
}

// Called every frame
void APillar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	StateExcute(DeltaTime);	

}

void APillar::StateExcute(float DT)
{
	switch (CurState)
	{
	case APillar::EnumState::Close:
	{
		if (true == bShutterOpen)
		{
			ChangeState(EnumState::WaitMove);
		}
	}
	break;
	case APillar::EnumState::WaitMove:
	{
		PlayerWaitRatio += DT;
		if (PlayerWaitRatio >= 1.f)
		{
			PlayerWaitRatio = 1.f;
			ChangeState(EnumState::Wait);
		}

		SetActorLocation(FMath::Lerp(DefaultPos, DefaultPos + PlayerWaitSize, PlayerWaitRatio));
	}
	break;
	case APillar::EnumState::Wait:
	{
		if (true == bOnPlayer)
		{
			ChangeState(EnumState::MoveUp);
		}
	}
	break;
	case APillar::EnumState::MoveUp:
	{
		if (false == bOnPlayer)
		{
			ChangeState(EnumState::MoveDown);
			return;
		}

		MoveRatio += DT;
		if (MoveRatio >= 1.f)
		{
			MoveRatio = 1.f;
			ChangeState(EnumState::WaitBoom);
		}

		SetActorLocation(FMath::Lerp(DefaultPos + PlayerWaitSize, DefaultPos + MoveSize, MoveRatio));
	}
	break;
	case APillar::EnumState::WaitBoom:
	{
		//실드 내리기 추가필요
		if (true == bExplode)
		{
			ChangeState(EnumState::Boom);
			return;
		}
		if (false == bOnPlayer)
		{
			ChangeState(EnumState::MoveDown);
			return;
		}
	}
	break;
	case APillar::EnumState::MoveDown:
	{
		if (true == bOnPlayer)
		{
			ChangeState(EnumState::MoveUp);
			return;
		}

		MoveRatio -= DT;
		if (MoveRatio <= 0.f)
		{
			MoveRatio = 0.f;
			ChangeState(EnumState::WaitBoom);
		}

		SetActorLocation(FMath::Lerp(DefaultPos + PlayerWaitSize, DefaultPos + MoveSize, MoveRatio));
	}
	break;
	case APillar::EnumState::Boom:
	{

	}
	break;
	case APillar::EnumState::Done:
	{

	}
	break;
	case APillar::EnumState::None:
	{

	}
	break;
	}
}