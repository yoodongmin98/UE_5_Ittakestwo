// Fill out your copyright notice in the Description page of Project Settings.


#include "MediumBox.h"
#include <Components/StaticMeshComponent.h>
#include "FsmComponent.h"

// Sets default values
AMediumBox::AMediumBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MediumBoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MediumBoxMesh"));
	SetupFsm();
}

// Called when the game starts or when spawned
void AMediumBox::BeginPlay()
{
	Super::BeginPlay();
	Random.GenerateNewSeed();

	FsmComp->ChangeState(Fsm::Wait);
	
}

void AMediumBox::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));

	FsmComp->CreateState(Fsm::Wait,
		[this]
		{

		},

		[this](float DeltaTime)
		{
			if (FsmComp->GetStateLiveTime()>1.f)
			{
				FsmComp->ChangeState(Fsm::Shake);
			}			
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::Shake,
		[this]
		{
			CurRotation = MediumBoxMesh->GetRelativeRotation();
		},

		[this](float DeltaTime)
		{
			//ShakeRange = Random.FRandRange(20.f,30.f);
			FRotator TempRot = { 0.f,0.f,FMath::Sin(FsmComp->GetStateLiveTime() * ShakeRange) };
			MediumBoxMesh->SetRelativeRotation(CurRotation+ TempRot);

			if (FsmComp->GetStateLiveTime() > 3.f)
			{
				FsmComp->ChangeState(Fsm::Rotate);
			}

		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::Rotate,
		[this]
		{
			RotateRatio = 0.f;
			FinalRotation = CurRotation + RotateSize;
		},

		[this](float DeltaTime)
		{
			RotateRatio += DeltaTime*10.f;
			if (RotateRatio >= 1.f)
			{
				RotateRatio = 1.f;
				MediumBoxMesh->SetRelativeRotation(FMath::Lerp(CurRotation, FinalRotation, RotateRatio));
				FsmComp->ChangeState(Fsm::Wait);
				return;
			}
			MediumBoxMesh->SetRelativeRotation(FMath::Lerp(CurRotation, FinalRotation, RotateRatio));
		},

		[this]
		{
		}
	);
}

// Called every frame
void AMediumBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

