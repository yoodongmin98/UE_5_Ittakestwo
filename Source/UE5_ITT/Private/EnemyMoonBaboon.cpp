// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyMoonBaboon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AEnemyMoonBaboon::AEnemyMoonBaboon()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		SetupComponent();
		bReplicates = true;
		SetReplicateMovement(true);
	}
}

// Called when the game starts or when spawned
void AEnemyMoonBaboon::BeginPlay()
{
	Super::BeginPlay();

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
	}
}

void AEnemyMoonBaboon::SetupComponent()
{
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(SceneComp);
	// GetArrowComponent()->SetupAttachment(SceneComp);
	// GetMesh()->SetupAttachment(SceneComp);
}

// Called every frame
void AEnemyMoonBaboon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		
	}
}

// Called to bind functionality to input
void AEnemyMoonBaboon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyMoonBaboon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 메시 컴포넌트를 Replication하기 위한 설정 추가
	DOREPLIFETIME(AEnemyMoonBaboon, SceneComp);
	DOREPLIFETIME(AEnemyMoonBaboon, SkeletalMeshComp);
}

