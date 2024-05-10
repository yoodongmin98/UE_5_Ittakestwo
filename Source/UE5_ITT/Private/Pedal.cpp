	// Fill out your copyright notice in the Description page of Project Settings.


#include "Pedal.h"
#include "PlayerBase.h"
#include "FsmComponent.h"
#include "EnemyMoonBaboon.h"

// Sets default values
APedal::APedal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (HasAuthority()== true)
	{
		SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
		SetRootComponent(SceneComp);

		SmasherMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SmasherMesh"));
		SmasherMesh->SetupAttachment(SceneComp);
		SetupFsm();
	}
}

void APedal::BeginPlay()
{
	Super::BeginPlay();
	FsmComp->ChangeState(Fsm::SmashWait);
}

void APedal::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));

	FsmComp->CreateState(Fsm::SmashWait,
		[this]
		{

		},

		[this](float DeltaTime)
		{
			if (FsmComp->GetStateLiveTime() > 1.f)
			{
				FsmComp->ChangeState(Fsm::Smash);
			}
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::Smash,
		[this]
		{
			SmashRatio = 0.f;
		},

		[this](float DeltaTime)
		{
			SmashRatio += DeltaTime * 10.f;
			if (SmashRatio >= 1.f)
			{
				SmashRatio = 1.f;
				SmasherMesh->SetRelativeRotation(FMath::Lerp(ReleaseRot, SmashRot, SmashRatio));
				FsmComp->ChangeState(Fsm::ReleaseWait);
				return;
			}
			SmasherMesh->SetRelativeRotation(FMath::Lerp(ReleaseRot, SmashRot, SmashRatio));
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::ReleaseWait,
		[this]
		{

		},

		[this](float DeltaTime)
		{
			if (FsmComp->GetStateLiveTime() > 1.f)
			{
				FsmComp->ChangeState(Fsm::Release);
			}
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::Release,
		[this]
		{
		},

		[this](float DeltaTime)
		{
			SmashRatio -= DeltaTime;
			if (SmashRatio <= 0.f)
			{
				SmashRatio = 0.f;
				SmasherMesh->SetRelativeRotation(FMath::Lerp(ReleaseRot, SmashRot, SmashRatio));
				FsmComp->ChangeState(Fsm::SmashWait);
				return;
			}
			SmasherMesh->SetRelativeRotation(FMath::Lerp(ReleaseRot, SmashRot, SmashRatio));
		},

		[this]
		{
		}
	);
}

void APedal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerBase* OverlapPlayer = Cast<APlayerBase>(OtherActor);
	if (OtherActor && (OtherActor != this) && OverlapPlayer != nullptr)
	{
		//OverlapPlayer-> 즉사처리
	}
}
// Called every frame
void APedal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority() == true)
	{

	}

}

