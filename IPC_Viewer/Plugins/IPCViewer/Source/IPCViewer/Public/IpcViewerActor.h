// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IpcLib.h"
#include "memory"
#include "GameFramework/Actor.h"
#include "IpcViewerActor.generated.h"

class AIpcViewerActor;

class ViewerWarpper :public ipc::IViewer
{
public:
	virtual void ShowPointCloud(ipc::FPointCloud* pt);

	ViewerWarpper(ViewerWarpper* viewer);

private:
	AIpcViewerActor* m_Viewer = nullptr;
};

UCLASS()
class IPCVIEWER_API AIpcViewerActor : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)


public:	
	// Sets default values for this actor's properties
	AIpcViewerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
