// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSaucerAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Cody.h"
#include "BehaviorTree/BlackboardComponent.h"

AFlyingSaucerAIController::AFlyingSaucerAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlyingSaucerAIController::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어 폰 셋팅
	SetupPlayerReference();
	SetupBehaviorTree();

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

	}
}

void AFlyingSaucerAIController::SetupPlayerReference()
{
	// 폰의 위치를 받아온다. 
	PlayerCody = Cast<ACody>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AFlyingSaucerAIController::SetupBehaviorTree()
{
	if (nullptr != AIBehaviorTree)
	{
		RunBehaviorTree(AIBehaviorTree);
		GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerCody->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
		GetBlackboardComponent()->SetValueAsVector(TEXT("CurrentActorLocation"), GetPawn()->GetActorLocation());
	}
}

