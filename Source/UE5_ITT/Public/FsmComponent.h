// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include <functional>

#include "FsmComponent.generated.h"


DECLARE_DELEGATE(StartFunction)
DECLARE_DELEGATE_OneParam(UpdateFunction,float)
DECLARE_DELEGATE(EndFunction)

USTRUCT(Atomic)
struct FBindParam
{
	GENERATED_USTRUCT_BODY()

	std::function<void()> BindStart = nullptr;
	std::function<void(float)> BindUpdate = nullptr;
	std::function<void()> BindEnd = nullptr;
};

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
	void CreateState(T Index, FBindParam BindParam)
	{
		CreateState(static_cast<int32>(Index), BindParam);
	};

	void CreateState(int Index, FBindParam BindParam)
	{
		FStateParam StateParam = MapState.FindOrAdd(Index);
		StateParam.Start.BindLambda(BindParam.BindStart);
		StateParam.Update.BindLambda(BindParam.BindUpdate);
		StateParam.End.BindLambda(BindParam.BindEnd);
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

private:
	TMap<int32, FStateParam> MapState;

	int32 CurState = -1;
};
