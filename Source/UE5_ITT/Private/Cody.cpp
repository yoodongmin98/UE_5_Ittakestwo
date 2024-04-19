// Fill out your copyright notice in the Description page of Project Settings.


#include "Cody.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACody::ACody()
{
	Tags.Add(FName("Cody"));
}

// Called when the game starts or when spawned
void ACody::BeginPlay()
{
	Super::BeginPlay();
	
	CurCodySize = CodySize::NORMAL;
	NextCodySize = CodySize::NORMAL;

	BigSize = FVector(4.0f, 4.0f, 4.0f);
	NormalSize = FVector(1.0f, 1.0f, 1.0f);
	SmallSize= FVector(0.1111111111f, 0.1111111111f, 0.1111111111f);
	TargetScale = NormalSize;
	CodyDefaultJumpHeight = GetCharacterMovement()->JumpZVelocity;
	

}

// Called every frame
void ACody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GetCapsuleComponent()->MarkRenderStateDirty();

	//Scale Check
	if (true == HasAuthority())
	{

		if (CurCodySize != NextCodySize)
		{
			switch (NextCodySize)
			{
			case CodySize::NONE:
				break;
			case CodySize::BIG:
			{
				ACharacter::JumpMaxCount = 1;
				BigCanDash = false;
				GetCharacterMovement()->GravityScale = DefaultGravityScale + 1.0f;
				GetCapsuleComponent()->SetWorldScale3D(BigSize);
				GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed;
				SpringArm->TargetArmLength = BigLength;
				break;
			}
			case CodySize::NORMAL:
			{
				BigCanDash = true;
				ACharacter::JumpMaxCount = 2;
				GetCharacterMovement()->GravityScale = DefaultGravityScale;
				GetCharacterMovement()->JumpZVelocity = CodyDefaultJumpHeight;
				DashDistance = 2500.0f;
				GetCapsuleComponent()->SetWorldScale3D(NormalSize);
				SpringArm->TargetArmLength = NormalLength;
				break;
			}
			case CodySize::SMALL:
			{
				GetCharacterMovement()->GravityScale = DefaultGravityScale - 3.5f;
				GetCharacterMovement()->JumpZVelocity = 250.0f;
				DashDistance = 700.0f;
				GetCapsuleComponent()->SetWorldScale3D(SmallSize);
				SpringArm->TargetArmLength = SmallLength;
				break;
			}
			default:
				break;
			}
			CurCodySize = NextCodySize;
		}
	}
	else
	{
		GetMesh()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}
	
	//SPRINT
	if (IsSprint)
	{
		switch (CurCodySize)
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
			GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed - 500.0f;
			break;
		}
		default:
			break;
		}
	}
	else
	{
		switch (CurCodySize)
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
			GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed - 700.0f;
			break;
		}
		default:
			break;
		}
	}
}

void ACody::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (PlayerInput != nullptr)
	{
		if(true == HasAuthority())
		{
			PlayerInput->BindAction(LeftMAction, ETriggerEvent::Triggered, this, &ACody::ChangeSmallSize);
			PlayerInput->BindAction(RightMAction, ETriggerEvent::Triggered, this, &ACody::ChangeBigSize);
		}
		else
		{
			PlayerInput->BindAction(LeftMAction, ETriggerEvent::Triggered, this, &ACody::ChangeServerSmallSize);
			PlayerInput->BindAction(RightMAction, ETriggerEvent::Triggered, this, &ACody::ChangeServerBigSize);
		}
	}
}



void ACody::ChangeBigSize_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("right function called"));
	if (!GetCharacterMovement()->IsFalling())
	{
		IsSprint = false;
		switch (CurCodySize)
		{
		case CodySize::BIG:
		{
			break;
		}
		case CodySize::NORMAL:
		{
			NextCodySize = CodySize::BIG;
			break;
		}
		case CodySize::SMALL:
		{
			NextCodySize = CodySize::NORMAL;
			break;
		}
		default:
			break;
		}
	}
}

void ACody::ChangeSmallSize_Implementation()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		IsSprint = false;
		switch (CurCodySize)
		{
		case CodySize::BIG:
		{
			NextCodySize = CodySize::NORMAL;
			break;
		}
		case CodySize::NORMAL:
		{
			NextCodySize = CodySize::SMALL;
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

bool ACody::ChangeServerBigSize_Validate()
{
	return true;
}

void ACody::ChangeServerBigSize_Implementation()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		IsSprint = false;
		switch (CurCodySize)
		{
		case CodySize::BIG:
		{
			break;
		}
		case CodySize::NORMAL:
		{
			NextCodySize = CodySize::BIG;
			break;
		}
		case CodySize::SMALL:
		{
			NextCodySize = CodySize::NORMAL;
			break;
		}
		default:
			break;
		}
	}
}
	

bool ACody::ChangeServerSmallSize_Validate()
{
	return true;
}
void ACody::ChangeServerSmallSize_Implementation()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		IsSprint = false;
		switch (CurCodySize)
		{
		case CodySize::BIG:
		{
			NextCodySize = CodySize::NORMAL;
			break;
		}
		case CodySize::NORMAL:
		{
			NextCodySize = CodySize::SMALL;
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
	IsSprint = !IsSprint;
}


void ACody::DashEnd()
{
	//대쉬가 끝나면 기본으로 적용된 중력,지면 마찰력으로 다시 적용
	switch (CurCodySize)
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
		GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed - 700;
		GetCharacterMovement()->GroundFriction = DefaultGroundFriction - 45.0f;
		GetCharacterMovement()->GravityScale = DefaultGravityScale - 3.5f;
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
		DashDuration = 0.75f;
	}
}
