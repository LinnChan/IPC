// Fill out your copyright notice in the Description page of Project Settings.

#include "IpcViewerActor.h"


ViewerWarpper::ViewerWarpper(ViewerWarpper* viewer)
{
	m_Viewer = viewer;
}

void ViewerWarpper::ShowPointCloud(ipc::FPointCloud* pt)
{

}

// Sets default values
AIpcViewerActor::AIpcViewerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AIpcViewerActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIpcViewerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

