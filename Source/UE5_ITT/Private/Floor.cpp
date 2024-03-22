// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"
#include "ParentShutter.h"
#include "Pillar.h"

// Sets default values
AFloor::AFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFloor::BeginPlay()
{
	Super::BeginPlay();

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

// Called every frame
void AFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PhaseExcute(DeltaTime);
}

void AFloor::PhaseExcute(float DeltaTime)
{
	switch (CurPhase)
	{
	case AFloor::EPhase::Phase1_1:
	{
		ParentShutter1->SetShutterOpen(true);
	}
	break;
	case AFloor::EPhase::Phase1_2:
	{
		ParentShutter2->SetShutterOpen(true);
	}
	break;
	case AFloor::EPhase::Phase1_3:
	{
		ParentShutter0->SetShutterOpen(true);
	}
	break;
	case AFloor::EPhase::Phase2:
	{

	}
	break;
	case AFloor::EPhase::Phase3:
	{

	}
	break;
	case AFloor::EPhase::None:
	{

	}
	break;
	}
}
