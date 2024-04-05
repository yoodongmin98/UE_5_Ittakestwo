// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBase.h"
#include "Cody.generated.h"





UENUM(BlueprintType)
enum class CodySize
{
	NONE UMETA(DisPlayName = "BIG"),
	BIG UMETA(DisPlayName = "BIG"),
	NORMAL UMETA(DisPlayName = "NORMAL"),
	SMALL UMETA(DisPlayName = "SMALL"),
};

UCLASS()
class UE5_ITT_API ACody : public APlayerBase
{
private:
	GENERATED_BODY()
	ACody();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;


	void ChangeBigSize();
	void ChangeSmallSize();

	void Change_Small_To_Normal();
	void Change_Normal_To_Big();
	void Change_Big_To_Normal();
	void Change_Normal_To_Small();
	CodySize CodySizes = CodySize::NONE;
};
