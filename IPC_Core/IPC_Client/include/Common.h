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

	struct FPointCloudRaw
	{
		FPointXYZRGB* data = nullptr;
		bool* valid = nullptr;
		uint32_t length = 0;

		FPointCloudRaw()
		{
			data = nullptr;
			valid = nullptr;
			length = 0;
		}

		FPointCloudRaw(const uint32_t& length_val)
		{
			data = new FPointXYZRGB[length_val];
			valid = new bool[length_val];
			length = length_val;
		}

		~FPointCloudRaw()
		{
			if(data!=nullptr)
				delete[] data;
			if (valid != nullptr)
				delete[] valid;
		}
	};

	struct FPointCloud
	{
		std::vector<std::vector<FPointXYZRGB>> data;
	};

	class ISensor
	{
	public:
		virtual ESensorResult Open() = 0;
		virtual ESensorResult Close() = 0;
		virtual ESensorResult GetPointCloudData(FPointCloudRaw** ppData) = 0;
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