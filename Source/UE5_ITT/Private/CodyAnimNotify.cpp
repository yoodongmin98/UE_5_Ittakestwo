// Fill out your copyright notice in the Description page of Project Settings.


#include "CodyAnimNotify.h"
#include "PlayerBase.h"
void UCodyAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp != nullptr)
	{
		APlayerBase* Owner = Cast<APlayerBase>(MeshComp->GetOwner());
		if (Owner == nullptr)
		{
			return;
		}
		Owner->SetCurrentAnimationEndT();
	}
}
