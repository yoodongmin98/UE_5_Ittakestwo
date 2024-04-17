// Fill out your copyright notice in the Description page of Project Settings.


#include "ParentShutter.h"
#include "CoreShutter.h"

// Sets default values
AParentShutter::AParentShutter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		bReplicates = true;
		SetReplicateMovement(true);
	}
	
}

// Called when the game starts or when spawned
void AParentShutter::BeginPlay()
{
	Super::BeginPlay();

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		ArrayCoreShutter.Reserve(4);

		for (int32 i = 0; i < 4; i++)
		{
			ArrayCoreShutter.Push(GetWorld()->SpawnActor<ACoreShutter>(CoreShutterClass));
			ArrayCoreShutter[i]->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
			ArrayCoreShutter[i]->SetActorRelativeRotation({ 0.f, 90.f * i,0.f });
			ArrayCoreShutter[i]->SetPivotPos(GetActorLocation());
		}
		ArrayCoreShutter[0]->SetOpenPos({ -MovingSize,MovingSize ,0.f });
		ArrayCoreShutter[1]->SetOpenPos({ -MovingSize,-MovingSize ,0.f });
		ArrayCoreShutter[2]->SetOpenPos({ MovingSize,-MovingSize ,0.f });
		ArrayCoreShutter[3]->SetOpenPos({ MovingSize,MovingSize ,0.f });
	}

}

// Called every frame
void AParentShutter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
}

void AParentShutter::SetShutterOpen()
{
	for (int32 i = 0; i < ArrayCoreShutter.Num(); i++)
	{
		ArrayCoreShutter[i]->OpenShutter();
	}
}

void AParentShutter::SetDone()
{
	for (int32 i = 0; i < ArrayCoreShutter.Num(); i++)
	{
		ArrayCoreShutter[i]->SetDone();
	}
}


