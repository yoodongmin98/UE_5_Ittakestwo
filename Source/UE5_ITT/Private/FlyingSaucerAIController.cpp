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

	ChangePhase(EBossPhase::Phase_1);
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

void AFlyingSaucerAIController::UpdatePhaseFromHealth(float DeltaTime)
{
	// 체력체크
	AEnemyFlyingSaucer* Boss = Cast<AEnemyFlyingSaucer>(GetPawn());
}

void AFlyingSaucerAIController::ChangePhase(EBossPhase Phase)
{
	// 변경될 페이즈의 값이 더 클때만 변경, 그게 아니라면 변경하지 않음
	if (CurrentBossPhase < Phase)
	{
		switch (Phase)
		{
		case AFlyingSaucerAIController::EBossPhase::Phase_1:
		{
			if (nullptr != AIBehaviorTreePhase1)
			{
				RunBehaviorTree(AIBehaviorTreePhase1);
			}
		}
			break;
		case AFlyingSaucerAIController::EBossPhase::Phase_2:
		{
			if (nullptr != AIBehaviorTreePhase2)
			{
				RunBehaviorTree(AIBehaviorTreePhase2);
			}
		}
			break;
		case AFlyingSaucerAIController::EBossPhase::Phase_3:
		{
			if (nullptr != AIBehaviorTreePhase3)
			{
				RunBehaviorTree(AIBehaviorTreePhase3);
			}
		}
			break;
		case AFlyingSaucerAIController::EBossPhase::Death:
			break;
		case AFlyingSaucerAIController::EBossPhase::Max:
			break;
		default:
			break;
		}
	}
	
}

