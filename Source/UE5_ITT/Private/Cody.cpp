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

	CodyDefaultJumpHeight = GetCharacterMovement()->JumpZVelocity;
	

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
	if (!GetCharacterMovement()->IsFalling())
	{
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
			ACharacter::JumpMaxCount = 1;
			BigCanDash = false;
			GetCharacterMovement()->GravityScale = DefaultGravityScale + 1.0f;
			break;
		}
		case CodySize::SMALL:
		{
			ChangeCodySizeEnum(CodySize::NORMAL);
			TargetScale = NormalSize;
			GetMesh()->AddLocalOffset(-SmallSizeCapsule);
			GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed;
			GetCharacterMovement()->GravityScale = DefaultGravityScale;
			GetCharacterMovement()->JumpZVelocity = CodyDefaultJumpHeight;
			DashDistance = 2500.0f;
			break;
		}
		default:
			break;
		}
	}
}

void ACody::ChangeSmallSize()
{
	//UE_LOG(LogTemp, Warning, TEXT("left function called"));
	if (!GetCharacterMovement()->IsFalling())
	{
		switch (CodySizes)
		{
		case CodySize::BIG:
		{
			ChangeCodySizeEnum(CodySize::NORMAL);
			TargetScale = NormalSize;
			GetMesh()->AddLocalOffset(-BigSizeCapsule);
			ACharacter::JumpMaxCount = 2;
			BigCanDash = true;
			GetCharacterMovement()->GravityScale = DefaultGravityScale;
			break;
		}
		case CodySize::NORMAL:
		{
			ChangeCodySizeEnum(CodySize::SMALL);
			TargetScale = SmallSize;
			GetMesh()->AddLocalOffset(SmallSizeCapsule);
			GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed - 400.0f;
			GetCharacterMovement()->GravityScale = DefaultGravityScale - 2.0f;
			GetCharacterMovement()->JumpZVelocity = 700.0f;
			DashDistance = 1000.0f;
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
}

void ACody::SprintInput()
{
	CodySprint = true;
	switch (CodySizes)
	{
	case CodySize::BIG:
	{
		break;
	}
	case CodySize::NORMAL:
	{
		GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed + 500.0f;
		break;
	}
	case CodySize::SMALL:
	{
		GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed - 400.0f;
		break;
	}
	default:
		break;
	}
}
void ACody::SprintNoneInput()
{
	CodySprint = false;
	switch (CodySizes)
	{
	case CodySize::BIG:
	{
		break;
	}
	case CodySize::NORMAL:
	{
		GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed;
		break;
	}
	case CodySize::SMALL:
	{
		GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed - 600.0f;
		break;
	}
	default:
		break;
	}
}

void ACody::DashEnd()
{
	//대쉬가 끝나면 기본으로 적용된 중력,지면 마찰력으로 다시 적용
	switch (CodySizes)
	{
	case CodySize::BIG:
	{
		break;
	}
	case CodySize::NORMAL:
	{
		GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
		GetCharacterMovement()->GravityScale = DefaultGravityScale;
		break;
	}
	case CodySize::SMALL:
	{
		GetCharacterMovement()->GroundFriction = DefaultGroundFriction - 20.0f;
		GetCharacterMovement()->GravityScale = DefaultGravityScale - 2.0f;
		break;
	}
	default:
		break;
	}
	//땅에 닿았을때만 대쉬 재시작 가능
	if (!GetCharacterMovement()->IsFalling())
	{
		bIsDashing = false;
		bIsDashingStart = false;
		bCanDash = false;
		DashDuration = 0.7f;
	}
}