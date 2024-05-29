// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManageComponent.h"
#include "Sound/SoundCue.h"
#include "GameManager.h"
#include "Kismet/GameplayStatics.h"

USoundCue* USoundManageComponent::GetCurSound()
{
	return CurSound;
}

void USoundManageComponent::ChangeSound(const FString& KeyName, bool bPlay, float StartTime)
{	
	//매니저에게 사운드 불러오기
	CurSound = Cast<UGameManager>(GetWorld()->GetGameInstance())->GetSound(KeyName);

	//현재 사운드 변경
	SetSound(CurSound);

	if (bPlay)
	{
		Play(StartTime);
	}
}

void USoundManageComponent::PlaySoundDirect(const FString& KeyName)
{
	UGameplayStatics::PlaySound2D(GetWorld(), Cast<UGameManager>(GetWorld()->GetGameInstance())->GetSound(KeyName));
}
