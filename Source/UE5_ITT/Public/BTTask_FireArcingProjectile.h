// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FireArcingProjectile.generated.h"

/**
 * 
 */
UCLASS()
class UE5_ITT_API UBTTask_FireArcingProjectile : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_FireArcingProjectile();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
