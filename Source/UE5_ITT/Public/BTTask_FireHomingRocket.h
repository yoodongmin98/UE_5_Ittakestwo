// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FireHomingRocket.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API UBTTask_FireHomingRocket : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_FireHomingRocket();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
