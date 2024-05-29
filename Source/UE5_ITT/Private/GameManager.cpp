// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "ParentCameraRail.h"
#include "Engine/ObjectLibrary.h"
#include "Sound/SoundCue.h"


void UGameManager::Init()
{
	Super::Init();
}

void UGameManager::AddCameraRigRail(const FString& KeyName , AParentCameraRail* _CameraRigRail)
{
	if (nullptr != MapCamera.Find(KeyName))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Key Already Exist"));
		return;
	}
	else
	{
		MapCamera.Add(KeyName, _CameraRigRail);
	}
}

void UGameManager::ChangeCameraView(const FString& KeyName, float BlendTime)
{
	if (nullptr == MapCamera.Find(KeyName))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Key Invalid"));
		return;
	}
	else
	{
		AParentCameraRail* CamActor = MapCamera[KeyName];
		CamActor->Multicast_SetCameraView(BlendTime);
	}
}

void UGameManager::SetCody(ACody* ActorCody)
{
	Cody = ActorCody;
}

void UGameManager::LoadSound()
{
	ObjLibrary = UObjectLibrary::CreateLibrary(nullptr, false, false);
	if (ObjLibrary != nullptr)
	{
		ObjLibrary->LoadAssetsFromPath(TEXT("/Game/Sound"));
	}

	TArray<USoundCue*> TempArray;
	ObjLibrary->GetObjects(TempArray);

	for (size_t i = 0; i < TempArray.Num(); i++)
	{
		if (SoundList.Find(TempArray[i]->GetName())!=nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Key Already Exist"));
			return;
		}

		SoundList.Add(TempArray[i]->GetName(),TempArray[i]);
	}

	UE_LOG(LogTemp, Display, TEXT("Load Sound %d"), SoundList.Num());
}

USoundCue* UGameManager::GetSound(const FString& KeyName)
{
	if (SoundList.Find(KeyName) == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid Key"));
		return nullptr;
	}

	return SoundList[KeyName];
}
