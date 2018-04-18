#pragma once

#include "Common.h"
#include <Kinect.h>

namespace ipc
{
	class CKinect :public ISensor
	{
	public:
		virtual ESensorResult Open();
		virtual ESensorResult Close();
		virtual ESensorResult GetPointCloudData(FPointCloud** ppData);

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

		int m_ColorFrameWidth = 0;
		int m_ColorFrameHeight = 0;
		unsigned int m_ColorFrameBytesPerPixel = 0;

		int m_DepthFrameWidth = 0;
		int m_DepthFrameHeight = 0;
		unsigned int m_DepthFrameBytesPerPixel = 0;
	};
}
