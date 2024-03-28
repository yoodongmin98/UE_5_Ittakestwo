// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSaucerAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Cody.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyFlyingSaucer.h"
#include "TimerManager.h"
#include "EnemyMoonBaboon.h"

AFlyingSaucerAIController::AFlyingSaucerAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlyingSaucerAIController::BeginPlay()
{
	Super::BeginPlay();
	
	SetupPlayerReference();
	SetupTimerManager();
	SetupStartBehaviorTreePhase1();

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

void AFlyingSaucerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 체력을 체크해서 페이즈 전환
		UpdatePhaseFromHealth(DeltaTime);
		UpdateLerpRatioForLaserBeam(DeltaTime);
	}
}

void AFlyingSaucerAIController::SetupPlayerReference()
{
	// 폰의 위치를 받아온다. 
	PlayerRef1 = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	
	PlayerRef2 = UGameplayStatics::GetPlayerPawn(GetWorld(), 1);
}

void AFlyingSaucerAIController::SetupStartBehaviorTreePhase1()
{
	if (nullptr != AIBehaviorTreePhase1)
	{
		RunBehaviorTree(AIBehaviorTreePhase1);
		GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerRef1"), PlayerRef1);
		GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerRef2"), PlayerRef2);
		GetBlackboardComponent()->SetValueAsInt(TEXT("Phase1TargetCount"), 1);
	}
	
}

void AFlyingSaucerAIController::SetupTimerManager()
{
	GetWorldTimerManager().SetTimer(TargetLocationCheckHandle,
														 this, 
	   &AFlyingSaucerAIController::SavePreviousTargetLocation,
														 0.01f,
														 true
															);
}

void AFlyingSaucerAIController::SavePreviousTargetLocation()
{
	APawn* TargetPawn = Cast<APawn>(GetBlackboardComponent()->GetValueAsObject(TEXT("LaserBeamTarget")));
	
	if (nullptr != TargetPawn)
	{
		FVector CurrentTargetLocation = TargetPawn->GetActorLocation();

		if (true == bPrevTargetLocationValid)
		{
			PrevTargetLocation = PrevTargetLocationBuffer;
			bPrevTargetLocationValid = false;
		}

		else
		{
			PrevTargetLocation = CurrentTargetLocation;
		}

		PrevTargetLocationBuffer = CurrentTargetLocation;
		bPrevTargetLocationValid = true;
	}

}

void AFlyingSaucerAIController::UpdateLerpRatioForLaserBeam(float DeltaTime)
{
	LaserLerpRatio += DeltaTime / 20;
	if (1.0f <= LaserLerpRatio)
	{
		LaserLerpRatio = 1.0f;
	}

	GetBlackboardComponent()->SetValueAsFloat(TEXT("LaserLerpRatio"), LaserLerpRatio);
}

void AFlyingSaucerAIController::UpdatePhaseFromHealth(float DeltaTime)
{
	// 체력체크
	AEnemyFlyingSaucer* Boss = Cast<AEnemyFlyingSaucer>(GetPawn());
	if (nullptr != Boss)
	{
		float BossCurrentHp = Boss->GetCurrentHp();
		if (EBossPhase::Phase_1 == CurrentBossPhase && BossCurrentHp < 70)
		{
			ChangePhase(EBossPhase::Phase_2);
		}
		else if (EBossPhase::Phase_2 == CurrentBossPhase && BossCurrentHp < 30)
		{
			ChangePhase(EBossPhase::Phase_3);
		}
		else if (EBossPhase::Phase_3 == CurrentBossPhase && BossCurrentHp <= 0)
		{
			Boss->SetCurrentHp(0);
			ChangePhase(EBossPhase::Death);
		}
	}
}

void AFlyingSaucerAIController::ChangePhase(EBossPhase Phase)
{
	// 페이즈 1은 beginplay
	switch (Phase)
	{
	case AFlyingSaucerAIController::EBossPhase::Phase_2:
	{
		if (nullptr != AIBehaviorTreePhase2)
		{
			CurrentBossPhase = EBossPhase::Phase_2;
			RunBehaviorTree(AIBehaviorTreePhase2);
		}
	}
	break;
	case AFlyingSaucerAIController::EBossPhase::Phase_3:
	{
		if (nullptr != AIBehaviorTreePhase3)
		{
			CurrentBossPhase = EBossPhase::Phase_3;
			RunBehaviorTree(AIBehaviorTreePhase3);
		}
	}
	break;
	case AFlyingSaucerAIController::EBossPhase::Death:
	{
		CurrentBossPhase = EBossPhase::Phase_3;
	}
	break;
	default:
	{
		UE_LOG(LogTemp, Warning, TEXT("The boss phase has not been set"));
	}
	break;
	}
	
}
