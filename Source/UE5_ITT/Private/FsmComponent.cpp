// Fill out your copyright notice in the Description page of Project Settings.


#include "FsmComponent.h"

// Sets default values for this component's properties
UFsmComponent::UFsmComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UFsmComponent::BeginPlay()
{
	Super::BeginPlay(); 
	// ...
	
}


void UFsmComponent::FsmTick(float DT)
{	
	StateLiveTime += DT;
	if (CurState == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s"), *GetOwner()->GetName(), TEXT("Fsm Index Error"));
	}
	if (true == MapState[CurState].Update.IsBound())
	{
		MapState[CurState].Update.Execute(DT);
	}	
}

// Called every frame
void UFsmComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (true == GetOwner()->HasAuthority())
	{
		FsmTick(DeltaTime);
	}
	// ...
}

void UFsmComponent::ChangeState(int32 Index)
{
	if (false == MapState.Contains(Index))
	{
		UE_LOG(LogTemp, Error, TEXT("Fsm key not exist"));
		return;
	}
	if (Index == CurState)
	{
		return;
	}
	if (CurState!=-1&&true == MapState[CurState].End.IsBound())
	{
		MapState[CurState].End.Execute();
	}
	CurState = Index;
	StateLiveTime = 0.f;

	if (true == MapState[CurState].Start.IsBound())
	{
		MapState[CurState].Start.Execute();
	}
}
