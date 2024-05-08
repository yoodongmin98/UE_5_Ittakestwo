// Fill out your copyright notice in the Description page of Project Settings.


#include "PhaseEndCameraRail.h"
#include "Kismet/GameplayStatics.h"
#include "FsmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"
#include "PlayerBase.h"
#include "EnemyFlyingSaucer.h"


APhaseEndCameraRail::APhaseEndCameraRail(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);

		CamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CamComp"));
		CamComp->SetupAttachment(RailCameraMount);

		FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));
		SetupFsmState();
	}
}

bool APhaseEndCameraRail::ShouldTickIfViewportsOnly() const
{
	return false;
}

void APhaseEndCameraRail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
	
void APhaseEndCameraRail::EnableCameraMove()
{
	FsmComp->ChangeState(Fsm::Move);
}

void APhaseEndCameraRail::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::Wait);
	}
}

void APhaseEndCameraRail::SetupFsmState()
{
	FsmComp->CreateState(Fsm::Wait,
		[this]
		{

		},

		[this](float DT)
		{
		},

		[this]
		{
			
		});

	FsmComp->CreateState(Fsm::Move,
		[this]
		{
			SetupActorsRef();

			UE_LOG(LogTemp, Warning, TEXT("Start"));
			CurrentPositionOnRail = 0;

			FVector TargetLocation = EnemyFlyingSaucer->GetActorLocation() - GetActorLocation();

			LerpEndRotation = TargetLocation.Rotation();
			LerpStartRotation = GetActorRotation();
		},

		[this](float DT)
		{
			// 플레이어 방향으로 회전 러프 완료시 보스 포커스상태로 변경
			CurrentPositionOnRail += DT * 0.25f;
			CamComp->SetWorldLocation(GetRailSplineComponent()->GetLocationAtTime(CurrentPositionOnRail, ESplineCoordinateSpace::World));

		/*	if (1.5f <= FsmComp->GetStateLiveTime())
			{
				LerpRatio += DT * 0.35f;
				if (1.0f <= LerpRatio)
				{
					LerpRatio = 1.0f;
					FRotator TargetRotation = FMath::Lerp(LerpStartRotation, LerpEndRotation, LerpRatio);
					CamComp->SetRelativeRotation(TargetRotation);
					return;
				}

				FRotator TargetRotation = FMath::Lerp(LerpStartRotation, LerpEndRotation, LerpRatio);
				CamComp->SetRelativeRotation(TargetRotation);
			}*/
		},

		[this]
		{
		});

	FsmComp->CreateState(Fsm::End,
		[this]
		{
		},

		[this](float DT)
		{
		},

		[this]
		{
		});
}

void APhaseEndCameraRail::SetupActorsRef()
{
	//// 플레이어 액터 받아오고, 각각의 포인터에 저장
	//TArray<AActor*> PlayerActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerBase::StaticClass(), PlayerActors);

	//if (2 > PlayerActors.Num())
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Players nullptr"));
	//	return;
	//}

	//// 0번이 코디일경우와 아닐경우 각각 반대로 저장
	//if (true == PlayerActors[0]->ActorHasTag("Cody"))
	//{
	//	PlayerCody = Cast<ACody>(PlayerActors[0]);
	//	PlayerMay = Cast<AMay>(PlayerActors[1]);
	//}
	//else
	//{
	//	PlayerCody = Cast<ACody>(PlayerActors[1]);
	//	PlayerMay = Cast<AMay>(PlayerActors[0]);
	//}

	AActor* BossPtr = UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyFlyingSaucer::StaticClass());
	if (nullptr != BossPtr)
	{
		EnemyFlyingSaucer = Cast<AEnemyFlyingSaucer>(BossPtr);
	}
}
