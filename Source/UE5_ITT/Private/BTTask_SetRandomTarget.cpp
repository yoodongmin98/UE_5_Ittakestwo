// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetRandomTarget.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetRandomTarget::UBTTask_SetRandomTarget()
{
	NodeName = TEXT("BTT_SetRandomTarget");
}

EBTNodeResult::Type UBTTask_SetRandomTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 블랙보드 컴포넌트의 키값을 가져왔을때. 메이,코디,메이 순으로 타겟 지정하도록
	// 임시로 세번다 코디 지정
	UBlackboardComponent* BlackBoardComp = OwnerComp.GetBlackboardComponent();
	if (nullptr != BlackBoardComp)
	{
		int32 TargetCount = BlackBoardComp->GetValueAsInt(TEXT("PatternMatchCount"));
		switch (TargetCount)
		{
		case 0:
		{
			UObject* PlayerRef = BlackBoardComp->GetValueAsObject(TEXT("PlayerCody"));
			BlackBoardComp->SetValueAsObject(TEXT("LaserBeamTarget"), PlayerRef);
		}
			break;
		case 1:
		{
			UObject* PlayerRef = BlackBoardComp->GetValueAsObject(TEXT("PlayerMay"));
			BlackBoardComp->SetValueAsObject(TEXT("LaserBeamTarget"), PlayerRef);
		}
			break;
		case 2:
		{
			UObject* PlayerRef = BlackBoardComp->GetValueAsObject(TEXT("PlayerCody"));
			BlackBoardComp->SetValueAsObject(TEXT("LaserBeamTarget"), PlayerRef);
		}
			break;
		default:
			return EBTNodeResult::Failed;
			break;
		}
	}

	return EBTNodeResult::Succeeded;
}
