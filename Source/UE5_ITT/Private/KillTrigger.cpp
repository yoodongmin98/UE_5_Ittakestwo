// Fill out your copyright notice in the Description page of Project Settings.


#include "KillTrigger.h"
#include "Components/ShapeComponent.h"
#include "PlayerBase.h"
	
AKillTrigger::AKillTrigger()
{
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);
	}
}

void AKillTrigger::BeginPlay()
{
	if (true == HasAuthority())
	{
		GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AKillTrigger::OnOverlapBegin);
	}
}

void AKillTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor != this && OtherActor->ActorHasTag("Player")==true)
	{
		APlayerBase* Player = Cast<APlayerBase>(OtherActor);

		Player->AttackPlayer(12);
	}
}
