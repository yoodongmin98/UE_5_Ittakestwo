// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcingProjectile.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BurstEffect.h"
#include "Floor.h"
#include "EnemyFlyingSaucer.h"

// test
#include "Cody.h"

// Sets default values
AArcingProjectile::AArcingProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

// Called when the game starts or when spawned
void AArcingProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetupOverlapEvent();

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		SetReplicates(true);
		SetReplicateMovement(true);
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
	// 오버랩 대상이 바닥이 아니라면 return;
	AFloor* OVerlapOtherActor = Cast<AFloor>(OtherActor);
	if (nullptr == OVerlapOtherActor)
	{
		return;
	}

	if (OtherActor != this && bIsOverlapEvent == false)
	{
		FVector SettingLocation = GetActorLocation();
		ABurstEffect* Effect = GetWorld()->SpawnActor<ABurstEffect>(BurstEffectClass, SettingLocation, FRotator::ZeroRotator);
		if (Effect != nullptr)
		{
			Effect->SetActorLocation(SettingLocation);

			AEnemyFlyingSaucer* ParentActor = Cast<AEnemyFlyingSaucer>(GetOwner());
			if (nullptr != ParentActor)
			{
				AActor* FloorActor = Cast<AActor>(ParentActor->GetFloor());
				Effect->AttachToActor(FloorActor, FAttachmentTransformRules::KeepWorldTransform);
			}
		}

		bIsOverlapEvent = true;
	}
}

void AArcingProjectile::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AArcingProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		
	}
}

void AArcingProjectile::SetupProjectileMovementComponent()
{
	// 이후에 플레이어 두명되면 변경해야함 
	if (nullptr != ProjectileMovementComp)
	{
		TargetLocation = Cast<ACody>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0))->GetActorLocation();
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

