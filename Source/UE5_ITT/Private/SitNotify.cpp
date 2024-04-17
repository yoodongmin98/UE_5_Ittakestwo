// Fill out your copyright notice in the Description page of Project Settings.


#include "SitNotify.h"
#include "Cody.h"
#include "May.h"
void USitNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp != nullptr)
	{
		APlayerBase* Players = Cast<APlayerBase>(MeshComp->GetOwner());

		if (Players != nullptr)
		{
			Players->SetChangeIdle();
		}
	}
}