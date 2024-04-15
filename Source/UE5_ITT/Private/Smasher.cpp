// Fill out your copyright notice in the Description page of Project Settings.


#include "Smasher.h"

// Sets default values
ASmasher::ASmasher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASmasher::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASmasher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

