// Fill out your copyright notice in the Description page of Project Settings.


#include "CutSceneAnimationNotify.h"
#include "Cody.h"






void UCutSceneAnimationNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp != nullptr)
	{
		ACody* Owner = Cast<ACody>(MeshComp->GetOwner());
		if (Owner == nullptr)
		{
			return;
		}
		Owner->SetCodyMoveable();
	}
}