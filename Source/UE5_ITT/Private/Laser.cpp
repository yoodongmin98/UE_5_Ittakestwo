// Fill out your copyright notice in the Description page of Project Settings.


#include "Laser.h"

// Sets default values
ALaser::ALaser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALaser::BeginPlay()
{
	Super::BeginPlay();
	
	bPhaseEnd = true;

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

// Called every frame
void ALaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (true == HasAuthority())
	{
		if (true == bAttackNow)
		{
			RotateTime -= DeltaTime;

			AddActorLocalRotation({ 0.f,360.f * DeltaTime,0.f });

			if (RotateTime <= 0.f)
			{
				RotateTime = 15.f;
				bAttackNow = false;
				bPhaseEnd = false;
			}
			return;
		}
		if (true == bPhaseEnd)
		{
			MovingRatio += DeltaTime;

			if (MovingRatio >= 1.f)
			{
				bAttackNow = true;
				MovingRatio = 1.f;
			}

			SetActorRelativeLocation(FMath::Lerp(DefaultPos, AttackPos, MovingRatio));
		}
		else
		{
			MovingRatio -= DeltaTime;

			if (MovingRatio <= 0.f)
			{
				bPhaseEnd = true;
				MovingRatio = 0.f;
			}

			SetActorRelativeLocation(FMath::Lerp(DefaultPos, AttackPos, MovingRatio));
		}
	}
}

