// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "ParentCameraRail.h"


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

void UGameManager::ChangeCameraView(const FString& KeyName)
{
	if (nullptr == MapCamera.Find(KeyName))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Key Invalid"));
		return;
	}
	else
	{
		AParentCameraRail* CamActor = MapCamera[KeyName];
		CamActor->Multicast_SetCameraView();
	}
}
