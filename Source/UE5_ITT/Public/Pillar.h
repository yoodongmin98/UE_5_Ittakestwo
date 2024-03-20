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

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* ButtonMesh = nullptr;


	UPROPERTY(EditAnywhere)
	bool bShutterOpen = false;
	bool bPlayerWait = false;
	bool bOnPlayer = false;
	bool bExplode = false;
	bool bShieldOpen = false;

	FVector DefaultPos = FVector::Zero();
	float PlayerWaitRatio = 0.f;
	float PlayerWaitSize = 80.f;
	FVector PlayerWaitPos = FVector::Zero();

	float MoveRatio = 0.f;
	float MoveSize = 750.f;
	FVector MovePos = FVector::Zero();

	float ShieldOpenRatio = 0.f;
	float ShieldOpenSize = 400.f;
	FVector ShieldDefaultPos = FVector::Zero();
	FVector ShieldOpenPos = FVector::Zero();

	UPROPERTY(VisibleAnywhere)
	int intCur = 0;

	EnumState CurState = EnumState::Close;
	void StateExcute(float DT);

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
