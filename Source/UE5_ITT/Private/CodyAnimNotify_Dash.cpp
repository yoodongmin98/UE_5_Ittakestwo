// Fill out your copyright notice in the Description page of Project Settings.


#include "CodyAnimNotify_Dash.h"
#include "Cody.h"
void UCodyAnimNotify_Dash::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp != nullptr)
	{
		ACody* Owner = Cast<ACody>(MeshComp->GetOwner());
		if (Owner == nullptr)
		{
			return;
		}
	}
}