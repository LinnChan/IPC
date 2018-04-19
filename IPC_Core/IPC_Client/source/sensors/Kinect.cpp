#include "sensors/Kinect.h"

#include <iostream>

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
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	hr = m_KinectSensor->Open();
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	hr = m_KinectSensor->get_ColorFrameSource(&m_ColorFrameSrc);
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	hr = m_KinectSensor->get_DepthFrameSource(&m_DepthFrameSrc);
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	hr = m_KinectSensor->get_BodyIndexFrameSource(&m_BodyIndexFrameSrc);
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	hr = m_KinectSensor->get_CoordinateMapper(&m_CoordMapper);
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	IFrameDescription* colorFrameDesc = nullptr;
	m_ColorFrameSrc->get_FrameDescription(&colorFrameDesc);
	colorFrameDesc->get_Height(&mColorFrameHeight);
	colorFrameDesc->get_Width(&mColorFrameWidth);
	colorFrameDesc->get_BytesPerPixel(&mColorFrameBytesPerPixel);
	SafeRelease(colorFrameDesc);
	
	IFrameDescription* depthFrameDesc = nullptr;
	m_DepthFrameSrc->get_FrameDescription(&depthFrameDesc);
	depthFrameDesc->get_Height(&mDepthFrameHeight);
	depthFrameDesc->get_Width(&mDepthFrameWidth);
	depthFrameDesc->get_BytesPerPixel(&mDepthFrameBytesPerPixel);
	SafeRelease(depthFrameDesc);

	hr = m_ColorFrameSrc->OpenReader(&m_ColorFrameReader);
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	hr = m_DepthFrameSrc->OpenReader(&m_DepthFrameReader);
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	hr = m_BodyIndexFrameSrc->OpenReader(&m_BodyIndexFrameReader);
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	std::cout << "Device launch successfully! " << std::endl;

	return ESensorResult::SUCCESS;
}

ipc::ESensorResult ipc::CKinect::Close()
{

	HRESULT hr = m_KinectSensor->Close();
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	return ESensorResult::SUCCESS;
}


ipc::ESensorResult ipc::CKinect::GetPointCloudData(FPointCloud* ppData)
{
	IDepthFrame* pDepthFrame = nullptr;
	HRESULT hr = m_DepthFrameReader->AcquireLatestFrame(&pDepthFrame);
	if (!SUCCEEDED(hr))
	{
		SafeRelease(pDepthFrame);
		return ESensorResult::FAIL;
	}

	IColorFrame* pColorFrame = nullptr;
	hr = m_ColorFrameReader->AcquireLatestFrame(&pColorFrame);
	if (!SUCCEEDED(hr))
	{
		SafeRelease(pDepthFrame);
		SafeRelease(pColorFrame);
		return ESensorResult::FAIL;
	}

	IBodyIndexFrame* pBodyIndexFrame = nullptr;
	hr = m_BodyIndexFrameReader->AcquireLatestFrame(&pBodyIndexFrame);
	if (!SUCCEEDED(hr))
	{
		SafeRelease(pBodyIndexFrame);
		SafeRelease(pDepthFrame);
		SafeRelease(pColorFrame);
		return ESensorResult::FAIL;
	}

	uint32_t depthDataLength = 0;
	uint16_t* pDepthData = nullptr;
	pDepthFrame->AccessUnderlyingBuffer(&depthDataLength, &pDepthData);
	uint32_t colorFrameLength = 0;
	uint8_t* pColorData = nullptr;
	pColorFrame->AccessRawUnderlyingBuffer(&colorFrameLength, &pColorData);
	uint32_t bodyIndexDataLength = 0;
	uint8_t* pBodyIndexData = nullptr;
	pBodyIndexFrame->AccessUnderlyingBuffer(&bodyIndexDataLength, &pBodyIndexData);

	Concurrency::parallel_for(0,16,1,[&](const uint32_t& index)
	{
		if (pBodyIndexData[index] > 0)
		{
			uint32_t x = index / mDepthFrameWidth;
			uint32_t y = index % mDepthFrameWidth;

			uint16_t depth = pDepthData[index];
			DepthSpacePoint depthPoint = { x,y };
			ColorSpacePoint colorPoint;
			CameraSpacePoint cameraPoint;
			m_CoordMapper->MapDepthPointToColorSpace(depthPoint, depth, &colorPoint);
			m_CoordMapper->MapDepthPointToCameraSpace(depthPoint, depth, &cameraPoint);
		}
	});

	SafeRelease(pDepthFrame);
	SafeRelease(pColorFrame);


	return ESensorResult::SUCCESS;
}

void ipc::CKinect::Run()
{

}