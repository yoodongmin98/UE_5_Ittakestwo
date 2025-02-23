// Fill out your copyright notice in the Description page of Project Settings.


#include "May.h"


// Sets default values
AMay::AMay()
{
	Tags.Add(FName("May"));
}


// Called when the game starts or when spawned
void AMay::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMay::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed + 500.0f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed;
	}
}

void AMay::DashEnd()
{
	//대쉬가 끝나면 기본으로 적용된 중력,지면 마찰력으로 다시 적용
	GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
	GetCharacterMovement()->GravityScale = DefaultGravityScale;
	//땅에 닿았을때만 대쉬 재시작 가능
	if (!GetCharacterMovement()->IsFalling())
	{
		bIsDashing = false;
		bIsDashingStart = false;
		bCanDash = false;
		DashDuration = 0.7f;
	}
}

void AMay::SprintInput()
{
	ClientSprintInput();
	ServerSprintInput();
}

void AMay::ClientSprintInput_Implementation()
{
	IsSprint = true;
}
bool AMay::ServerSprintInput_Validate()
{
	return true;
}
void AMay::ServerSprintInput_Implementation()
{
	IsSprint = true;
}
