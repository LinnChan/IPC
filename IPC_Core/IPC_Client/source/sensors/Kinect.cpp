#include "sensors/Kinect.h"

ipc::CKinect::~CKinect()
{
	SafeRelease(m_KinectSensor);
	SafeRelease(m_ColorFrameSrc);
	SafeRelease(m_DepthFrameSrc);
	SafeRelease(m_CoordMapper);
}

ipc::ESensorResult ipc::CKinect::Open()
{
	if (m_KinectSensor != nullptr)
		return ipc::ESensorResult::FAIL;

	HRESULT hr = GetDefaultKinectSensor(&m_KinectSensor);
	if (!SUCCEEDED(hr)) return ipc::ESensorResult::FAIL;

	hr = m_KinectSensor->Open();
	if (!SUCCEEDED(hr)) return ipc::ESensorResult::FAIL;

	hr = m_KinectSensor->get_ColorFrameSource(&m_ColorFrameSrc);
	if (!SUCCEEDED(hr)) return ipc::ESensorResult::FAIL;

	hr = m_KinectSensor->get_DepthFrameSource(&m_DepthFrameSrc);
	if (!SUCCEEDED(hr)) return ipc::ESensorResult::FAIL;

	hr = m_KinectSensor->get_BodyIndexFrameSource(&m_BodyIndexFrameSrc);
	if (!SUCCEEDED(hr)) return ipc::ESensorResult::FAIL;

	hr = m_KinectSensor->get_CoordinateMapper(&m_CoordMapper);
	if (!SUCCEEDED(hr)) return ipc::ESensorResult::FAIL;

	IFrameDescription* colorFrameDesc = nullptr;
	m_ColorFrameSrc->get_FrameDescription(&colorFrameDesc);
	colorFrameDesc->get_Height(&m_ColorFrameHeight);
	colorFrameDesc->get_Width(&m_ColorFrameWidth);
	colorFrameDesc->get_BytesPerPixel(&m_DepthFrameBytesPerPixel);
	SafeRelease(colorFrameDesc);
	
	IFrameDescription* depthFrameDesc = nullptr;
	m_DepthFrameSrc->get_FrameDescription(&depthFrameDesc);
	depthFrameDesc->get_Height(&m_DepthFrameHeight);
	depthFrameDesc->get_Width(&m_DepthFrameWidth);
	depthFrameDesc->get_BytesPerPixel(&m_DepthFrameBytesPerPixel);
	SafeRelease(depthFrameDesc);

	m_ColorFrameSrc->OpenReader(&m_ColorFrameReader);
	m_DepthFrameSrc->OpenReader(&m_DepthFrameReader);

	return ipc::ESensorResult::SUCCESS;
}

ipc::ESensorResult ipc::CKinect::Close()
{
	HRESULT hr = m_KinectSensor->Close();
	if (!SUCCEEDED(hr)) return ipc::ESensorResult::FAIL;

	return ipc::ESensorResult::SUCCESS;
}


ipc::ESensorResult ipc::CKinect::GetPointCloudData(FPointCloud** ppData)
{
	return ipc::ESensorResult::SUCCESS;
}

void ipc::CKinect::Run()
{
	
}