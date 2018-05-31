// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IpcLib.h"
#include "memory"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Engine/Texture2D.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Actor.h"
#include "IpcViewerActor.generated.h"

class AIpcViewerActor;

class ViewerWarpper :public ipc::IViewer
{
public:
	virtual void ShowPointCloud(ipc::FPointCloud* pt);
	ViewerWarpper(AIpcViewerActor* viewer);

private:
	AIpcViewerActor * _Viewer = nullptr;
};

UCLASS()
class IPCVIEWER_API AIpcViewerActor : public AActor
{
	GENERATED_BODY()
public:	
	void AddFrame(ipc::FPointCloud* frame);
	ipc::FPointCloud* GetLatestFrame();

	AIpcViewerActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	void SetMaterialTexture(ipc::FPointCloud* frame);
	
public:
	UPROPERTY(EditAnywhere)
		UMaterial* MaterialReference;
	UPROPERTY(EditAnywhere)
		FName MaterialParameterName_Position0;
	UPROPERTY(EditAnywhere)
		FName MaterialParameterName_Color;
	UPROPERTY(EditAnywhere)
		UTexture2D*  Texture_Position0 = nullptr;
	UPROPERTY(EditAnywhere)
		UTexture2D*  Texture_Color = nullptr;
	UPROPERTY(EditAnywhere)
		float PlayFrameRate = 20.0f;

private:
	UParticleSystemComponent* ParticleSystemComponent = nullptr;
	UMaterialInstanceDynamic* MatInsDynamic = nullptr;

	TArray<ipc::FPointCloud*> FrameQueue;
	FCriticalSection FrameQueueMutex;
	ViewerWarpper * _ViewerWapper = nullptr;
	
};
