// Fill out your copyright notice in the Description page of Project Settings.


#include "UIRoseArm.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AUIRoseArm::AUIRoseArm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	RootComponent = SkeletalMeshComp;

	
}

// Called when the game starts or when spawned
void AUIRoseArm::BeginPlay()
{
	Super::BeginPlay();
	

	// 네트워크 권한을 확인하는 코드
	if (true == HasAuthority())
	{
		// 서버와 클라이언트 모두에서 변경사항을 적용할 도록 하는 코드입니다.
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	//D: / project / UE5_Ittakestwo / Content / UI / StartLevel / RoseArm / RoseArm_SkeletalMesh.uasset
}

// Called every frame
void AUIRoseArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

