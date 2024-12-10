// Fill out your copyright notice in the Description page of Project Settings.


#include "RotatingFan.h"
#include "PlayerBase.h"

ARotatingFan::ARotatingFan()
{
	if (true == HasAuthority())
	{
		// ������ Ŭ���̾�Ʈ ��ο��� ��������� ������ ���� �ϴ� �ڵ��Դϴ�.
		bReplicates = true;
		SetReplicateMovement(true);
		RootComponent = GetStaticMeshComponent();

		ColMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ColMesh"));
		ColMesh->SetupAttachment(RootComponent);
	}
}

void ARotatingFan::BeginPlay()
{
	if (true == HasAuthority())
	{
		ColMesh->OnComponentBeginOverlap.AddDynamic(this, &ARotatingFan::OnOverlapBegin);
	}
}

void ARotatingFan::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		APlayerBase* Player = Cast<APlayerBase>(OtherActor);
		Player->AttackPlayer(12);
	}
}
