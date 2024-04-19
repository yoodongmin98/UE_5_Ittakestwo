// Fill out your copyright notice in the Description page of Project Settings.


#include "FxElectric.h"
#include "FsmComponent.h"
#include "NiagaraComponent.h"


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
		FsmComp->ChangeState(Fsm::None);
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

	FsmComp->CreateState(Fsm::None,
		[this]
		{
		},

		[this](float DT)
		{
			
			if (GetWorld()->GetTimeSeconds() > 3.f)
			{
				UE_LOG(LogTemp, Display, TEXT("StartEffect"));
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
			if (true == bDelay)
			{
				if (FsmComp->GetStateLiveTime()>3.f)
				{
					FsmComp->ChangeState(Fsm::End);
				}
			}
			else
			{
				FsmComp->ChangeState(Fsm::End);
			}
		},

		[this]
		{

		}
	);
	FsmComp->CreateState(Fsm::End,
		[this]
		{
			NiagaraToggle();
		},

		[this](float DT)
		{
		},

		[this]
		{

		}
	);
}
