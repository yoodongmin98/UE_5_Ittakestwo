// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSaucerAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Cody.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyFlyingSaucer.h"

AFlyingSaucerAIController::AFlyingSaucerAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlyingSaucerAIController::BeginPlay()
{
	Super::BeginPlay();
	
	SetupStartBehaviorTree();
	SetupPlayerReference();

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

	// 체력을 체크해서 페이즈 전환
	UpdatePhaseFromHealth(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{

	}
}

void AFlyingSaucerAIController::SetupPlayerReference()
{
	// 폰의 위치를 받아온다. 
	PlayerCodyRef = Cast<ACody>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AFlyingSaucerAIController::SetupStartBehaviorTree()
{
	if (nullptr != AIBehaviorTreePhase1)
	{
		RunBehaviorTree(AIBehaviorTreePhase1);
	}
	
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

