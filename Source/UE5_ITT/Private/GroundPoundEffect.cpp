// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundPoundEffect.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "PlayerBase.h"



// Sets default values
AGroundPoundEffect::AGroundPoundEffect()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);

		SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
		SetRootComponent(SceneComp);

		StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
		StaticMeshComp->SetupAttachment(SceneComp);
		
		GroundPoundEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GroundPoundEffectComponent"));
		GroundPoundEffectComp->SetupAttachment(SceneComp);
	}
}

// Called when the game starts or when spawned
void AGroundPoundEffect::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		if (nullptr != StaticMeshComp)
		{
			StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AGroundPoundEffect::OnOverlapBegin);
			StaticMeshComp->OnComponentEndOverlap.AddDynamic(this, &AGroundPoundEffect::OnOverlapEnd);
		}
	}
}

// Called every frame
void AGroundPoundEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (true == HasAuthority())
	{
		if (true == bIsPlayerOverlap)
		{
			// OverlapPlayer->AttackPlayer(DamageToPlayer);
		}
	}
}


void AGroundPoundEffect::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGroundPoundEffect, bIsPlayerOverlap);
}

void AGroundPoundEffect::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (nullptr != OtherActor)
	{
		if (true == OtherActor->ActorHasTag(TEXT("Player")))
		{
			if (true == OtherActor->HasAuthority())
			{
				OtherComp->GetName();
				
				UE_LOG(LogTemp, Display, TEXT("Otherdasdfdasadfa f"));
				UE_LOG(LogTemp, Display, TEXT("Player Overlap true"));
				bIsPlayerOverlap = true;
				OverlapPlayer = Cast<APlayerBase>(OtherActor);
			}
		}
	}
}

void AGroundPoundEffect::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (nullptr != OtherActor)
	{
		if (true == OtherActor->ActorHasTag(TEXT("Player")))
		{
			if (true == OtherActor->HasAuthority())
			{
				UE_LOG(LogTemp, Display, TEXT("Player Overlap false"));
				bIsPlayerOverlap = false;
				OverlapPlayer = nullptr;
			}
			
		}
	}
}

