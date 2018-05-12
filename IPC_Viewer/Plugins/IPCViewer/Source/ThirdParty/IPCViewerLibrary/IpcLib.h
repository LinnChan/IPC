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
		uint32_t length;
		uint32_t buffer_id;
	};

	class IPCLIB_API IViewer
	{
	public:
		virtual void ShowPointCloud(FPointCloud* pt) = 0;
	};

	void IPCLIB_API StartServer(IViewer* viewer, std::string ip, uint16_t port);
	void IPCLIB_API StopServer();
	void IPCLIB_API ReleaseBuffer(const uint32_t id);
}
