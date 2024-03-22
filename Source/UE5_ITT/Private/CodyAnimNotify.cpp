// Fill out your copyright notice in the Description page of Project Settings.


#include "CodyAnimNotify.h"
#include "Cody.h"
void UCodyAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp != nullptr)
	{
		ACody* Owner = Cast<ACody>(MeshComp->GetOwner());
		if (Owner == nullptr)
		{
			return;
		}
		Owner->SetIsMoveEndT();
	}
}
