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
}

// Called every frame
void ACody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACody::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* PlayerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput != nullptr)
	{
		PlayerInput->BindAction(LeftMAction, ETriggerEvent::Triggered, this, &ACody::ChangeSmallSize);
		PlayerInput->BindAction(RightMAction, ETriggerEvent::Triggered, this, &ACody::ChangeBigSize);
	}
}



void ACody::ChangeBigSize()
{
	switch (CodySizes)
	{
	case CodySize::BIG:
	{
		return;
		break;
	}
	case CodySize::NORMAL:
	{
		Change_Normal_To_Big();
		break;
	}
	case CodySize::SMALL:
	{
		Change_Small_To_Normal();
	}
	default :
		//대충 에러코드
		break;
	}
}

void ACody::ChangeSmallSize()
{
	switch (CodySizes)
	{
	case CodySize::BIG:
	{
		Change_Big_To_Normal();
	}
	case CodySize::NORMAL:
	{
		Change_Normal_To_Small();
		break;
	}
	case CodySize::SMALL:
	{
		return;
		break;
	}
	default:
		//대충 에러코드
		break;
	}
}


void ACody::Change_Small_To_Normal()	  
{

}
void ACody::Change_Normal_To_Big()		  
{

}
void ACody::Change_Big_To_Normal()		  
{

}
void ACody::Change_Normal_To_Small()
{

}