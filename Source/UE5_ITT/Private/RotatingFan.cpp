// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatingFan.h"
#include "PlayerBase.h"

ARotatingFan::ARotatingFan()
{
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		bReplicates = true;
		SetReplicateMovement(true);
	}
}

void ARotatingFan::BeginPlay()
{
	if (true == HasAuthority())
	{
		GetStaticMeshComponent()->OnComponentHit.AddDynamic(this, &ARotatingFan::OnComponentHit);
	}
}

void ARotatingFan::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Display, TEXT("Hit"));
	if (OtherActor&& OtherActor->ActorHasTag("Player"))
	{
		APlayerBase* Player = Cast<APlayerBase>(OtherActor);
		Player->AttackPlayer(12);
	}
}
