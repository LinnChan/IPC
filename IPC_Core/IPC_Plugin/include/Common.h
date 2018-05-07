#pragma once

#include <stdint.h>
#include <string>

#ifdef IPCLIB_EXPORTS
#define IPCLIB_API __declspec(dllexport) 		
#else
#define IPCLIB_API __declspec(dllimport)
#endif

namespace ipc
{
	struct IPCLIB_API FPointXYZRGB
	{
		float x;
		float y;
		float z;
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};

	struct IPCLIB_API FPointCloud
	{
		FPointXYZRGB* data;
		int length;
		int buffer_id;
	};

	void ReleaseBuffer(int id);

	class IPCLIB_API IViewer
	{
		virtual void ShowPointCloud(FPointCloud* pt) = 0;
	};
}
