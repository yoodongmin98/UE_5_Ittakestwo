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
	
	CodySizes = CodySize::NORMAL;
	ScaleSpeed = 2.0f;
	CameraSpeed = 1.5f;
	CustomBigSize = FVector(4.0f, 4.0f, 4.0f);
	BeforeSize = FVector(1.0f, 1.0f, 1.0f);;
	CustomNormalSize = FVector(1.0f, 1.0f, 1.0f);
	CustomSmallSize= FVector(0.2f, 0.2f, 0.2f);
	TargetScale = CustomNormalSize;
	//CustomBigSizeCapsule = FVector(0.0f, 0.0f, -270.0f);
	//CustomNormalSizeCapsule = FVector(0.0f, 0.0f, -90.0f);
	//CustomSmallSizeCapsule = FVector(0.0f, 0.0f, 72.0f);

	CodyDefaultJumpHeight = GetCharacterMovement()->JumpZVelocity;
	
	//GetMesh()->AddLocalOffset(NormalSizeCapsule);
	LerpTime = 0.0f;
}

// Called every frame
void ACody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Scale Check
	if (true == HasAuthority())
	{
		//LerpTime += DeltaTime;
		//if (TargetScale != BeforeSize)
		//{
		//	if (LerpTime > 1.0f)
		//	{
		//		LerpTime = 1.0f;
		//	}
		//	FVector NextScale = FMath::Lerp(BeforeSize, TargetScale, LerpTime);
		//	GetCapsuleComponent()->SetWorldScale3D(NextScale);
		//}
		//switch (CodySizes)
		//{
		//case CodySize::BIG:
		//{
		//	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, BigLength, DeltaTime, CameraSpeed);
		//	GetCapsuleComponent()->SetWorldScale3D(NewScale);
		//	//GetCapsuleComponent()->SetCapsuleSize(140.0f, 360.0f);
		//	//SetCharacterSize();
		//	break;
		//}
		//case CodySize::NORMAL:
		//{
		//	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, NormalLength, DeltaTime, CameraSpeed);
		//	GetCapsuleComponent()->SetCapsuleSize(35.0f, 90.0f);
		//	break;
		//}
		//case CodySize::SMALL:
		//{
		//	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, SmallLength, DeltaTime, CameraSpeed * 2.0f);
		//	GetCapsuleComponent()->SetCapsuleSize(7.0f, 18.0f);
		//	if (GetCharacterMovement()->IsFalling())
		//	{
		//		DashDuration = 0.03f;
		//	}
		//	break;
		//}
		//default:
		//	break;
		//}
		// 
		//GetRootComponent()
		//GetCapsuleComponent()->SetRelativeScale3D(FMath::VInterpTo(GetRootComponent()->GetComponentScale(), TargetScale, DeltaTime, ScaleSpeed));


		//if (GetRootComponent()->GetComponentScale() == TargetScale)
		//{
		//	int a = 0;
		//	UE_LOG(LogTemp, Display, TEXT("ee"));
		//}
		//GetCapsuleComponent()->SetCapsuleSize(GetCapsuleComponent()->GetScaledCapsuleRadius(), GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	}
	else
	{
	}
	


		//switch (CodySizes)
		//{
		//case CodySize::BIG:
		//{
		//	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, BigLength, DeltaTime, CameraSpeed);
		//	GetCapsuleComponent()->SetCapsuleSize(140.0f, 360.0f);
		//	break;
		//}
		//case CodySize::NORMAL:
		//{
		//	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, NormalLength, DeltaTime, CameraSpeed);
		//	GetCapsuleComponent()->SetCapsuleSize(35.0f, 90.0f);
		//	break;
		//}
		//case CodySize::SMALL:
		//{
		//	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, SmallLength, DeltaTime, CameraSpeed * 2.0f);
		//	GetCapsuleComponent()->SetCapsuleSize(7.0f, 18.0f);
		//	if (GetCharacterMovement()->IsFalling())
		//	{
		//		DashDuration = 0.03f;
		//	}
		//	break;
		//}
		//default:
		//	break;
		//}

		

	//SPRINT
	if (IsSprint)
	{
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
			GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed - 500.0f;
			break;
		}
		default:
			break;
		}
	}
	else
	{
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



bool ACody::ChangeBigSize_Validate()
{
	return true;
}
void ACody::ChangeBigSize_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("right function called"));
	if (!GetCharacterMovement()->IsFalling())
	{
		IsSprint = false;
		BeforeSize = GetCapsuleComponent()->GetComponentScale();
		switch (CodySizes)
		{
		case CodySize::BIG:
		{
			break;
		}
		case CodySize::NORMAL:
		{
			CodySizes = CodySize::BIG;
			//ChangeCodySizeEnum(CodySize::BIG);
			TargetScale = CustomBigSize;
			//GetMesh()->AddLocalOffset(BigSizeCapsule);
			ACharacter::JumpMaxCount = 1;
			BigCanDash = false;
			GetCharacterMovement()->GravityScale = DefaultGravityScale + 1.0f;
			LerpTime = 0.0f;

			break;
		}
		case CodySize::SMALL:
		{
			//ChangeCodySizeEnum(CodySize::NORMAL);
			CodySizes = CodySize::NORMAL;
			TargetScale = CustomNormalSize;
			//GetMesh()->AddLocalOffset(-SmallSizeCapsule);
			GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed;
			GetCharacterMovement()->GravityScale = DefaultGravityScale;
			GetCharacterMovement()->JumpZVelocity = CodyDefaultJumpHeight;
			DashDistance = 2500.0f;
			LerpTime = 0.0f;

			break;
		}
		default:
			break;
		}
		GetCapsuleComponent()->SetWorldScale3D(TargetScale);

	}
}

bool ACody::ChangeSmallSize_Validate()
{
	return true;
}
void ACody::ChangeSmallSize_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("left function called"));
	if (!GetCharacterMovement()->IsFalling())
	{
		BeforeSize = GetCapsuleComponent()->GetComponentScale();

		IsSprint = false;
		switch (CodySizes)
		{
		case CodySize::BIG:
		{
			//ChangeCodySizeEnum(CodySize::NORMAL);
			CodySizes = CodySize::NORMAL;

			TargetScale = CustomNormalSize;
			//GetMesh()->AddLocalOffset(-BigSizeCapsule);
			ACharacter::JumpMaxCount = 2;
			BigCanDash = true;
			GetCharacterMovement()->GravityScale = DefaultGravityScale;
			LerpTime = 0.0f;

			break;
		}
		case CodySize::NORMAL:
		{
			//ChangeCodySizeEnum(CodySize::SMALL);
			CodySizes = CodySize::SMALL;

			TargetScale = CustomSmallSize;
			//GetMesh()->AddLocalOffset(SmallSizeCapsule);
			GetCharacterMovement()->GravityScale = DefaultGravityScale - 3.5f;
			GetCharacterMovement()->JumpZVelocity = 250.0f;
			DashDistance = 700.0f;
			LerpTime = 0.0f;

			break;
		}
		case CodySize::SMALL:
		{
			break;
		}
		default:
			break;
		}
		GetCapsuleComponent()->SetWorldScale3D(TargetScale);

	}
}

bool ACody::ChangeServerBigSize_Validate()
{
	return true;
}

void ACody::ChangeServerBigSize_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("right function called"));
	if (!GetCharacterMovement()->IsFalling())
	{
		BeforeSize = GetCapsuleComponent()->GetComponentScale();

		IsSprint = false;
		switch (CodySizes)
		{
		case CodySize::BIG:
		{
			break;
		}
		case CodySize::NORMAL:
		{
			//ChangeCodySizeEnum(CodySize::BIG);
			CodySizes = CodySize::BIG;

			TargetScale = CustomBigSize;
			//GetMesh()->AddLocalOffset(BigSizeCapsule);
			ACharacter::JumpMaxCount = 1;
			BigCanDash = false;
			GetCharacterMovement()->GravityScale = DefaultGravityScale + 1.0f;
			LerpTime = 0.0f;

			break;
		}
		case CodySize::SMALL:
		{
			//ChangeCodySizeEnum(CodySize::NORMAL);
			CodySizes = CodySize::NORMAL;

			TargetScale = CustomNormalSize;
			//GetMesh()->AddLocalOffset(-SmallSizeCapsule);
			GetCharacterMovement()->MaxWalkSpeed = PlayerDefaultSpeed;
			GetCharacterMovement()->GravityScale = DefaultGravityScale;
			GetCharacterMovement()->JumpZVelocity = CodyDefaultJumpHeight;
			DashDistance = 2500.0f;
			LerpTime = 0.0f;

			break;
		}
		default:
			break;
		}
		GetCapsuleComponent()->SetWorldScale3D(TargetScale);

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
		BeforeSize = GetCapsuleComponent()->GetComponentScale();

		IsSprint = false;
		switch (CodySizes)
		{
		case CodySize::BIG:
		{
			//ChangeCodySizeEnum(CodySize::NORMAL);
			CodySizes = CodySize::NORMAL;

			TargetScale = CustomNormalSize;
			//GetMesh()->AddLocalOffset(-BigSizeCapsule);
			ACharacter::JumpMaxCount = 2;
			BigCanDash = true;
			GetCharacterMovement()->GravityScale = DefaultGravityScale;
			LerpTime = 0.0f;

			break;
		}
		case CodySize::NORMAL:
		{
			//ChangeCodySizeEnum(CodySize::SMALL);
			CodySizes = CodySize::SMALL;

			TargetScale = CustomSmallSize;
			//GetMesh()->AddLocalOffset(SmallSizeCapsule);
			GetCharacterMovement()->GravityScale = DefaultGravityScale - 3.5f;
			GetCharacterMovement()->JumpZVelocity = 250.0f;
			DashDistance = 700.0f;
			LerpTime = 0.0f;

			break;
		}
		case CodySize::SMALL:
		{
			break;
		}
		default:
			break;
		}
		GetCapsuleComponent()->SetWorldScale3D(TargetScale);

	}
}

void ACody::SprintInput()
{
	IsSprint = !IsSprint;
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

// 소스 파일
void ACody::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// MyVariable을 Replicate할 것임을 엔진에게 알려줍니다.
	//DOREPLIFETIME(ACody, CodySizes);
	//DOREPLIFETIME(ACody, GetCapsuleComponent());
	//DOREPLIFETIME(ACody, TargetScale);
}