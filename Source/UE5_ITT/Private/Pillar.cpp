// Fill out your copyright notice in the Description page of Project Settings.


#include "Pillar.h"
#include "Components/StaticMeshComponent.h"
#include "FsmComponent.h"
#include "ParentShutter.h"
#include "EnergyCore.h"

// Sets default values
APillar::APillar()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarMesh"));
	RootComponent = PillarMesh;
	
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(PillarMesh);

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetupAttachment(PillarMesh);


	SetupFsm();
}

// Called when the game starts or when spawned
void APillar::BeginPlay()
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
void APillar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void APillar::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));
	FsmComp->CreateState(Fsm::Close,
		[this]
		{

		},

		[this](float DT)
		{
			if (bShutterOpen == true)
			{
				FsmComp->ChangeState(Fsm::ShutterOpen);
			}
		},

		[this]
		{
		});

	FsmComp->CreateState(Fsm::ShutterOpen,
		[this]
		{
			ParentShutter->SetShutterOpen();
		},

		[this](float DT)
		{
			if (FsmComp->GetStateLiveTime()>2.f)
			{
				FsmComp->ChangeState(Fsm::WaitMove);
			}
		},

		[this]
		{

		});

	FsmComp->CreateState(Fsm::WaitMove,
		[this]
		{
			DefaultPos = GetActorLocation();

			PlayerWaitPos = DefaultPos;
			PlayerWaitPos.Z += PlayerWaitSize;

			MovePos = DefaultPos;
			MovePos.Z += MoveSize;

			ShieldDefaultPos = ShieldMesh->GetRelativeLocation();
			ShieldOpenPos = ShieldDefaultPos;
			ShieldOpenPos.Z -= ShieldOpenSize;
		},

		[this](float DT)
		{
			PlayerWaitRatio += DT;
			if (PlayerWaitRatio >= 1.f)
			{
				PlayerWaitRatio = 1.f;
				FsmComp->ChangeState(Fsm::Wait);
				ButtonMesh->OnComponentBeginOverlap.AddDynamic(this, &APillar::OnOverlapBegin);
				ButtonMesh->OnComponentEndOverlap.AddDynamic(this, &APillar::OnOverlapEnd);
			}

			SetActorLocation(FMath::Lerp(DefaultPos, PlayerWaitPos, PlayerWaitRatio));
		},

		[this]
		{

		});

	FsmComp->CreateState(Fsm::Wait,
		[this]
		{

		},

		[this](float DT)
		{
			if (true == bOnPlayer)
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
			if (false == bOnPlayer)
			{
				bShieldOpen = true;
				FsmComp->ChangeState(Fsm::MoveDown);
				return;
			}

			MoveRatio += DT;
			if (MoveRatio >= 1.f)
			{
				MoveRatio = 1.f;
				FsmComp->ChangeState(Fsm::WaitBoom);
			}

			SetActorLocation(FMath::Lerp(PlayerWaitPos, MovePos, MoveRatio));
		},

		[this]
		{

		});

	FsmComp->CreateState(Fsm::WaitBoom,
		[this]
		{

		},

		[this](float DT)
		{
			if (false == bOnPlayer)
			{
				bShieldOpen = true;
				FsmComp->ChangeState(Fsm::MoveDown);
				return;
			}

			if (false == bShieldOpen)
			{
				ShieldOpenRatio += DT;
				if (ShieldOpenRatio >= 1.f)
				{
					ShieldOpenRatio = 1.f;
					bShieldOpen = true;
				}
				ShieldMesh->SetRelativeLocation(FMath::Lerp(ShieldDefaultPos, ShieldOpenPos, ShieldOpenRatio));
			}

			if (true == EnergyCoreActor->IsExplode())
			{
				//레이저 타격 체크 필요
				FsmComp->ChangeState(Fsm::Boom);
				return;
			}
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

			if (true == bOnPlayer)
			{
				FsmComp->ChangeState(Fsm::MoveUp);
				return;
			}

			if (true == bShieldOpen)
			{
				ShieldOpenRatio -= DT * 3.f;
				if (ShieldOpenRatio <= 0.f)
				{
					ShieldOpenRatio = 0.f;
					bShieldOpen = false;
				}
				ShieldMesh->SetRelativeLocation(FMath::Lerp(ShieldDefaultPos, ShieldOpenPos, ShieldOpenRatio));
			}

			MoveRatio -= DT;
			if (MoveRatio <= 0.f)
			{
				MoveRatio = 0.f;
				FsmComp->ChangeState(Fsm::Wait);
			}

			SetActorLocation(FMath::Lerp(PlayerWaitPos, MovePos, MoveRatio));
		},

		[this]
		{

		});

	FsmComp->CreateState(Fsm::Boom,
		[this]
		{

		},

		[this](float DT)
		{
			FsmComp->ChangeState(Fsm::Done);
		},

		[this]
		{

		});


	FsmComp->CreateState(Fsm::ShutterClose,
		[this]
		{
			ParentShutter->SetDone();
		},

		[this](float DT)
		{
			if (FsmComp->GetStateLiveTime() > 2.f)
			{
				FsmComp->ChangeState(Fsm::Close);
			}
		},

		[this]
		{
			bShutterOpen = false;
			bDone = true;
		});

	FsmComp->CreateState(Fsm::Done,
		[this]
		{

		},

		[this](float DT)
		{
			//폭발 완료되면 전부 내리고 close으로 변경하기
			MoveRatio -= DT;
			if (MoveRatio <= 0.f)
			{
				MoveRatio = 0.f;
				FsmComp->ChangeState(Fsm::ShutterClose);
			}

			SetActorLocation(FMath::Lerp(DefaultPos, MovePos, MoveRatio));
		},

		[this]
		{
		});
}

void APillar::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		bOnPlayer = true;
	}
}

void APillar::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		bOnPlayer = false;
	}
}
