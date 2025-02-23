// Fill out your copyright notice in the Description page of Project Settings.


#include "FxElectric.h"
#include "FsmComponent.h"
#include "NiagaraComponent.h"
#include "ITTGameModeBase.h"
#include "Components/BoxComponent.h"
#include "PlayerBase.h"

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

		BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCol"));
		BoxCollision->SetupAttachment(NiagaraComp);
	}
}

void AFxElectric::NiagaraToggle_Implementation()
{
	NiagaraComp->ToggleVisibility();
}

void AFxElectric::BeginPlay()
{
	Super::BeginPlay();

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::ClientWait);

		//꺼두고 시작
		NiagaraToggle();

		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AFxElectric::OnOverlapBegin);
		BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AFxElectric::OnOverlapEnd);
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
			//클라 접속 대기
			if (GetWorld()->GetAuthGameMode()->GetNumPlayers() == 2)
			{
				ClientWaitTime += DeltaTime;
			}
			if (ClientWaitTime >= 1.f)
			{
				FsmComp->ChangeState(Fsm::Delay);
				return;
			}
		},

		[this]
		{
		}
	);

	FsmComp->CreateState(Fsm::Delay,
		[this]
		{
		},

		[this](float DT)
		{
			//번갈아 켜져야하니까 딜레이
			if (true == bDelay)
			{
				if (FsmComp->GetStateLiveTime()>=3.f)
				{
					FsmComp->ChangeState(Fsm::Active);
					return;
				}
			}
			else
			{
				FsmComp->ChangeState(Fsm::Active);
				return;
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
			ActiveTime += DT;
			if (ColPlayer!=nullptr)
			{
				ColPlayer->AttackPlayer(12);
			}
			if (ActiveTime >= 2.f)
			{
				FsmComp->ChangeState(Fsm::Wait);
				return;
			}
		},

		[this]
		{
			ActiveTime -= 2.f;
		}
	);

	FsmComp->CreateState(Fsm::Wait,
		[this]
		{
			NiagaraToggle();
		},

		[this](float DT)
		{
			ActiveTime += DT;
			if (ActiveTime >= 4.f)
			{
				FsmComp->ChangeState(Fsm::Active);
				return;
			}
		},

		[this]
		{
			ActiveTime -= 4.f;
		}
	);
}

void AFxElectric::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherActor->ActorHasTag("Player") == true)
	{
		APlayerBase* OverlapPlayer = Cast<APlayerBase>(OtherActor);
		ColPlayer = OverlapPlayer;
	}
}

void AFxElectric::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && ColPlayer && (OtherActor == ColPlayer))
	{
		ColPlayer = nullptr;
	}
}
