// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pillar.generated.h"

UCLASS()
class UE5_ITT_API APillar : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APillar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	enum class EnumState
	{
		Close,
		WaitMove,
		Wait,
		MoveUp,
		WaitBoom,
		MoveDown,
		Boom,
		Done,
		None,
	};

	void ChangeState(EnumState NextState)
	{
		CurState = NextState;
	}



private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* GlassMesh = nullptr;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* PillarMesh = nullptr;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* CoreMesh = nullptr;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ShieldMesh = nullptr;

	//UPROPERTY(EditAnywhere)
	//class USphereComponent* TriggerSphere = nullptr;

	bool bShutterOpen = false;
	bool bPlayerWait = false;
	bool bOnPlayer = false;
	bool bExplode = false;

	FVector DefaultPos = FVector::Zero();
	float PlayerWaitRatio = 0.f;
	float PlayerWaitSize = 60.f;

	float MoveRatio = 0.f;
	float MoveSize = 750.f;

	FVector GlassPos = FVector::Zero();

	EnumState CurState = EnumState::Close;
	void StateExcute(float DT);
};
