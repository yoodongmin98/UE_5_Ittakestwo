// Fill out your copyright notice in the Description page of Project Settings.


#include "FxElectric.h"
#include "FsmComponent.h"
#include "NiagaraComponent.h"
#include "ITTGameModeBase.h"
#include "Components/BoxComponent.h"
#include "PlayerBase.h"
#include "GameManager.h"
#include "SoundManageComponent.h"

AFxElectric::AFxElectric(const FObjectInitializer& ObjectInitializer)
{
	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		bReplicates = true;
		SetReplicateMovement(true);
		SetupFsm(); 
		NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
		NiagaraComp->SetupAttachment(RootComponent);

		BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCol"));
		BoxCollision->SetupAttachment(NiagaraComp);

		SoundComp = CreateDefaultSubobject<USoundManageComponent>(TEXT("SoundComp"));
	}
}

void AFxElectric::NiagaraToggle_Implementation()
{
	NiagaraComp->ToggleVisibility();
}

void AFxElectric::BeginPlay()
{
	Super::BeginPlay();

	// ��Ʈ��ũ ������ Ȯ���ϴ� �ڵ�
	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::ClientWait);

		//���ΰ� ����
		NiagaraToggle();

		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AFxElectric::OnOverlapBegin);
		BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AFxElectric::OnOverlapEnd); 
		SoundComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
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
			if (true == Cast<UGameManager>(GetWorld()->GetGameInstance())->IsGameStart())
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
			SoundComp->MulticastSetAttenuationDistance(250.f, 300.f);
		},

		[this](float DT)
		{
			//������ �������ϴϱ� ������
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
				FsmComp->ChangeState(Fsm::End);
				return;
			}
		},

		[this]
		{
			ActiveTime -= 2.f;
		}
	);

	FsmComp->CreateState(Fsm::End,
		[this]
		{
			NiagaraToggle();
		},

		[this](float DT)
		{
			ActiveTime += DT;
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
			SoundComp->MulticastChangeSound(TEXT("Electric_Cue"));
		},

		[this](float DT)
		{
			ActiveTime += DT;
			if (ActiveTime >= 2.f)
			{
				FsmComp->ChangeState(Fsm::Active);
				return;
			}
		},

		[this]
		{
			ActiveTime -= 2.f;
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
