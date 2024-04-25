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
#include "ITTGameModeBase.h"
#include "Net/UnrealNetwork.h"

AFlyingSaucerAIController::AFlyingSaucerAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		CurrentBehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("CurrentBehaviorTreeCompnent"));
		bReplicates = true;
		SetReplicateMovement(true);
	}
}

void AFlyingSaucerAIController::AddPatternMatchCount()
{
	++PatternMatchCount;
	GetBlackboardComponent()->SetValueAsInt(TEXT("PatternMatchCount"), PatternMatchCount);
}

void AFlyingSaucerAIController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// 메시 컴포넌트를 Replication하기 위한 설정 추가
	DOREPLIFETIME(AFlyingSaucerAIController, AIBehaviorTreePhase1);
	DOREPLIFETIME(AFlyingSaucerAIController, AIBehaviorTreePhase2);
	DOREPLIFETIME(AFlyingSaucerAIController, AIBehaviorTreePhase3);
	DOREPLIFETIME(AFlyingSaucerAIController, CurrentBehaviorTree);
	DOREPLIFETIME(AFlyingSaucerAIController, CurrentBehaviorTreeComp);

	/*DOREPLIFETIME(AFlyingSaucerAIController, PrevTargetLocation);
	DOREPLIFETIME(AFlyingSaucerAIController, PrevTargetLocationBuffer);
	DOREPLIFETIME(AFlyingSaucerAIController, bPrevTargetLocationValid);
	DOREPLIFETIME(AFlyingSaucerAIController, LaserLerpRatio);
	DOREPLIFETIME(AFlyingSaucerAIController, LaserLerpRate);
	DOREPLIFETIME(AFlyingSaucerAIController, PatternMatchCount);*/
}



void AFlyingSaucerAIController::BeginPlay()
{
	Super::BeginPlay();

	
	
	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		
	}
}

void AFlyingSaucerAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 플레이어 셋업이 되지 않았을 때 
		if (false == bIsSetupPlayerRef)
		{
			SetupPlayerRefAndBehaviorTreePhase1();
		}

		// 셋팅이 되었다면 그때부터 동작
		else if (true == bIsSetupPlayerRef)
		{
			// UpdateLerpRatioForLaserBeam(DeltaTime);
		}
	}
}

void AFlyingSaucerAIController::SetupPlayerRefAndBehaviorTreePhase1()
{
	// 여기서 게임모드를 받아와서 카운트가 2가 되었는지 확인하고 2가 되었다면 bool 값을 true로 변경
	AITTGameModeBase* CurrentGameMode = Cast<AITTGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	int32 LoginCount = CurrentGameMode->GetPlayerLoginCount();
	if (2 == LoginCount)
	{
		TArray<AActor*> Players = CurrentGameMode->GetLoginPlayerControllers();
		APlayerController* PlayerController0 = Cast<APlayerController>(Players[0]);
		APlayerController* PlayerController1 = Cast<APlayerController>(Players[1]);
		PlayerRef1 = PlayerController0->GetPawn();
		PlayerRef2 = PlayerController1->GetPawn();
		SetupStartBehaviorTreePhase1();
		//GetBlackboardComponent()->SetValueAsVector(TEXT("PrevTargetLocation"), PrevTargetLocation);
		bIsSetupPlayerRef = true;
	}
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

//void AFlyingSaucerAIController::SavePreviousTargetLocation()
//{
//	APawn* TargetPawn = Cast<APawn>(GetBlackboardComponent()->GetValueAsObject(TEXT("LaserBeamTarget")));
//	
//	if (nullptr != TargetPawn)
//	{
//		FVector CurrentTargetLocation = TargetPawn->GetActorLocation();
//
//		// 이전 타겟 위치가 유효하다면 
//		if (true == bPrevTargetLocationValid)
//		{
//			// 타겟 위치는 저장되어있는 이전타겟위치로 지정하고 false 처리
//			PrevTargetLocation = PrevTargetLocationBuffer;
//			bPrevTargetLocationValid = false;
//		}
//
//		else
//		{
//			// 유효하지 않다면 타겟 위치는 현재 위치로 지정
//			PrevTargetLocation = CurrentTargetLocation;
//		}
//
//		// 타겟위치를 세팅
//		GetBlackboardComponent()->SetValueAsVector(TEXT("PrevTargetLocation"), PrevTargetLocation);
//		PrevTargetLocationBuffer = CurrentTargetLocation;
//		bPrevTargetLocationValid = true;
//	}
//
//}
//
//void AFlyingSaucerAIController::UpdateLerpRatioForLaserBeam(float DeltaTime)
//{
//	LaserLerpRatio += DeltaTime * LaserLerpRate;
//	if (1.0f <= LaserLerpRatio)
//	{
//		GetBlackboardComponent()->SetValueAsVector(TEXT("PrevLaserAttackLocation"), PrevTargetLocation);
//		SavePreviousTargetLocation();
//		LaserLerpRatio -= 1.0f;
//		if (0.1f <= LaserLerpRatio)
//		{
//			LaserLerpRatio = 0.0f;
//		}
//	}
//
//	GetBlackboardComponent()->SetValueAsFloat(TEXT("LaserLerpRatio"), LaserLerpRatio);
//}
//
