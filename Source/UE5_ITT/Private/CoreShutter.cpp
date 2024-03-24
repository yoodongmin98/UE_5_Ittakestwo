// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreShutter.h"
#include <Components/StaticMeshComponent.h>

#include "ParentShutter.h"
#include "FsmComponent.h"

// Sets default values
ACoreShutter::ACoreShutter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetupFsm();
}
	
// Called when the game starts or when spawned
void ACoreShutter::BeginPlay()
{
	Super::BeginPlay();

	FsmComp->ChangeState(Fsm::Close);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

// Called every frame
void ACoreShutter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACoreShutter::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));

	FsmComp->CreateState(Fsm::Close,
		[this]
		{

		},

		[this](float DeltaTime)
		{
			if (true == bOpen)
			{
				FsmComp->ChangeState(Fsm::Opening);
			}
		},

		[this]
		{

		}
	);

	FsmComp->CreateState(Fsm::Opening,		
	[this]
		{
			DefaultPos = GetActorLocation();
			PivotPos = DefaultPos + OpenPos;
		},
	[this](float DeltaTime)
		{
				MovingRatio += DeltaTime * 0.5f;

				if (MovingRatio >= 1.f)
				{
					FsmComp->ChangeState(Fsm::Open);
					MovingRatio = 1.f;
				}

				AddActorLocalRotation({ 0.f,RotateSize * DeltaTime * 0.5f,0.f });
				SetActorLocation(FMath::Lerp(DefaultPos, PivotPos, MovingRatio));
			
		},
	[this]
		{

		}
		);


	FsmComp->CreateState(Fsm::Open,
		[this]
		{

		},

	[this](float DeltaTime)
		{
			if (true == bDone)
			{
				FsmComp->ChangeState(Fsm::Done);
			}
		},

	[this]
		{

		} 
		);

	FsmComp->CreateState(Fsm::Done,		
	[this]
		{

		},

	[this](float DeltaTime)
		{
			MovingRatio -= DeltaTime * 0.5f;
			if (MovingRatio <= 0.f)
			{
				MovingRatio = 0.f;
			}

			AddActorLocalRotation({ 0.f,-RotateSize * DeltaTime * 0.5f,0.f });
			SetActorLocation(PivotPos + FMath::Lerp(DefaultPos, OpenPos, MovingRatio));
		},

	[this]
		{

		} 
		);

}

