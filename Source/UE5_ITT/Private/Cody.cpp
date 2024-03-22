// Fill out your copyright notice in the Description page of Project Settings.


#include "Cody.h"

// Sets default values
ACody::ACody()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACody::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACody::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACody::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Player_MoveFB"), this, &ACody::Cody_MoveFB);
	PlayerInputComponent->BindAxis(TEXT("Player_MoveLR"), this, &ACody::Cody_MoveLR);
	PlayerInputComponent->BindAxis(TEXT("Player_LookUD"), this, &ACody::Cody_LookUD);
	PlayerInputComponent->BindAxis(TEXT("Player_LookLR"), this, &ACody::Cody_LookLR);
	PlayerInputComponent->BindAction(TEXT("Player_Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	
}


//////////////////////////////캐릭터 이동관련///////////////////////////
void ACody::Cody_MoveFB(float _AxisValue)
{
	IsMoveEnd = false;
	AddMovementInput(GetActorForwardVector() * _AxisValue);
}
void ACody::Cody_MoveLR(float _AxisValue)
{
	AddMovementInput(GetActorRightVector() * _AxisValue);
}

void ACody::Cody_LookUD(float _AxisValue)
{
	AddControllerPitchInput(_AxisValue * UpRotationSpeed * GetWorld()->GetDeltaSeconds());
}

void ACody::Cody_LookLR(float _AxisValue)
{
	AddControllerYawInput(_AxisValue * RightRotationSpeed * GetWorld()->GetDeltaSeconds());
}

//////////////FSM//////////
void ACody::ChangeState(Cody_State _State)
{
	CodyState = _State;
}