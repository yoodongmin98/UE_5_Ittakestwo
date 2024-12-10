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

	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		bReplicates = true;
		SetReplicateMovement(true);
		SetupFsm();		
	}

}
	
// Called when the game starts or when spawned
void ACoreShutter::BeginPlay()
{
	Super::BeginPlay();

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::Close);
	}
}

// Called every frame
void ACoreShutter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACoreShutter::OpenShutter()
{
	bOpen = true;
}

void ACoreShutter::SetDone()
{
	FsmComp->ChangeState(Fsm::Done);
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
			bOpen = false;
		}
	);

	FsmComp->CreateState(Fsm::Opening,		
	[this]
		{
			//DefaultPos = GetActorLocation();
			//PivotPos = DefaultPos + OpenPos;
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
				SetActorRelativeLocation(FMath::Lerp(DefaultPos, OpenPos, MovingRatio));			
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
			MovingRatio -= DeltaTime * 1.f;
			if (MovingRatio <= 0.f)
			{
				MovingRatio = 0.f;
				FsmComp->ChangeState(Fsm::Close);
			}

			AddActorLocalRotation({ 0.f,-RotateSize * DeltaTime * 1.f,0.f });
			SetActorRelativeLocation(FMath::Lerp(DefaultPos, OpenPos, MovingRatio));
		},

	[this]
		{
		} 
		);

}

