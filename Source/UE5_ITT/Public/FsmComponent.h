// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FsmComponent.generated.h"

DECLARE_DELEGATE(StateFunction);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5_ITT_API UFsmComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFsmComponent();

	class State
	{
	public:
		StateFunction Start;
		StateFunction Update;
		StateFunction End;
	};

	template<class T> 
	void InitState(T Index)
	{
		StateArray.SetNum(static_cast<int32>(Index));
	};

	void FsmTick();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	TArray<State> StateArray;
	int32 CurState = -1;
	int32 PrevState = -1;
};
