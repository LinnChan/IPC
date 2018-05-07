#include "sensors/Kinect.h"

#include <iostream>

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

ipc::CKinect::~CKinect()
{
	Close();
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
	m_pPointBuffer = new FPointCloud_Raw(mDepthFrameHeight*mDepthFrameWidth);

	pColorBuffer = new uint8_t[mColorFrameWidth*mColorFrameHeight * 4];

	m_IsOpen = true;

	std::cout << "Device launch successfully! " << std::endl;

	return ESensorResult::SUCCESS;
}

ipc::ESensorResult ipc::CKinect::Close()
{
	if (m_KinectSensor != nullptr)
	{
		HRESULT hr = m_KinectSensor->Close();
		if (!SUCCEEDED(hr)) return ESensorResult::FAIL;
	}

	m_IsOpen = false;

	delete m_pPointBuffer;

	SafeRelease(m_ColorFrameSrc);
	SafeRelease(m_DepthFrameSrc);
	SafeRelease(m_BodyIndexFrameSrc);
	SafeRelease(m_ColorFrameReader);
	SafeRelease(m_DepthFrameReader);
	SafeRelease(m_CoordMapper);
	SafeRelease(m_BodyIndexFrameReader);
	SafeRelease(m_KinectSensor);

	return ESensorResult::SUCCESS;
}


ipc::ESensorResult ipc::CKinect::GetPointCloudData(FPointCloud_Raw** ppData)
{
	if (m_IsOpen)
		return ESensorResult::FAIL;

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
	hr = pColorFrame->CopyConvertedFrameDataToArray(mColorFrameHeight*mColorFrameWidth * 4, pColorBuffer, ColorImageFormat::ColorImageFormat_Bgra);
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	uint32_t bodyIndexDataLength = 0;
	uint8_t* pBodyIndexData = nullptr;
	hr = pBodyIndexFrame->AccessUnderlyingBuffer(&bodyIndexDataLength, &pBodyIndexData);
	if (!SUCCEEDED(hr)) return ESensorResult::FAIL;

	cv::Mat depthShaded = cv::Mat(mDepthFrameHeight, mDepthFrameWidth, CV_8UC3);
	uint8_t* pDepthShadedData = depthShaded.data;

	Concurrency::parallel_for(0, (int)depthDataLength, 1, [&](const uint32_t& index)
	{
		if (pBodyIndexData[index] != 0xFF)
		{
			uint32_t depthX = index % mDepthFrameWidth;
			uint32_t depthY = index / mDepthFrameWidth;

			uint16_t depth = pDepthData[index];
			DepthSpacePoint depthPoint = { static_cast<float>(depthX), static_cast<float>(depthY) };
			ColorSpacePoint colorPoint;
			CameraSpacePoint cameraPoint;
			m_CoordMapper->MapDepthPointToColorSpace(depthPoint, depth, &colorPoint);
			m_CoordMapper->MapDepthPointToCameraSpace(depthPoint, depth, &cameraPoint);

			if (colorPoint.X != -std::numeric_limits<float>::infinity() &&
				colorPoint.Y != -std::numeric_limits<float>::infinity() &&
				cameraPoint.X != -std::numeric_limits<float>::infinity() &&
				cameraPoint.Y != -std::numeric_limits<float>::infinity() &&
				cameraPoint.Z != -std::numeric_limits<float>::infinity())
			{
				int colorX = static_cast<int>(colorPoint.X + 0.5f);
				int colorY = static_cast<int>(colorPoint.Y + 0.5f);
				if (colorX >= 0 && colorX < mColorFrameWidth && 
					colorY >= 0 && colorY < mColorFrameHeight)
				{
					int colorIndex = colorY*mColorFrameWidth + colorX;

					m_pPointBuffer->valid[index] = true;

					m_pPointBuffer->data[index].position.x_val = cameraPoint.Z;
					m_pPointBuffer->data[index].position.y_val = cameraPoint.X;
					m_pPointBuffer->data[index].position.z_val = cameraPoint.Y;

					m_pPointBuffer->data[index].color.b = pColorBuffer[4 * colorIndex + 0];
					m_pPointBuffer->data[index].color.g = pColorBuffer[4 * colorIndex + 1];
					m_pPointBuffer->data[index].color.r = pColorBuffer[4 * colorIndex + 2];
				}
			}
		}
		else
		{
			m_pPointBuffer->valid[index] = false;
		}
	});

	*ppData = m_pPointBuffer;

	SafeRelease(pDepthFrame);
	SafeRelease(pColorFrame);
	SafeRelease(pBodyIndexFrame);

	return ESensorResult::SUCCESS;
}
