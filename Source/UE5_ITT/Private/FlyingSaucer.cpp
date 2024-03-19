// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingSaucer.h"
#include "Components/SkeletalMeshComponent.h"
#include "MoonBaboon.h"

// Sets default values
AFlyingSaucer::AFlyingSaucer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(SceneComp);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FlyingSaucerMesh"));
	SkeletalMeshComp->SetupAttachment(SceneComp);
}

// Called when the game starts or when spawned
void AFlyingSaucer::BeginPlay()
{
	Super::BeginPlay();
	
	MoonBaboon = GetWorld()->SpawnActor<AMoonBaboon>(MoonBaboonClass);
	MoonBaboon->AttachToComponent(SkeletalMeshComp, FAttachmentTransformRules::KeepRelativeTransform, TEXT("ChairSocket"));
	MoonBaboon->SetOwner(this);
}

// Called every frame
void AFlyingSaucer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

