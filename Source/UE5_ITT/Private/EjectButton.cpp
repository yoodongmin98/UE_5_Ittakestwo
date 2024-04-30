// Fill out your copyright notice in the Description page of Project Settings.


#include "EjectButton.h"
#include "Components/StaticMeshComponent.h"
#include "FsmComponent.h"
#include "Cody.h"

// Sets default values
AEjectButton::AEjectButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		bReplicates = true;
		SetReplicateMovement(true);
		SetupFsm();

		SM_BaseComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_BaseComp"));
		SetRootComponent(SM_BaseComp);
		SM_PushComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_PushComp"));
		SM_PushComp->SetupAttachment(SM_BaseComp);
		SM_CollisionComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_CollisionComp"));
		SM_CollisionComp->SetupAttachment(SM_PushComp);
	}
}

// Called when the game starts or when spawned
void AEjectButton::BeginPlay()
{
	Super::BeginPlay();
	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::OnWait);
		SM_CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AEjectButton::OnOverlapBegin);
		SM_CollisionComp->OnComponentEndOverlap.AddDynamic(this, &AEjectButton::OnOverlapEnd);
		PlayerWaitPos = SM_PushComp->GetRelativeLocation();
		MovePos = PlayerWaitPos;
		MovePos.Z -= 30.f;
		PushPos = PlayerWaitPos;
		PushPos.Z -= 90.f;
	}
}

void AEjectButton::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));

	FsmComp->CreateState(Fsm::OnWait,
		[this]
		{

		},

		[this](float DeltaTime)
		{
			if (bPush == true)
			{
				FsmComp->ChangeState(Fsm::Push);
				return;
			}
			if (bOnPlayer == true)
			{
				FsmComp->ChangeState(Fsm::On);
				return;
			}

		},

		[this]
		{
		}
	);
	FsmComp->CreateState(Fsm::OffWait,
		[this]
		{

		},

		[this](float DeltaTime)
		{
			if (bPush == true)
			{
				FsmComp->ChangeState(Fsm::Push);
				return;
			}
			if (bOnPlayer == false)
			{
				FsmComp->ChangeState(Fsm::Off);
				return;
			}

		},

		[this]
		{
		}
	);
	FsmComp->CreateState(Fsm::Off,
		[this]
		{

		},

		[this](float DeltaTime)
		{
			if (bPush == true)
			{
				FsmComp->ChangeState(Fsm::Push);
				return;
			}
			if (true == bOnPlayer)
			{
				FsmComp->ChangeState(Fsm::On);
				return;
			}

			MoveRatio -= DeltaTime*10.f;
			if (MoveRatio <= 0.f)
			{
				MoveRatio = 0.f;
				SM_PushComp->SetRelativeLocation(FMath::Lerp(PlayerWaitPos, MovePos, MoveRatio));
				FsmComp->ChangeState(Fsm::OnWait);
				return;
			}

			SM_PushComp->SetRelativeLocation(FMath::Lerp(PlayerWaitPos, MovePos, MoveRatio));
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::On,
		[this]
		{

		},

		[this](float DeltaTime)
		{
			if (bPush == true)
			{
				FsmComp->ChangeState(Fsm::Push);
				return;
			}

			if (false == bOnPlayer)
			{
				FsmComp->ChangeState(Fsm::Off);
				return;
			}


			MoveRatio += DeltaTime * 10.f;
			if (MoveRatio >= 1.f)
			{
				MoveRatio = 1.f;
				SM_PushComp->SetRelativeLocation(FMath::Lerp(PlayerWaitPos, MovePos, MoveRatio));
				FsmComp->ChangeState(Fsm::OffWait);
				return;
			}

			SM_PushComp->SetRelativeLocation(FMath::Lerp(PlayerWaitPos, MovePos, MoveRatio));
		},

		[this]
		{
		}
	);
	FsmComp->CreateState(Fsm::Push,
		[this]
		{
		},

		[this](float DeltaTime)
		{
			MoveRatio += DeltaTime * 10.f;
			if (MoveRatio >= 1.f)
			{
				MoveRatio = 1.f;
				SM_PushComp->SetRelativeLocation(FMath::Lerp(PlayerWaitPos, PushPos, MoveRatio));
				FsmComp->ChangeState(Fsm::End);
				return;
			}

			SM_PushComp->SetRelativeLocation(FMath::Lerp(PlayerWaitPos, PushPos, MoveRatio));
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::End,
		[this]
		{
		},

		[this](float DeltaTime)
		{
			UE_LOG(LogTemp, Display, TEXT("End"));
		},

		[this]
		{
		}
	);
}

void AEjectButton::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("OverlapBegin"));
	ACody* Cody = Cast<ACody>(OtherActor);
	if (Cody!=nullptr)
	{
		if (Cody->GetIsSit())
		{
			bPush = true;
		}
		else
		{
			bOnPlayer = true;
		}
	}
}

void AEjectButton::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bOnPlayer = false;
}

// Called every frame
void AEjectButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
	