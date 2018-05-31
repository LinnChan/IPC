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

	//ParticleSystemComponent = static_cast<UParticleSystemComponent*>(this->GetComponentByClass(UParticleSystemComponent::StaticClass()));
}

void AIpcViewerActor::BeginPlay()
{
	Super::BeginPlay();

	Texture_Position0 = UTexture2D::CreateTransient(1024, 1024);
	Texture_Position0->UpdateResource();
	Texture_Color = UTexture2D::CreateTransient(1024, 1024);
	Texture_Color->UpdateResource();

	ParticleSystemComponent =FindComponentByClass<UParticleSystemComponent>();
	MatInsDynamic = ParticleSystemComponent->CreateDynamicMaterialInstance(0, (UMaterialInterface*)(MaterialReference));
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
	
	static float frameRateAcc = 0.0f;

	frameRateAcc += DeltaTime;
	if (frameRateAcc > 1.0f / PlayFrameRate)
	{
		frameRateAcc = 0.0f;
		ipc::FPointCloud* latestFrame = GetLatestFrame();
		if (latestFrame != nullptr)
		{
			SetMaterialTexture(latestFrame);
			ipc::ReleaseBuffer(latestFrame->buffer_id);
		}
	}
}

void AIpcViewerActor::AddFrame(ipc::FPointCloud* frame)
{
	FrameQueueMutex.Lock();
	FrameQueue.Add(frame);
	FrameQueueMutex.Unlock();
}

ipc::FPointCloud* AIpcViewerActor::GetLatestFrame()
{
	if (FrameQueue.Num() > 0)
	{
		FrameQueueMutex.Lock();
		ipc::FPointCloud* result = FrameQueue[0];
		FrameQueue.RemoveAt(0);
		FrameQueueMutex.Unlock();

		return result;
	}

	return nullptr;
}

void AIpcViewerActor::SetMaterialTexture(ipc::FPointCloud* frame)
{
	uint8_t* pos_data = new uint8_t[1024 * 1024 * 3];
	uint8_t* col_data = new uint8_t[1024 * 1024 * 3];
	for (uint32_t index = 0; index < 1024 * 1024 && index<frame->length; ++index)
	{
		pos_data[index] = static_cast<uint8_t>(frame->data[index].x*100.0f);
		pos_data[index] = static_cast<uint8_t>(frame->data[index].y*100.0f);
		pos_data[index] = static_cast<uint8_t>(frame->data[index].z*100.0f);
		col_data[index] = static_cast<uint8_t>(frame->data[index].r);
		col_data[index] = static_cast<uint8_t>(frame->data[index].g);
		col_data[index] = static_cast<uint8_t>(frame->data[index].b);
	}

	FUpdateTextureRegion2D region(0, 0, 0, 0, 1024, 1024);

	Texture_Position0->UpdateTextureRegions(0, 1, &region, 30, 3, pos_data);

}