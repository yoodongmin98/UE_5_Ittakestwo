// Fill out your copyright notice in the Description page of Project Settings.


#include "Cody.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
ACody::ACody()
{
	
}

// Called when the game starts or when spawned
void ACody::BeginPlay()
{
	Super::BeginPlay();
	
	CodySizes = CodySize::NORMAL;
	ScaleSpeed = 2.0f;
	CameraSpeed = 1.5f;
	BigSize = FVector(4.0f, 4.0f, 4.0f);
	NormalSize = FVector(1.0f, 1.0f, 1.0f);
	SmallSize= FVector(0.2f, 0.2f, 0.2f);
	TargetScale = NormalSize;
	BigSizeCapsule = FVector(0.0f, 0.0f, -270.0f);
	NormalSizeCapsule = FVector(0.0f, 0.0f, -90.0f);
	SmallSizeCapsule = FVector(0.0f, 0.0f, 72.0f);

	GetMesh()->AddLocalOffset(NormalSizeCapsule);
}

// Called every frame
void ACody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Scale Check
	GetMesh()->SetWorldScale3D(FMath::VInterpTo(GetMesh()->GetComponentScale(), TargetScale, DeltaTime, ScaleSpeed));



	//Size Setting
	switch (CodySizes)
	{
	case CodySize::BIG :
	{
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, BigLength, DeltaTime, CameraSpeed);
		GetCapsuleComponent()->SetCapsuleSize(140.0f, 360.0f);
		break;
	}
	case CodySize::NORMAL:
	{
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, NormalLength, DeltaTime, CameraSpeed);
		GetCapsuleComponent()->SetCapsuleSize(35.0f, 90.0f);
		break;
	}
	case CodySize::SMALL:
	{
		SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, SmallLength, DeltaTime, CameraSpeed*2.0f);
		GetCapsuleComponent()->SetCapsuleSize(7.0f, 18.0f);
		break;
	}
	default :
		break;
	}
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
	//UE_LOG(LogTemp, Warning, TEXT("right function called"));
	switch (CodySizes)
	{
	case CodySize::BIG:
	{
		break;
	}
	case CodySize::NORMAL:
	{
		ChangeCodySizeEnum(CodySize::BIG);
		TargetScale = BigSize;
		GetMesh()->AddLocalOffset(BigSizeCapsule);
		break;
	}
	case CodySize::SMALL:
	{
		ChangeCodySizeEnum(CodySize::NORMAL);
		TargetScale = NormalSize;
		GetMesh()->AddLocalOffset(-SmallSizeCapsule);
		break;
	}
	default :
		break;
	}
}

void ACody::ChangeSmallSize()
{
	//UE_LOG(LogTemp, Warning, TEXT("left function called"));
	switch (CodySizes)
	{
	case CodySize::BIG :
	{
		ChangeCodySizeEnum(CodySize::NORMAL);
		TargetScale = NormalSize;
		GetMesh()->AddLocalOffset(-BigSizeCapsule);
		break;
	}
	case CodySize::NORMAL:
	{
		ChangeCodySizeEnum(CodySize::SMALL);
		TargetScale = SmallSize;
		GetMesh()->AddLocalOffset(SmallSizeCapsule);
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