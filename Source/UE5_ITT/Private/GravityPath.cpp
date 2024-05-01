// Fill out your copyright notice in the Description page of Project Settings.


#include "GravityPath.h"
#include "Components/BoxComponent.h"
#include "May.h"

// Sets default values
AGravityPath::AGravityPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		bReplicates = true;
		SetReplicateMovement(true);
		BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
		MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
		RootComponent = MeshComp;
		BoxComp->SetupAttachment(MeshComp);

		PivotComp = CreateDefaultSubobject<USceneComponent>(TEXT("PivotComp"));;
		PivotComp->SetupAttachment(MeshComp);
		PivotComp->SetRelativeLocation(FVector(400.f, 0.f, 400.f));
	}
}

// Called when the game starts or when spawned
void AGravityPath::BeginPlay()
{
	Super::BeginPlay();

	if (true == HasAuthority())
	{
		BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AGravityPath::OnOverlapBegin);
		BoxComp->OnComponentEndOverlap.AddDynamic(this, &AGravityPath::OnOverlapEnd);
		
	}
}

void AGravityPath::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	PlayerMay = Cast<AMay>(OtherActor);
}

void AGravityPath::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (PlayerMay!= nullptr)
	{
		PlayerMay->SetOnGravityPath(true);
	}
}

// Called every frame
void AGravityPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (true == HasAuthority())
	{
		if (PlayerMay!=nullptr&&BoxComp->IsOverlappingActor(PlayerMay))
		{
			//FVector Direction = MeshComp->getlocation//PlayerMay->GetActorLocation()
			//PlayerMay->GetCharacterMovement()->SetGravityDirection()

			FVector StartPos = PlayerMay->GetActorLocation();
			FVector EndPos = StartPos *-PlayerMay->GetActorUpVector();

			UE_LOG(LogTemp, Display, TEXT("StartPos %s"), *StartPos.ToString());
			UE_LOG(LogTemp, Display, TEXT("EndPos %s"), *EndPos.ToString());
			UE_LOG(LogTemp, Display, TEXT("Foward %s"), *PlayerMay->GetActorForwardVector().ToString());

			bool IsHit = false;
			FCollisionQueryParams ColQueryParam;
			ColQueryParam.AddIgnoredActor(PlayerMay);
			//EndPos.Normalize();
			IsHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartPos, EndPos *1000.f,ECollisionChannel::ECC_Visibility, ColQueryParam);

			DrawDebugLine(GetWorld(), StartPos, EndPos * 1000.f,FColor::Red,false,1.f,0,1.f);
			if (IsHit&& HitResult.GetActor() == this)
			{
				PlayerMay->SetOnGravityPath(true);
				UE_LOG(LogTemp, Display, TEXT("ImpactNormal %s"), *HitResult.ImpactNormal.ToString());
				PlayerMay->GetCharacterMovement()->SetGravityDirection(-HitResult.ImpactNormal);
				FRotator SettingRot = (HitResult.ImpactNormal).Rotation();
				SettingRot.Pitch -= 90.f;
				PlayerMay->SetGravityRotator(SettingRot);
			}
		}
	}
}

