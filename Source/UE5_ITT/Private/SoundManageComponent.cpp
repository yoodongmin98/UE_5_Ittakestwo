// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManageComponent.h"
#include "Sound/SoundCue.h"
#include "GameManager.h"
#include "Kismet/GameplayStatics.h"

void USoundManageComponent::MulticastPlaySoundLocation_Implementation(const FString& KeyName, FVector Location, float Attenuation)
{
	USoundCue* TempCue = Cast<UGameManager>(GetWorld()->GetGameInstance())->GetSound(KeyName);
	if (TempCue != nullptr)
	{
		USoundAttenuation* AttenuationSet = NewObject<USoundAttenuation>();
		AttenuationSet->Attenuation.bAttenuate = true;
		AttenuationSet->Attenuation.AttenuationShape = EAttenuationShape::Sphere;
		AttenuationSet->Attenuation.FalloffDistance = Attenuation;

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), TempCue, Location,1.f,1.f,0.f, AttenuationSet);
	}

}

void USoundManageComponent::MulticastChangeSound_Implementation(const FString& KeyName, bool bPlay, float StartTime)
{	
	//매니저에게 사운드 불러오기
	USoundCue* TempCue = Cast<UGameManager>(GetWorld()->GetGameInstance())->GetSound(KeyName);

	//현재 사운드 변경
	if (TempCue !=nullptr)
	{
		SetSound(TempCue);

		if (bPlay)
		{
			Play(StartTime);
		}
	}

}

void USoundManageComponent::MulticastPlaySoundDirect_Implementation(const FString& KeyName)
{
	USoundCue* TempCue = Cast<UGameManager>(GetWorld()->GetGameInstance())->GetSound(KeyName);
	if (TempCue!=nullptr)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), TempCue);
	}
}
