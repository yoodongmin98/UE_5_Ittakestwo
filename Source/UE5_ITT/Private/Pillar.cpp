// Fill out your copyright notice in the Description page of Project Settings.


#include "Pillar.h"
#include "Components/StaticMeshComponent.h"
#include "FsmComponent.h"
#include "ParentShutter.h"
#include "EnergyCore.h"
#include "NiagaraComponent.h"
#include "CoreExplosionEffect.h"
#include "SoundManageComponent.h"

// Sets default values
APillar::APillar()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
	
	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarMesh"));
	RootComponent = PillarMesh;
	
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(PillarMesh);

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetupAttachment(PillarMesh);

	SoundComp = CreateDefaultSubobject<USoundManageComponent>(TEXT("SoundComp"));

	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);
		SetupFsm();
	}
}

// Called when the game starts or when spawned
void APillar::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::Close);
		SoundComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
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
			SoundComp->MulticastSetAttenuationDistance(1500.f, 4000.f);
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
			SoundComp->MulticastChangeSound(TEXT("Shutter_Cue"));
			DefaultPos = GetActorLocation();
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
			SoundComp->MulticastChangeSound(TEXT("PowerCoreElevatorUp_Cue"));
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
			//���� ���̱��� �ø��� �������̺�Ʈ ���ε�
			PlayerWaitRatio += DT;
			if (PlayerWaitRatio >= 1.f)
			{
				PlayerWaitRatio = 1.f;
				FsmComp->ChangeState(Fsm::Wait);
				SetActorLocation(FMath::Lerp(DefaultPos, PlayerWaitPos, PlayerWaitRatio));
				ButtonMesh->OnComponentBeginOverlap.AddDynamic(this, &APillar::OnOverlapBegin);
				ButtonMesh->OnComponentEndOverlap.AddDynamic(this, &APillar::OnOverlapEnd);
				return;
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
			//�÷��̾ �ö���� ��ٸ��� ����
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
			SoundComp->MulticastChangeSound(TEXT("PowerCoreElevatorLoop_Cue"));
		},

		[this](float DT)
		{
			//�÷��̾ ������
			if (false == bOnPlayer)
			{
				bShieldOpen = true;
				FsmComp->ChangeState(Fsm::MoveDown);
				return;
			}

			//��� ���� �ø���
			MoveRatio += DT;
			if (MoveRatio >= 1.f)
			{
				MoveRatio = 1.f;
				SetActorLocation(FMath::Lerp(PlayerWaitPos, MovePos, MoveRatio));
				FsmComp->ChangeState(Fsm::WaitBoom);
				return;
			}

			SetActorLocation(FMath::Lerp(PlayerWaitPos, MovePos, MoveRatio));
		},

		[this]
		{
			SoundComp->MulticastSoundStop();
		});

	FsmComp->CreateState(Fsm::WaitBoom,
		[this]
		{
		},

		[this](float DT)
		{
			
			////////////////Debug//////////////
			if (bDone == true)
			{
				FsmComp->ChangeState(Fsm::Boom);
				return;
			}
			////////////////Debug//////////////

			//�÷��̾ ������
			if (false == bOnPlayer)
			{
				bShieldOpen = true;
				FsmComp->ChangeState(Fsm::MoveDown);
				return;
			}

			//�ǵ尡 �� �ȿ������� ����
			if (false == bShieldOpen)
			{
				ShieldOpenRatio += DT;
				if (ShieldOpenRatio >= 1.f)
				{
					ShieldOpenRatio = 1.f;
					bShieldOpen = true;
					SoundComp->MulticastPlaySoundLocation(TEXT("PowerCoreShieldactivate_Cue"),GetActorLocation());
				}
				ShieldMesh->SetRelativeLocation(FMath::Lerp(ShieldDefaultPos, ShieldOpenPos, ShieldOpenRatio));
			}
			
			//�ǵ� ���ȴµ� ��ü �¾Ƽ� ����
			if (true==bShieldOpen&&true == EnergyCoreActor->IsExplode())
			{
				//������ Ÿ�� üũ �ʿ�
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
			SoundComp->MulticastChangeSound(TEXT("PowerCoreElevatorLoop_Cue"));
		},

		[this](float DT)
		{
			//�÷��̾ �ö�Ž
			if (true == bOnPlayer)
			{
				FsmComp->ChangeState(Fsm::MoveUp);
				return;
			}

			//�ǵ尡 ���������� �ݱ�
			if (true == bShieldOpen)
			{
				ShieldOpenRatio -= DT * 3.f;
				if (ShieldOpenRatio <= 0.f)
				{
					ShieldOpenRatio = 0.f;
					bShieldOpen = false;
					SoundComp->MulticastPlaySoundLocation(TEXT("PowerCoreShielddeactivate_Cue"), GetActorLocation());
				}
				ShieldMesh->SetRelativeLocation(FMath::Lerp(ShieldDefaultPos, ShieldOpenPos, ShieldOpenRatio));
			}

			//��� ������
			MoveRatio -= DT;
			if (MoveRatio <= 0.f)
			{
				MoveRatio = 0.f;
				SetActorLocation(FMath::Lerp(PlayerWaitPos, MovePos, MoveRatio));
				FsmComp->ChangeState(Fsm::Wait);
				return;
			}

			SetActorLocation(FMath::Lerp(PlayerWaitPos, MovePos, MoveRatio));
		},

		[this]
		{
			SoundComp->MulticastSoundStop();
		});

	FsmComp->CreateState(Fsm::Boom,
		[this]
		{
			ParentShutter->SetDone();

			SoundComp->MulticastPlaySoundDirect("PowerCoreHit_Cue");
		},

		[this](float DT)
		{
			if (FsmComp->GetStateLiveTime()>=1.f)
			{
				FsmComp->ChangeState(Fsm::Done);
			}
		},

		[this]
		{
			MulticastSpawnNiagaraEffect();
			EnergyCoreActor->Destroy();
		});


	FsmComp->CreateState(Fsm::ShutterClose,
		[this]
		{
		},

		[this](float DT)
		{
			if (FsmComp->GetStateLiveTime() > 1.f)
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
			SoundComp->MulticastChangeSound(TEXT("PowerCoreElevatorLoop_Cue"));
		},

		[this](float DT)
		{
			//���� �Ϸ�Ǹ� ���� ������ close���� �����ϱ�
			MoveRatio -= DT;
			if (MoveRatio <= 0.f)
			{
				MoveRatio = 0.f;
				SetActorLocation(FMath::Lerp(DefaultPos, MovePos, MoveRatio));
				FsmComp->ChangeState(Fsm::ShutterClose);
				return;
			}

			SetActorLocation(FMath::Lerp(DefaultPos, MovePos, MoveRatio));
		},

		[this]
		{
			SoundComp->MulticastSoundStop();
		});
}

void APillar::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherActor->ActorHasTag("Player") == true)
	{
		bOnPlayer = true;
	}
}

void APillar::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherActor->ActorHasTag("Player") == true)
	{
		bOnPlayer = false;
	}
}

void APillar::MulticastSpawnNiagaraEffect_Implementation()
{
	if (nullptr != EnergyCoreActor)
	{
		FVector EffectLocation = EnergyCoreActor->GetActorLocation();
		ACoreExplosionEffect* Effect = GetWorld()->SpawnActor<ACoreExplosionEffect>(ExplosionEffectClass, EffectLocation, FRotator::ZeroRotator);
	}
}