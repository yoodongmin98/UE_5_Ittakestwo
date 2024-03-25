// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FireHomingRocket.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyFlyingSaucer.h"

UBTTask_FireHomingRocket::UBTTask_FireHomingRocket()
{
	NodeName = TEXT("Fire Homing Rocket");
}

EBTNodeResult::Type UBTTask_FireHomingRocket::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyFlyingSaucer* Boss = Cast<AEnemyFlyingSaucer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));

	if (nullptr != Boss)
	{
		Boss->FireHomingRocket();
	}

	else if (nullptr == Boss)
	{
		UE_LOG(LogTemp, Warning, TEXT("The Boss is nullptr"));
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}
