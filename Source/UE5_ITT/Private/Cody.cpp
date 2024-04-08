// Fill out your copyright notice in the Description page of Project Settings.


#include "Cody.h"



// Sets default values
ACody::ACody()
{
	
}

// Called when the game starts or when spawned
void ACody::BeginPlay()
{
	Super::BeginPlay();
	
	CodySizes = CodySize::NORMAL;
	ScaleSpeed = 0.5f;
	BigSize = FVector(2.0f, 2.0f, 2.0f);
	NormalSize = FVector(1.0f, 1.0f, 1.0f);
	SmallSize= FVector(0.5f, 0.5f, 0.5f);
	TargetScale = NormalSize;
}

// Called every frame
void ACody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FVector NewScale = FMath::Lerp(GetActorScale3D(), TargetScale, ScaleSpeed * DeltaTime);
	GetMesh()->SetWorldScale3D(TargetScale);
}

void ACody::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInput != nullptr)
	{
		PlayerInput->BindAction(LeftMAction, ETriggerEvent::Triggered, this, &ACody::ChangeSmallSize);
		PlayerInput->BindAction(RightMAction, ETriggerEvent::Triggered, this, &ACody::ChangeBigSize);
	}
}



void ACody::ChangeBigSize()
{
	UE_LOG(LogTemp, Warning, TEXT("right function called"));
	switch (CodySizes)
	{
	case CodySize::BIG:
	{
		break;
	}
	case CodySize::NORMAL:
	{
		ChangeCodySizeEnum(CodySize::BIG);
		TargetScale = FVector(2.0f, 2.0f, 2.0f);
		break;
	}
	case CodySize::SMALL:
	{
		ChangeCodySizeEnum(CodySize::NORMAL);
		TargetScale = FVector(1.0f, 1.0f, 1.0f);
		break;
	}
	default :
		break;
	}
}

void ACody::ChangeSmallSize()
{
	UE_LOG(LogTemp, Warning, TEXT("left function called"));
	switch (CodySizes)
	{
	case CodySize::BIG :
	{
		ChangeCodySizeEnum(CodySize::NORMAL);
		TargetScale = FVector(1.0f, 1.0f, 1.0f);
		break;
	}
	case CodySize::NORMAL:
	{
		ChangeCodySizeEnum(CodySize::SMALL);
		TargetScale = FVector(0.5f, 0.5f, 0.5f);
		break;
	}
	case CodySize::SMALL:
	{
		break;
	}
	default:
		break;
	}
}