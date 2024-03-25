// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FireLaserBeam.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyFlyingSaucer.h"

UBTTask_FireLaserBeam::UBTTask_FireLaserBeam()
{
	NodeName = TEXT("Fire Laser Beam");
}

EBTNodeResult::Type UBTTask_FireLaserBeam::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyFlyingSaucer* Boss = Cast<AEnemyFlyingSaucer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));

	if (nullptr == Boss)
	{
		UE_LOG(LogTemp, Warning, TEXT("The Boss is nullptr"));
		return EBTNodeResult::Failed;
	}

	Boss->FireHomingRocket();

	return EBTNodeResult::Succeeded;
	
}
