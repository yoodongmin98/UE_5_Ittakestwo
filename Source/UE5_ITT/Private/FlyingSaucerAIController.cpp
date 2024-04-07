// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSaucerAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Cody.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "EnemyFlyingSaucer.h"
#include "TimerManager.h"
#include "EnemyMoonBaboon.h"

AFlyingSaucerAIController::AFlyingSaucerAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentBehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("CurrentBehaviorTreeCompnent"));
}

void AFlyingSaucerAIController::AddPatternMatchCount()
{
	++PatternMatchCount;
	GetBlackboardComponent()->SetValueAsInt(TEXT("PatternMatchCount"), PatternMatchCount);
}

void AFlyingSaucerAIController::BeginPlay()
{
	Super::BeginPlay();
	
	SetupPlayerReference();
	SetupStartBehaviorTreePhase1();
	GetBlackboardComponent()->SetValueAsVector(TEXT("PrevTargetLocation"), PrevTargetLocation);

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
		CurrentBehaviorTree = AIBehaviorTreePhase1;
		CurrentBehaviorTreeComp->StartTree(*AIBehaviorTreePhase1, EBTExecutionMode::Looped);
		GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerCody"), PlayerRef1);
		GetBlackboardComponent()->SetValueAsObject(TEXT("PlayerMay"), PlayerRef2);
		GetBlackboardComponent()->SetValueAsInt(TEXT("Phase1TargetCount"), 1);
		GetBlackboardComponent()->SetValueAsInt(TEXT("PatternMatchCount"), PatternMatchCount);
	}
}

void AFlyingSaucerAIController::SetupStartBehaviorTreePhase2()
{
	if (nullptr != CurrentBehaviorTree && nullptr != AIBehaviorTreePhase2)
	{
		CurrentBehaviorTreeComp->StopTree();
		RunBehaviorTree(AIBehaviorTreePhase2);
		CurrentBehaviorTreeComp->StartTree(*AIBehaviorTreePhase2);
		CurrentBehaviorTree = AIBehaviorTreePhase2;
	}
}

void AFlyingSaucerAIController::SetupStartBehaviorTreePhase3()
{
	if (nullptr != CurrentBehaviorTree && nullptr != AIBehaviorTreePhase3)
	{
		CurrentBehaviorTreeComp->StopTree();
		RunBehaviorTree(AIBehaviorTreePhase3);
		CurrentBehaviorTreeComp->StartTree(*AIBehaviorTreePhase3);
		CurrentBehaviorTree = AIBehaviorTreePhase3;
	}
}

void AFlyingSaucerAIController::SavePreviousTargetLocation()
{
	APawn* TargetPawn = Cast<APawn>(GetBlackboardComponent()->GetValueAsObject(TEXT("LaserBeamTarget")));
	
	if (nullptr != TargetPawn)
	{
		FVector CurrentTargetLocation = TargetPawn->GetActorLocation();

		// 이전 타겟 위치가 유효하다면 
		if (true == bPrevTargetLocationValid)
		{
			// 타겟 위치는 저장되어있는 이전타겟위치로 지정하고 false 처리
			PrevTargetLocation = PrevTargetLocationBuffer;
			bPrevTargetLocationValid = false;
		}

		else
		{
			// 유효하지 않다면 타겟 위치는 현재 위치로 지정
			PrevTargetLocation = CurrentTargetLocation;
		}

		// 타겟위치를 세팅
		GetBlackboardComponent()->SetValueAsVector(TEXT("PrevTargetLocation"), PrevTargetLocation);
		PrevTargetLocationBuffer = CurrentTargetLocation;
		bPrevTargetLocationValid = true;
	}

}

void AFlyingSaucerAIController::UpdateLerpRatioForLaserBeam(float DeltaTime)
{
	LaserLerpRatio += DeltaTime * LaserLerpRate;
	if (1.0f <= LaserLerpRatio)
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("PrevLaserAttackLocation"), PrevTargetLocation);
		SavePreviousTargetLocation();
		LaserLerpRatio -= 1.0f;
	}

	GetBlackboardComponent()->SetValueAsFloat(TEXT("LaserLerpRatio"), LaserLerpRatio);
}

