// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerBase.h"
#include "Cody.generated.h"





UENUM(BlueprintType)
enum class CodySize : uint8
{
	NONE UMETA(DisPlayName = "NONE"),
	BIG UMETA(DisPlayName = "BIG"),
	NORMAL UMETA(DisPlayName = "NORMAL"),
	SMALL UMETA(DisPlayName = "SMALL"),
};

UCLASS()
class UE5_ITT_API ACody : public APlayerBase
{
public:
	GENERATED_BODY()
	ACody();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	//CodySize Enum을 변경합니다
	void ChangeCodySizeEnum(CodySize _Enum)
	{
		CodySizes = _Enum;
	}
	//Cody의 Size를 Enum으로 반환합니다.
	UFUNCTION(BlueprintCallable)
	inline CodySize GetCodySize() const
	{
		return CodySizes;
	}
	//변환하고자하는 Size의 TargetScale을 반환합니다
	UFUNCTION(BlueprintCallable)
	inline FVector GetTargetScale() const
	{
		return TargetScale;
	}
	
	//UFUNCTION(Client, Reliable, WithValidation)


	UFUNCTION(Server, Reliable, WithValidation)
	void ChangeBigSize();

	//UFUNCTION(Client, Reliable, WithValidation)
	UFUNCTION(Server, Reliable, WithValidation)
	void ChangeSmallSize();



	UFUNCTION(Server, Reliable, WithValidation)
	void ChangeServerBigSize();

	UFUNCTION(Server, Reliable, WithValidation)
	void ChangeServerSmallSize();


	virtual void SprintInput() override;
	virtual void DashEnd() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Enum 값을 포함한 변수
	//UPROPERTY(Replicated, EditAnywhere, Replicated)
	CodySize CodySizes = CodySize::NONE;
	//Set Speed
	float ScaleSpeed;
	float CameraSpeed;
	//UPROPERTY(Replicated)
	FVector TargetScale;
	//Size
	FVector CustomBigSize;
	FVector CustomNormalSize;
	FVector CustomSmallSize;
	//FVector CustomBigSizeCapsule;
	//FVector CustomNormalSizeCapsule;
	//FVector CustomSmallSizeCapsule;
	//Transform
	FTransform CodyCapsuleComponent;
	FTransform CodyTransform;
	FTransform CodyLocalTransform;

	float CodyDefaultSpeed;
	float CodyDefaultJumpHeight;
	
	FVector BeforeSize;
	float LerpTime;
};
