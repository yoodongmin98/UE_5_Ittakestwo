// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FsmComponent.generated.h"

DECLARE_DELEGATE(StartFunction)
DECLARE_DELEGATE_OneParam(UpdateFunction,float)
DECLARE_DELEGATE(EndFunction)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_ITT_API UFsmComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFsmComponent();	

public:
	class StateFunction
	{
	public:
		StartFunction Start = nullptr;
		UpdateFunction Update = nullptr;
		EndFunction End = nullptr;
	};

	template<class T>
	void InitFsm(T Index)
	{
		int32 CastIndex = static_cast<int32>(Index);
		for (int32 i = 0; i < CastIndex; i++)
		{
			MapState.Add(i);
		}
	}
	template<class T>
	void CreateState(T Index, StateFunction& StateFunc)
	{
		CreateState(static_cast<int32>(Index), StateFunc);
	};

	void CreateState(int Index, StateFunction& StateFunc)
	{
		MapState[Index]= StateFunc;
	};

	template<class T>
	void ChangeState(T Index)
	{
		ChangeState(static_cast<int32>(Index));
	};

	void ChangeState(int32 Index);

	void FsmTick(float DT);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TMap<int32,StateFunction> MapState;

	int32 CurState = -1;
};

/*
 
///////FsmState생성하기///////

enum class FSM
{
	Example0,
	Example1,
	None, // None필수
}

//Fsm컴포넌트 받아오고
FsmComp = CreateDefaultSubobject<UFsmComponent>(TEXT("FsmComp"));

UFsmComponent::StateFunction ExampleState0;

ExampleState0.Start.BindLambda(
	[this]
	{

	});

ExampleState0.Update.BindLambda(
	[this](float DeltaTime)
	{

	});

ExampleState0.End.BindLambda(
	[this]
	{

	});

FsmComp->CreateState(Fsm::First, ExampleState0);
*/
