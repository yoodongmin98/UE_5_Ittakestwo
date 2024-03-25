// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HomingRocket.generated.h"

UCLASS()
class UE5_ITT_API AHomingRocket : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHomingRocket();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	class USceneComponent* SceneComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	class UStaticMeshComponent* RocketMeshComp = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RocketLifeTime = 7.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RocketMoveSpeed = 600.0f;

	UPROPERTY(VisibleAnywhere)
	class ACody* PlayerCodyRef = nullptr;

};
