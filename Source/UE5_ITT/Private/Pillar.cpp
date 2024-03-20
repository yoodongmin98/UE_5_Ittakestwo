// Fill out your copyright notice in the Description page of Project Settings.


#include "Pillar.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APillar::APillar()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PillarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillarMesh"));
	RootComponent = PillarMesh;

	GlassMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GlassMesh"));
	
	GlassMesh->SetupAttachment(PillarMesh);

	CoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMesh"));
	CoreMesh->SetupAttachment(GlassMesh);
	
	ShieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	ShieldMesh->SetupAttachment(GlassMesh);

	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetupAttachment(PillarMesh);

}

// Called when the game starts or when spawned
void APillar::BeginPlay()
{
	Super::BeginPlay();

	DefaultPos = GetActorLocation();

	PlayerWaitPos = DefaultPos;
	PlayerWaitPos.Z += PlayerWaitSize;

	MovePos = DefaultPos;
	MovePos.Z += MoveSize;

	ShieldDefaultPos = ShieldMesh->GetRelativeLocation();
	ShieldOpenPos = ShieldDefaultPos;
	ShieldOpenPos.Z -= ShieldOpenSize;
}

// Called every frame
void APillar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	StateExcute(DeltaTime);	

}

void APillar::StateExcute(float DT)
{
	switch (CurState)
	{
	case APillar::EnumState::Close:
	{		
		if (true == bShutterOpen)
		{
			ChangeState(EnumState::WaitMove);
		}
	}
	break;
	case APillar::EnumState::WaitMove:
	{
		PlayerWaitRatio += DT;
		if (PlayerWaitRatio >= 1.f)
		{
			PlayerWaitRatio = 1.f;
			ChangeState(EnumState::Wait);
			ButtonMesh->OnComponentBeginOverlap.AddDynamic(this, &APillar::OnOverlapBegin);
			ButtonMesh->OnComponentEndOverlap.AddDynamic(this, &APillar::OnOverlapEnd);
		}

		SetActorLocation(FMath::Lerp(DefaultPos, PlayerWaitPos, PlayerWaitRatio));
	}
	break;
	case APillar::EnumState::Wait:
	{		
		if (true == bOnPlayer)
		{
			ChangeState(EnumState::MoveUp);
		}
	}
	break;
	case APillar::EnumState::MoveUp:
	{
		if (false == bOnPlayer)
		{
			bShieldOpen = true;
			ChangeState(EnumState::MoveDown);
			return;
		}

		MoveRatio += DT;
		if (MoveRatio >= 1.f)
		{
			MoveRatio = 1.f;
			ChangeState(EnumState::WaitBoom);
		}

		SetActorLocation(FMath::Lerp(PlayerWaitPos, MovePos, MoveRatio));
	}
	break;
	case APillar::EnumState::WaitBoom:
	{
		if (false == bOnPlayer)
		{
			bShieldOpen = true;
			ChangeState(EnumState::MoveDown);
			return;
		}

		if (false == bShieldOpen)
		{
			ShieldOpenRatio += DT;
			if (ShieldOpenRatio >= 1.f)
			{
				ShieldOpenRatio = 1.f;
				bShieldOpen = true;
			}
			ShieldMesh->SetRelativeLocation(FMath::Lerp(ShieldDefaultPos, ShieldOpenPos, ShieldOpenRatio));
		}

		if (true == bExplode)
		{
			//레이저 타격 체크 필요
			ChangeState(EnumState::Boom);
			return;
		}
	}
	break;
	case APillar::EnumState::MoveDown:
	{
		if (true == bOnPlayer)
		{
			ChangeState(EnumState::MoveUp);
			return;
		}

		if (true == bShieldOpen)
		{
			ShieldOpenRatio -= DT*3.f;
			if (ShieldOpenRatio <= 0.f)
			{
				ShieldOpenRatio = 0.f;
				bShieldOpen = false;
			}
			ShieldMesh->SetRelativeLocation(FMath::Lerp(ShieldDefaultPos, ShieldOpenPos, ShieldOpenRatio));
		}

		MoveRatio -= DT;
		if (MoveRatio <= 0.f)
		{
			MoveRatio = 0.f;
			ChangeState(EnumState::Wait);
		}

		SetActorLocation(FMath::Lerp(PlayerWaitPos, MovePos, MoveRatio));
	}
	break;
	case APillar::EnumState::Boom:
	{
		//레이저가 닿으면 폭발
	}
	break;
	case APillar::EnumState::Done:
	{
		//폭발 완료되면 전부 내리고 none으로 변경하기
	}
	break;
	case APillar::EnumState::None:
	{

	}
	break;
	}
}

void APillar::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		bOnPlayer = true;
	}
}

void APillar::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComp)
	{
		bOnPlayer = false;
	}
}
