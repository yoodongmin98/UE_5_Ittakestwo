// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetActorLocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Cody.h"

UBTTask_SetActorLocation::UBTTask_SetActorLocation()
{
	NodeName = TEXT("Set Actor Location");
}

EBTNodeResult::Type UBTTask_SetActorLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ACody* PlayerRef = Cast<ACody>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("PlayerCodyRef"));
	if (nullptr != PlayerRef)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerRef->GetActorLocation());
	}
	
	else
	{
		return EBTNodeResult::Failed;
	}

	
	return EBTNodeResult::Succeeded;
}
