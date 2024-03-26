// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FireArcingProjectile.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnemyFlyingSaucer.h"

UBTTask_FireArcingProjectile::UBTTask_FireArcingProjectile()
{
	NodeName = TEXT("BTT_FireArcingProjectile");
}

EBTNodeResult::Type UBTTask_FireArcingProjectile::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AEnemyFlyingSaucer* Boss = Cast<AEnemyFlyingSaucer>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));
	
	if (nullptr == Boss)
	{
		UE_LOG(LogTemp, Warning, TEXT("The Boss is nullptr"));
		return EBTNodeResult::Failed;
	}
	
	Boss->FireArcingProjectile();
	
	return EBTNodeResult::Succeeded;
}
