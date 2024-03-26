// Fill out your copyright notice in the Description page of Project Settings.


#include "MoonBaboonAIController.h"

AMoonBaboonAIController::AMoonBaboonAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMoonBaboonAIController::BeginPlay()
{
	Super::BeginPlay();

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

void AMoonBaboonAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		
	}
}
