// Fill out your copyright notice in the Description page of Project Settings.


#include "FxElectric.h"
#include "FsmComponent.h"
#include "NiagaraComponent.h"
#include "ITTGameModeBase.h"

AFxElectric::AFxElectric(const FObjectInitializer& ObjectInitializer)
{
	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		bReplicates = true;
		SetReplicateMovement(true);
		SetupFsm(); 
		NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
		NiagaraComp->SetupAttachment(RootComponent);
	}
}

void AFxElectric::NiagaraToggle_Implementation()
{
	NiagaraComp->ToggleActive();
}

void AFxElectric::BeginPlay()
{
	Super::BeginPlay();

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::ClientWait);
	}
}

// Called every frame
void AFxElectric::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFxElectric::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));

	FsmComp->CreateState(Fsm::ClientWait,
		[this]
		{

		},

		[this](float DeltaTime)
		{
			if (GetWorld()->GetAuthGameMode()->GetNumPlayers() == 2)
			{
				ClientWaitTime += DeltaTime;
			}
			if (ClientWaitTime > 1.f)
			{
				FsmComp->ChangeState(Fsm::Delay);
			}
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::Delay,
		[this]
		{
			NiagaraToggle();
		},

		[this](float DT)
		{
			if (true == bDelay)
			{
				if (FsmComp->GetStateLiveTime()>=3.f)
				{
					FsmComp->ChangeState(Fsm::Active);
				}
			}
			else
			{
				FsmComp->ChangeState(Fsm::Active);
			}
		},

		[this]
		{

		}
	);

	FsmComp->CreateState(Fsm::Active,
		[this]
		{
			NiagaraToggle();
		},

		[this](float DT)
		{
			if (FsmComp->GetStateLiveTime() >= 2.f)
			{
				FsmComp->ChangeState(Fsm::Wait);
			}
		},

		[this]
		{

		}
	);

	FsmComp->CreateState(Fsm::Wait,
		[this]
		{
			NiagaraToggle();
		},

		[this](float DT)
		{
			if (FsmComp->GetStateLiveTime() >= 4.f)
			{
				FsmComp->ChangeState(Fsm::Active);
			}
		},

		[this]
		{

		}
	);
}
