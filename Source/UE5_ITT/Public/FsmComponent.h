// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include <functional>

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
	class FStateParam
	{
	public:
		StartFunction Start = nullptr;
		UpdateFunction Update = nullptr;
		EndFunction End = nullptr;
	};

	template<class T>
	void CreateState(T Index, 
		std::function<void()> BindStart,
		std::function<void(float)> BindUpdate,
		std::function<void()> BindEnd)
	{
		CreateState(static_cast<int32>(Index), BindStart, BindUpdate, BindEnd);
	};

	void CreateState(int Index,
		std::function<void()> BindStart,
		std::function<void(float)> BindUpdate,
		std::function<void()> BindEnd)
	{
		MapState.FindOrAdd(Index);
		MapState[Index].Start.BindLambda(BindStart);
		MapState[Index].Update.BindLambda(BindUpdate);
		MapState[Index].End.BindLambda(BindEnd);
	};


	template<class T>
	void ChangeState(T Index)
	{
		ChangeState(static_cast<int32>(Index));
	};

	void ChangeState(int32 Index);

	void FsmTick(float DT);

	float GetStateLiveTime()
	{
		return StateLiveTime;
	}

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentState() const
	{
		return CurState;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	TMap<int32, FStateParam> MapState;
	int32 CurState = -1;

	float StateLiveTime = 0.f;
};
