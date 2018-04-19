#pragma once

#include "Common.h"

#include "opencv2/opencv.hpp"

#include <Kinect.h>
#include <thread>
#include <ppl.h>

namespace ipc
{
	class CKinect :public ISensor
	{
	public:
		virtual ESensorResult Open();
		virtual ESensorResult Close();
		virtual ESensorResult GetPointCloudData(FPointCloud* ppData);

		CKinect() {};
		virtual ~CKinect();
		
	private:
		CKinect(const CKinect& other) = default;
		void Run();

	private:
		IKinectSensor* m_KinectSensor = nullptr;
		IColorFrameSource* m_ColorFrameSrc = nullptr;
		IDepthFrameSource* m_DepthFrameSrc = nullptr;
		IBodyIndexFrameSource* m_BodyIndexFrameSrc = nullptr;
		ICoordinateMapper* m_CoordMapper = nullptr;
		IColorFrameReader* m_ColorFrameReader = nullptr;
		IDepthFrameReader* m_DepthFrameReader = nullptr;
		IBodyIndexFrameReader* m_BodyIndexFrameReader = nullptr;

		int mColorFrameWidth = 0;
		int mColorFrameHeight = 0;
		unsigned int mColorFrameBytesPerPixel = 0;

		int mDepthFrameWidth = 0;
		int mDepthFrameHeight = 0;
		unsigned int mDepthFrameBytesPerPixel = 0;
	};
}
