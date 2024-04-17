// Fill out your copyright notice in the Description page of Project Settings.


#include "Laser.h"
#include "FsmComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ALaser::ALaser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		bReplicates = true;
		SetReplicateMovement(true);
	}

	LaserMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserMesh"));

	SetupFsm();

}

// Called when the game starts or when spawned
void ALaser::BeginPlay()
{
	Super::BeginPlay();

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::Wait);

		bPhaseEnd = true;

		DefaultPos = LaserMesh->GetRelativeLocation();
		AttackPos = DefaultPos;
		AttackPos.Z += AttackMoveSize;
	}
}

void ALaser::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));
	FsmComp->CreateState(Fsm::Wait,
		[this]
		{

		},

		[this](float DT)
		{
			if (true == bAttackStart)
			{
				FsmComp->ChangeState(Fsm::MoveUp);
			}
		},

		[this]
		{
		});

	FsmComp->CreateState(Fsm::MoveUp,
		[this]
		{
		},

		[this](float DT)
		{
			MovingRatio += DT;

			if (MovingRatio >= 1.f)
			{
				MovingRatio = 1.f;
				FsmComp->ChangeState(Fsm::LaserOn);
			}

			SetActorRelativeLocation(FMath::Lerp(DefaultPos, AttackPos, MovingRatio));
		},

		[this]
		{
		});

	FsmComp->CreateState(Fsm::LaserOn,
		[this]
		{

		},

		[this](float DT)
		{
			FsmComp->ChangeState(Fsm::Attack);
		},

		[this]
		{
		});

	FsmComp->CreateState(Fsm::Attack,
		[this]
		{

		},

		[this](float DT)
		{
			if (false == bAttackStart)
			{
				FsmComp->ChangeState(Fsm::LaserOff);
			}

			RotateTime -= DT;

			AddActorLocalRotation({ 0.f,360.f * DT,0.f });
		},

		[this]
		{
			RotateTime = 15.f;
		});

	FsmComp->CreateState(Fsm::LaserOff,
		[this]
		{

		},

		[this](float DT)
		{
			FsmComp->ChangeState(Fsm::MoveDown);
		},

		[this]
		{
		});

	FsmComp->CreateState(Fsm::MoveDown,
		[this]
		{

		},

		[this](float DT)
		{
			MovingRatio -= DT;

			if (MovingRatio <= 0.f)
			{
				MovingRatio = 0.f;
				FsmComp->ChangeState(Fsm::Wait);
			}

			SetActorRelativeLocation(FMath::Lerp(DefaultPos, AttackPos, MovingRatio));
		},

		[this]
		{
		});

}

// Called every frame
void ALaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

