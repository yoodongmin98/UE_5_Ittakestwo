// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcingProjectile.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BurstEffect.h"

// test
#include "Cody.h"

static int TestCount = 0;

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

	// test
	TestCount = 0;

	ProjectileMeshComp->OnComponentBeginOverlap.AddDynamic(this, &AArcingProjectile::OnOverlapBegin);
	ProjectileMeshComp->OnComponentEndOverlap.AddDynamic(this, &AArcingProjectile::OnOverlapEnd);
	ProjectileMeshComp->OnComponentHit.AddDynamic(this, &AArcingProjectile::OnHit);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

void AArcingProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	++TestCount;

	if (OtherActor != this)
	{
		FVector SettingLocation = GetActorLocation();

		// 오버랩시작시 버스트이펙트생성
		ABurstEffect* Effect = GetWorld()->SpawnActor<ABurstEffect>(BurstEffectClass, SettingLocation, FRotator::ZeroRotator);
		if (Effect != nullptr)
		{
			Effect->SetActorLocation(SettingLocation);
		}
	}
}

void AArcingProjectile::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

// Called every frame
void AArcingProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 타이머설정해두고 0.2초 이후에 
	// 오버랩세팅 하게 해두면 해결되지 않을까?


	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		
	}
	// 여기서 한번만 호출되게 ? 
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

void AArcingProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 여기서 충돌했을 때 원형으로 커지는 구체 생성.
	ABurstEffect* Effect = GetWorld()->SpawnActor<ABurstEffect>(ABurstEffect::StaticClass());
	Effect->SetActorLocation(GetActorLocation());
}

