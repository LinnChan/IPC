// Fill out your copyright notice in the Description page of Project Settings.

#include "IpcViewerActor.h"


ViewerWarpper::ViewerWarpper(AIpcViewerActor* viewer)
{
	_Viewer = viewer;
}

void ViewerWarpper::ShowPointCloud(ipc::FPointCloud* pt)
{
	_Viewer->AddFrame(pt);
}

AIpcViewerActor::AIpcViewerActor()
{
	PrimaryActorTick.bCanEverTick = true;

	ParticleSystemComponent = GetComponentByClass<UParticleSystemComponent>();
}

void AIpcViewerActor::BeginPlay()
{
	Super::BeginPlay();

	MatInsDynamic = ParticleSystemComponent->CreateDynamicMaterialInstance(0, MaterialReference);
	MatInsDynamic->SetTextureParameterValue(MaterialParameterName_Position0, Texture_Position0);
	MatInsDynamic->SetTextureParameterValue(MaterialParameterName_Position0, Texture_Color);

	if (HasAuthority())
	{
		_ViewerWapper = new ViewerWarpper(this);
		ipc::StartServer(_ViewerWapper, "127.0.0.1", 19385);
	}
}

void AIpcViewerActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed)	
	{
		if (HasAuthority())
		{
			ipc::StopServer();
			delete _ViewerWapper;
		}
	}
	Super::EndPlay(EndPlayReason);
}

void AIpcViewerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UMaterialInterface* material = ParticleSystemComponent->GetMaterial(0);
	ParticleSystemComponent->SetMaterialParameter()
}

void AIpcViewerActor::AddFrame(ipc::FPointCloud* frame)
{
	FrameQueueMutex.Lock();
	FrameQueue.Add(frame);
	FrameQueueMutex.Unlock();
}

void AIpcViewerActor::SetMaterialTexture(ipc::FPointCloud* frame)
{
	//Texture_Position0->UpdateTextureRegions()

	//MatInsDynamic->SetTextureParameterValue(MaterialParameterName_Position0, TexturePosition0);
	//MatInsDynamic->SetTextureParameterValue(MaterialParameterName_Position0, TexturePosition0);
}