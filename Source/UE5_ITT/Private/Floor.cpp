// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"
#include <Engine/StaticMeshActor.h>
#include "Pillar.h"
#include "FsmComponent.h"
#include "Laser.h"

// Sets default values
AFloor::AFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetupFsm();

	Tags.Add(FName("Floor"));
}

// Called when the game starts or when spawned
void AFloor::BeginPlay()
{
	Super::BeginPlay();

	FsmComp->ChangeState(Fsm::Phase1_1);
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
}

void AFloor::SetPhase(Fsm Phase)
{	
	FsmComp->ChangeState(Phase);	
}

int32 AFloor::GetCurrentPhase() const
{
	if (nullptr == FsmComp)
	{
		return -1;
	}

	return FsmComp->GetCurrentState();
}

void AFloor::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));


	FsmComp->CreateState(Fsm::Phase1_1,
		[this]
		{
			Pillar1->ShutterOpen();
		},

		[this](float DT)
		{
			if (true == Pillar1->IsDone())
			{
				FsmComp->ChangeState(Fsm::Phase1_1Attack);
			}
		},

		[this]
		{
		}
	);
	FsmComp->CreateState(Fsm::Phase1_1Attack,
		[this]
		{
			MoveRatio = 0.f;
			CurPos = GetActorLocation();
			NextPos = CurPos;
			NextPos.Z += MoveSize;
			MainLaser->SetAttack(true);
		},

		[this](float DT)
		{

			MoveRatio += DT / MoveTime;
			if (MoveRatio >= 1.f)
			{
				MoveRatio = 1.f;
				MainLaser->SetAttack(false);
				FsmComp->ChangeState(Fsm::Phase1_2);
			}

			SetActorLocation(FMath::Lerp(CurPos, NextPos, MoveRatio));
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::Phase1_2,
		[this]
		{
			Pillar2->ShutterOpen();
		},

		[this](float DT)
		{
			if (true == Pillar2->IsDone())
			{
				FsmComp->ChangeState(Fsm::Phase1_2Attack);
			}
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::Phase1_2Attack,
		[this]
		{
			MoveRatio = 0.f;
			CurPos = GetActorLocation();
			NextPos = CurPos;
			NextPos.Z += MoveSize;
			MainLaser->SetAttack(true);
		},

		[this](float DT)
		{	

			MoveRatio += DT / MoveTime;
			if (MoveRatio >= 1.f)
			{
				MoveRatio = 1.f;
				MainLaser->SetAttack(false);
				FsmComp->ChangeState(Fsm::Phase1_3);
			}

			SetActorLocation(FMath::Lerp(CurPos, NextPos, MoveRatio));
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::Phase1_3,
		[this]
		{
			Pillar0->ShutterOpen();
			Pillar0->SetLongPillar();
			MoveRatio = 0.f;
			CurPos = GetActorLocation();
			NextPos = CurPos;
			NextPos.Z += MoveSize;
		},

		[this](float DT)
		{
			if (true == Pillar0->IsDone())
			{
				MoveRatio += DT / MoveTime;
				if (MoveRatio >= 1.f)
				{
					MoveRatio = 1.f;
					FsmComp->ChangeState(Fsm::Phase1End);
				}
			}
		},

		[this]
		{
		}
	);
		

	FsmComp->CreateState(Fsm::Phase1End,
		[this]
		{
		},

		[this](float DT)
		{
			//컷씬을 하고 컷씬이 끝나면 문닫기
			//LeftDoor->SetActorLocation();
			//RightDoor->SetActorLocation();
		},

		[this]
		{
		}
	);
}
