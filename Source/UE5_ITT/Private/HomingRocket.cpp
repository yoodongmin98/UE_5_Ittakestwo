// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingRocket.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "NiagaraComponent.h"
#include "EnemyFlyingSaucer.h"
#include "ExplosionEffect.h"
#include "Floor.h"
#include "Cody.h"


// Sets default values
AHomingRocket::AHomingRocket()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComp);

	RocketMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RocketMesh"));
	RocketMeshComp->SetupAttachment(SceneComp);

	FireEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireEffectComponent"));
	FireEffectComp->SetupAttachment(SceneComp);
}

// Called when the game starts or when spawned
void AHomingRocket::BeginPlay()
{
	Super::BeginPlay();

	// 임시, 수정예정
	PlayerCodyRef = Cast<ACody>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	RocketMeshComp->OnComponentHit.AddDynamic(this, &AHomingRocket::OnHit);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		SetReplicates(true);
		SetReplicateMovement(true);
	}
}

// Called every frame
void AHomingRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 일단 액티브 상태가 아니면 리턴하게
		if (false == bIsActive)
		{
			return;
		}

		if (nullptr == PlayerCodyRef || 0.0f >= RocketLifeTime)
		{
			// 테스트코드. 비활성 함수를 만들고, 비활성 상태로 변경한다. 
			bIsActive = false;
			return;
		}

		RocketLifeTime -= DeltaTime;
		FVector RocketLocation = GetActorLocation();
		FVector TargetLocation = PlayerCodyRef->GetActorLocation();

		FVector Dir = TargetLocation - RocketLocation;
		Dir.Normalize();

		SetActorRotation(Dir.Rotation());

		FVector NewRocketLocation = RocketLocation + Dir * RocketMoveSpeed * DeltaTime;
		SetActorLocation(NewRocketLocation);
	}
}

void AHomingRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FVector SettingLocation = GetActorLocation();
	AExplosionEffect* Effect = GetWorld()->SpawnActor<AExplosionEffect>(ExplosionEffectClass, SettingLocation, FRotator::ZeroRotator);
	if (Effect != nullptr)
	{
		AEnemyFlyingSaucer* ParentActor = Cast<AEnemyFlyingSaucer>(GetOwner());
		if (nullptr != ParentActor)
		{
			AActor* FloorActor = Cast<AActor>(ParentActor->GetFloor());
			Effect->AttachToActor(FloorActor, FAttachmentTransformRules::KeepWorldTransform);
		}
	}

	Destroy();
}

