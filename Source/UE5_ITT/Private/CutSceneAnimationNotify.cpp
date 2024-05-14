// Fill out your copyright notice in the Description page of Project Settings.


#include "CutSceneAnimationNotify.h"
#include "Cody.h"
#include "May.h"






void UCutSceneAnimationNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp != nullptr)
	{
		ACody* Codys = Cast<ACody>(MeshComp->GetOwner());
		AMay* Mays = Cast<AMay>(MeshComp->GetOwner());
		
		if (Codys != nullptr)
		{
			Codys->SetCodyMoveable();
		}
		if (Mays != nullptr)
		{
			Mays->SetMayMoveable();
		}
	}
}