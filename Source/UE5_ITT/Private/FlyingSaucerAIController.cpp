// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSaucerAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Cody.h"

AFlyingSaucerAIController::AFlyingSaucerAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlyingSaucerAIController::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어 폰 셋팅
	SetPlayerReference();


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

	if (nullptr != AIBehaviorTree)
	{
		RunBehaviorTree(AIBehaviorTree);
	}

	/*if (nullptr != PlayerCody || true == LineOfSightTo(PlayerCody, PlayerCody->GetActorLocation(), false))
	{
		SetFocus(PlayerCody);
	}
	else
	{
		ClearFocus(EAIFocusPriority::Gameplay);
	}*/

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{

	}
}

void AFlyingSaucerAIController::SetPlayerReference()
{
	// 폰의 위치를 받아온다. 
	PlayerCody = Cast<ACody>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

