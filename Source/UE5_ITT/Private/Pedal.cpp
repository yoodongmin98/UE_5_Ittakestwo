	// Fill out your copyright notice in the Description page of Project Settings.


#include "Pedal.h"
#include "PlayerBase.h"
#include "FsmComponent.h"
#include "EnemyMoonBaboon.h"
#include "ITTGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimSequence.h"

// Sets default values
APedal::APedal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (HasAuthority()== true)
	{
		bReplicates = true;
		SetReplicateMovement(true);

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
	if (HasAuthority() == true)
	{
		FsmComp->ChangeState(Fsm::ServerDelay);
	}
}

void APedal::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));

	FsmComp->CreateState(Fsm::ServerDelay,
		[this]
		{
		},

		[this](float DeltaTime)
		{
			AITTGameModeBase* ITTGameMode = Cast<AITTGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
			if (ITTGameMode->GetPlayerLoginCount() == 2)
			{
				ServerDelayTime -= DeltaTime;
			}
			if (ServerDelayTime=<0.f)
			{
				FsmComp->ChangeState(Fsm::SmashWait);
			}
		},

		[this]
		{
			Multicast_ChangeAnim(TEXT("/Game/Characters/EnemyMoonBaboon/Animations/MoonBaboon_Ufo_Mh_Pedal_Anim.MoonBaboon_Ufo_Mh_Pedal_Anim"), true);
		}
	);
	FsmComp->CreateState(Fsm::SmashWait,
		[this]
		{

		},

		[this](float DeltaTime)
		{			
			if( 7.f> CurAnimFrame&& CurAnimFrame >=5.f)
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
			//5프레임 지속
			SmashRatio += DeltaTime * (1.f / (Anim1FrameTime * 5.f));
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
			if (43.f > CurAnimFrame && CurAnimFrame >= 41.f)
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
			//18프레임 지속
			SmashRatio -= DeltaTime * (1.f/(Anim1FrameTime*16.f));
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

void APedal::Multicast_ChangeAnim_Implementation(const FString& strPath, bool bLoop)
{
	UAnimSequence* AnimSequence = LoadObject<UAnimSequence>(nullptr, *strPath);
	if (nullptr == AnimSequence)
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimSequence is nullptr"));
		return;
	}

	MoonBaboon->GetMesh()->PlayAnimation(AnimSequence, bLoop);

	UE_LOG(LogTemp, Display, TEXT("TestCount 1"));
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
		//애니메이션 인스턴스
		UAnimInstance* AnimInstance = MoonBaboon->GetMesh()->GetAnimInstance();

		//null이 아니면서 노티파이 체크(체크 안하면 클라 들어올때 터짐)
		if (AnimInstance && !AnimInstance->ActiveAnimNotifyEventReference.IsEmpty())
		{
			float CurTime = AnimInstance->ActiveAnimNotifyEventReference[0].GetCurrentAnimationTime();
			CurAnimFrame = CurTime / (AnimMaxTime / AnimMaxFrame);
		}
	}
}

