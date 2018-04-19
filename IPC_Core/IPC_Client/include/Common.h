#pragma once

#include <stdint.h>
#include <vector>

namespace ipc
{
	enum class ESensorResult :uint8_t
	{
		FAIL = 0,
		SUCCESS = 1
	};

	struct FVector3f
	{
		float x_val;
		float y_val;
		float z_val;
	};

	struct FVector2i
	{
		int x_val;
		int y_val;
	};

	struct FColorRGB
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};

	struct FPointXYZRGB
	{
		FVector3f pos;
		FColorRGB color;

		uint32_t line_idx;
		uint32_t seg_idx;

	};

	struct FPointCloud
	{
		std::vector<std::vector<FVector3f>> data;
	};

	class ISensor
	{
	public:
		virtual ESensorResult Open() = 0;
		virtual ESensorResult Close() = 0;
		virtual ESensorResult GetPointCloudData(FPointCloud* ppData) = 0;
	};
}

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