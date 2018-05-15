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

	Texture_Position0 = UTexture2D::CreateTransient(1024, 1024);
	Texture_Position0->UpdateResource();
	Texture_Color = UTexture2D::CreateTransient(1024, 1024);
	Texture_Color->UpdateResource();

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
	uint8_t* pos_data = new uint8_t[1024 * 1024 * 3];
	uint8_t* col_data = new uint8_t[1024 * 1024 * 3];
	for (uint32_t index = 0; index < 1024 * 1024; ++index)
	{
		pos_data[index] = static_cast<uint8_t>(frame->data->x*1000.0f);
		pos_data[index] = static_cast<uint8_t>(frame->data->y*1000.0f);
		pos_data[index] = static_cast<uint8_t>(frame->data->z*1000.0f);
		col_data[index] = static_cast<uint8_t>(frame->data->r);
		col_data[index] = static_cast<uint8_t>(frame->data->g);
		col_data[index] = static_cast<uint8_t>(frame->data->b);
	}

	FUpdateTextureRegion2D region(0, 0, 0, 0, 1024, 1024);

	Texture_Position0->UpdateTextureRegions(0, 1, &region, 30, 3, pos_data);

}