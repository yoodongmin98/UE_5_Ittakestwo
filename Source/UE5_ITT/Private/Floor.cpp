// Fill out your copyright notice in the Description page of Project Settings.


#include "Floor.h"
#include "ParentShutter.h"
#include "Pillar.h"
#include "FsmComponent.h"

// Sets default values
AFloor::AFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetupFsm();
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

void AFloor::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));

	FsmComp->CreateState(Fsm::Phase1_1,		
		[this]
		{
			ParentShutter1->SetShutterOpen(true);
		},

		[this](float DeltaTime)
		{
			PillarSecond += DeltaTime;
			if (PillarSecond >= 1.f)
			{
				Pillar1->ChangeState(APillar::Fsm::WaitMove);
				FsmComp->ChangeState(Fsm::Phase1_1Wait);
			}
		},

		[this]
		{
			PillarSecond = 0.f;
		}
		);

	FsmComp->CreateState(Fsm::Phase1_1Wait,
		[this]
		{
			MoveRatio = 0.f;
			CurPos = GetActorLocation();
			NextPos = CurPos;
			NextPos.Z += MoveSize;
		},

		[this](float DT)
		{
			if (true == Pillar1->IsDone())
			{
				MoveRatio += DT/MoveTime;
				if (MoveRatio >= 1.f)
				{
					MoveRatio = 1.f;
					FsmComp->ChangeState(Fsm::Phase1_2);
				}

				SetActorLocation(FMath::Lerp(CurPos, NextPos, MoveRatio));
			}
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::Phase1_2,
		[this]
		{
			ParentShutter2->SetShutterOpen(true);
		},

		[this](float DeltaTime)
		{
			PillarSecond += DeltaTime;
			if (PillarSecond >= 1.f)
			{
				Pillar2->ChangeState(APillar::Fsm::WaitMove);
				FsmComp->ChangeState(Fsm::Phase1_2Wait);
			}
		},

		[this]
		{
			PillarSecond = 0.f;
		}
	);

	FsmComp->CreateState(Fsm::Phase1_2Wait,
		[this]
		{
			MoveRatio = 0.f;
			CurPos = GetActorLocation();
			NextPos = CurPos;
			NextPos.Z += MoveSize;
		},

		[this](float DT)
		{
			if (true == Pillar2->IsDone())
			{
				MoveRatio += DT / MoveTime;
				if (MoveRatio >= 1.f)
				{
					MoveRatio = 1.f;
					FsmComp->ChangeState(Fsm::Phase1_3);
				}

				SetActorLocation(FMath::Lerp(CurPos, NextPos, MoveRatio));
			}
		},

		[this]
		{
		}
	);


	FsmComp->CreateState(Fsm::Phase1_3,
		[this]
		{
			ParentShutter0->SetShutterOpen(true);
		},

		[this](float DeltaTime)
		{
			PillarSecond += DeltaTime;
			if (PillarSecond >= 1.f)
			{
				Pillar0->ChangeState(APillar::Fsm::WaitMove);
				FsmComp->ChangeState(Fsm::Phase1_3Wait);
			}
		},

		[this]
		{
			PillarSecond = 0.f;
		}
	);

	FsmComp->CreateState(Fsm::Phase1_3Wait,
		[this]
		{
			MoveRatio = 0.f;
			CurPos = GetActorLocation();
			NextPos = CurPos;
			NextPos.Z += MoveSize;
		},

		[this](float DT)
		{
			if (true == Pillar0->IsDone())
			{
				FsmComp->ChangeState(Fsm::Phase2);				
			}
		},

		[this]
		{
		}
	);


	FsmComp->CreateState(Fsm::Phase2,
		[this]
		{
		},

		[this](float DT)
		{
		},

		[this]
		{
		}
	);
}
