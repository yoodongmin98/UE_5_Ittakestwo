// Fill out your copyright notice in the Description page of Project Settings.


#include "EjectButton.h"
#include "Components/StaticMeshComponent.h"
#include "FsmComponent.h"

// Sets default values
AEjectButton::AEjectButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		bReplicates = true;
		SetReplicateMovement(true);
		SetupFsm();

		SM_BaseComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_BaseComp"));
		SetRootComponent(SM_BaseComp);
		SM_PushComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_PushComp"));
		SM_PushComp->SetupAttachment(SM_BaseComp);
	}
}

// Called when the game starts or when spawned
void AEjectButton::BeginPlay()
{
	Super::BeginPlay();
	if (true == HasAuthority())
	{
		FsmComp->ChangeState(Fsm::Wait);
	}
}

void AEjectButton::SetupFsm()
{
	FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));

	FsmComp->CreateState(Fsm::Wait,
		[this]
		{

		},

		[this](float DeltaTime)
		{
			UE_LOG(LogTemp, Display, TEXT("TestButton"));
		},

		[this]
		{
		}
	);
}

// Called every frame
void AEjectButton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
	