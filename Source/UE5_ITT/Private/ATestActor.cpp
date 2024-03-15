// Fill out your copyright notice in the Description page of Project Settings.


#include "ATestActor.h"

// Sets default values
AATestActor::AATestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AATestActor::BeginPlay()
{
	Super::BeginPlay();

	StartPosFloat = GetActorLocation();

	MovingUnit = 0.0f;
	Test_Dir = StartPosFloat - DestPosFloat;
	//SetActorLocation(StartPosFloat);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

// Called every frame
void AATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		if (MovingUnit > 1.0f * 2)
		{
			FVector tmp = StartPosFloat;
			StartPosFloat = DestPosFloat;
			DestPosFloat = tmp;
			MovingUnit = 0.0f;
		}
		// 이동하도록 하는 예제
		MovingUnit += DeltaTime;
		FVector NextPos = FMath::Lerp(StartPosFloat, DestPosFloat, MovingUnit / 2);
		SetActorLocation(NextPos);
	}
}
