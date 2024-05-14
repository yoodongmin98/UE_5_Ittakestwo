// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcingProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "EnemyFlyingSaucer.h"
#include "BurstEffect.h"
#include "Floor.h"

// Sets default values
AArcingProjectile::AArcingProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		bReplicates = true;
		SetReplicateMovement(true);
		SetupComponent();
	}
}

// Called when the game starts or when spawned
void AArcingProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		SetupOverlapEvent();
	}
}

void AArcingProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{

	}
}

void AArcingProjectile::SetupComponent()
{
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComp);

	ProjectileMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMeshComp->SetupAttachment(SceneComp);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComp->SetUpdatedComponent(RootComponent);
	ProjectileMovementComp->InitialSpeed = 400.0f; // Initial speed (will be set based on suggested velocity)
	ProjectileMovementComp->ProjectileGravityScale = 1.0f; // Gravity scale (if you want to override global gravity)
	ProjectileMovementComp->bShouldBounce = false; // Set to true if you want the projectile to bounce

	TrailEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffectComponent"));
	TrailEffectComp->SetupAttachment(SceneComp);
}

void AArcingProjectile::SetupProjectileMovementComponent()
{
	if (nullptr != ProjectileMovementComp)
	{
		FVector StartLocation = GetActorLocation(); // 시작 위치
		StartLocation.Z += 500.0f;
		float ArcValue = 0.62f; // ArcParam (0.0-1.0)
		FVector OutVelocity = FVector::ZeroVector;
		if (UGameplayStatics::SuggestProjectileVelocity_CustomArc(this, OutVelocity, StartLocation, TargetLocation, GetWorld()->GetGravityZ(), ArcValue))
		{
			ProjectileMovementComp->Velocity = OutVelocity;
		}
	}
}

void AArcingProjectile::SetupOverlapEvent()
{
	if (nullptr != ProjectileMeshComp)
	{
		ProjectileMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AArcingProjectile::OnOverlapBegin);
		ProjectileMeshComp->OnComponentEndOverlap.AddDynamic(this, &AArcingProjectile::OnOverlapEnd);
	}
}

void AArcingProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (nullptr == OtherActor)
	{
		return;
	}

	if (false == OtherActor->ActorHasTag(TEXT("Floor")))
	{
		return;
	}

	if (false == bIsOverlapEvent)
	{
		FVector SettingLocation = GetActorLocation();
		ABurstEffect* Effect = GetWorld()->SpawnActor<ABurstEffect>(BurstEffectClass, SettingLocation, FRotator::ZeroRotator);
		if (Effect != nullptr)
		{
			// Effect->SetActorLocation(SettingLocation);

			AEnemyFlyingSaucer* ParentActor = Cast<AEnemyFlyingSaucer>(GetOwner());
			if (nullptr != ParentActor)
			{
				AActor* FloorActor = Cast<AActor>(ParentActor->GetFloor());
				Effect->AttachToActor(FloorActor, FAttachmentTransformRules::KeepWorldTransform);
			}
		}

		bIsOverlapEvent = true;
		Destroy();
	}
}

void AArcingProjectile::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}



