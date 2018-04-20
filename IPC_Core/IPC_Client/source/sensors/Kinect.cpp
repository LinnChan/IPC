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

	// preallocate point cloud buffer
	mPointBuffer = FPointCloudRaw(mDepthFrameHeight*mDepthFrameWidth);

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
	hr = pDepthFrame->AccessUnderlyingBuffer(&depthDataLength, &pDepthData);
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	uint32_t colorFrameLength = 0;
	uint8_t* pColorData = nullptr;
	hr =  pColorFrame->AccessRawUnderlyingBuffer(&colorFrameLength, &pColorData);
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	uint32_t bodyIndexDataLength = 0;
	uint8_t* pBodyIndexData = nullptr;
	hr = pBodyIndexFrame->AccessUnderlyingBuffer(&bodyIndexDataLength, &pBodyIndexData);
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	cv::Mat depthShaded = cv::Mat(mDepthFrameHeight, mDepthFrameWidth, CV_8UC3);
	uint8_t* pDepthShadedData = depthShaded.data;

	//Concurrency::parallel_for(0, (int)depthDataLength, 1, [&](const uint32_t& index)
	for (uint32_t index = 0; index < depthDataLength; ++index)
	{
		if (pBodyIndexData[index] != 255)
		{
			uint32_t x = index / mDepthFrameWidth;
			uint32_t y = index % mDepthFrameWidth;

			uint16_t depth = pDepthData[index];
			DepthSpacePoint depthPoint = { static_cast<float>(x),static_cast<float>(y) };
			ColorSpacePoint colorPoint;
			CameraSpacePoint cameraPoint;
			m_CoordMapper->MapDepthPointToColorSpace(depthPoint, depth, &colorPoint);
			m_CoordMapper->MapDepthPointToCameraSpace(depthPoint, depth, &cameraPoint);

			if (isnan(cameraPoint.X) || isnan(cameraPoint.Y) || isnan(cameraPoint.Z) ||
				isnan(colorPoint.X) || isnan(colorPoint.Y))
			{
				mPointBuffer.data[index].pos.x_val = NAN;
				mPointBuffer.data[index].pos.y_val = NAN;
				mPointBuffer.data[index].pos.z_val = NAN;
				//return;
				continue;
			}

			int colorX = static_cast<int>(colorPoint.X + 0.5f);
			int colorY = static_cast<int>(colorPoint.Y + 0.5f);
			if (colorX<0 || colorX>mColorFrameWidth || colorY<0 || colorY>mColorFrameHeight) 
				//return;
				continue;
			
			int colorIndex = colorY*mColorFrameWidth + colorX;

			//mPointBuffer.data[index].pos.x_val = cameraPoint.Z;
			//mPointBuffer.data[index].pos.y_val = cameraPoint.X;
			//mPointBuffer.data[index].pos.z_val = cameraPoint.Y;

			//mPointBuffer.data[index].color.b = pColorData[3 * colorIndex + 0];
			//mPointBuffer.data[index].color.g = pColorData[3 * colorIndex + 1];
			//mPointBuffer.data[index].color.r = pColorData[3 * colorIndex + 2];

			pDepthShadedData[3 * index + 0] = pColorData[3 * colorIndex + 0];
			pDepthShadedData[3 * index + 1] = pColorData[3 * colorIndex + 1];
			pDepthShadedData[3 * index + 2] = pColorData[3 * colorIndex + 2];

			//pDepthShadedData[3 * index + 0] = 0;
			//pDepthShadedData[3 * index + 1] = 0;
			//pDepthShadedData[3 * index + 2] = 0;
		}
	}
	//);

	cv::imshow("color", depthShaded);
	cv::waitKey(10);

	SafeRelease(pDepthFrame);
	SafeRelease(pColorFrame);
	SafeRelease(pBodyIndexFrame);


	return ESensorResult::SUCCESS;
}

void ipc::CKinect::Run()
{

}