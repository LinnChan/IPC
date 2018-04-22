#pragma once

#include "Common.hpp"

#include "opencv2/opencv.hpp"

#include <Kinect.h>
#include <thread>
#include <limits>
#include <ppl.h>

namespace ipc
{
	class CKinect :public ISensor
	{
	public:
		virtual ESensorResult Open();
		virtual ESensorResult Close();
		virtual ESensorResult GetPointCloudData(FPointCloud_Raw** ppData);
		virtual bool IsOpen() { return m_IsOpen; }

		CKinect() {};
		virtual ~CKinect();
		
	private:
		CKinect(const CKinect& other) = default;

	private:
		IKinectSensor* m_KinectSensor = nullptr;
		ICoordinateMapper* m_CoordMapper = nullptr;
		IColorFrameSource* m_ColorFrameSrc = nullptr;
		IColorFrameReader* m_ColorFrameReader = nullptr;
		IDepthFrameSource* m_DepthFrameSrc = nullptr;
		IDepthFrameReader* m_DepthFrameReader = nullptr;
		IBodyIndexFrameSource* m_BodyIndexFrameSrc = nullptr;
		IBodyIndexFrameReader* m_BodyIndexFrameReader = nullptr;

		int mColorFrameWidth = 0;
		int mColorFrameHeight = 0;
		unsigned int mColorFrameBytesPerPixel = 0;
		uint8_t* pColorBuffer = nullptr;

		int mDepthFrameWidth = 0;
		int mDepthFrameHeight = 0;
		unsigned int mDepthFrameBytesPerPixel = 0;

		FPointCloud_Raw* m_pPointBuffer;

		bool m_IsOpen = false;
	};
}
