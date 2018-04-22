#pragma once

#include <stdint.h>
#include <vector>
#include "rpc/msgpack.hpp"

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

		MSGPACK_DEFINE_MAP(x_val, y_val, z_val);
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

		MSGPACK_DEFINE_MAP(r, g, b);
	};

	struct FPointXYZRGB
	{
		FVector3f position;
		FColorRGB color;

		MSGPACK_DEFINE_MAP(position, color);
	};

	struct FPointCloud_Raw
	{
		FPointXYZRGB* data = nullptr;
		bool* valid = nullptr;
		uint32_t length = 0;

		FPointCloud_Raw()
		{
			data = nullptr;
			valid = nullptr;
			length = 0;
		}

		FPointCloud_Raw(const uint32_t& length_val)
		{
			data = new FPointXYZRGB[length_val];
			valid = new bool[length_val];
			length = length_val;
		}

		~FPointCloud_Raw()
		{
			if(data!=nullptr)
				delete[] data;
			if (valid != nullptr)
				delete[] valid;
		}
	};

	struct FPointCloud_Send
	{
		std::vector<FPointXYZRGB> data;

		MSGPACK_DEFINE_MAP(data);

		FPointCloud_Send() = default;

		FPointCloud_Send(const FPointCloud_Raw& raw_data)
		{
			if (raw_data.length > 0)
			{
				for (uint32_t index = 0; index < raw_data.length; ++index)
				{
					if (raw_data.valid[index])
						data.push_back(raw_data.data[index]);
				}
			}
		}
	};

	class ISensor
	{
	public:
		virtual ESensorResult Open() = 0;
		virtual ESensorResult Close() = 0;
		virtual ESensorResult GetPointCloudData(FPointCloud_Raw** ppData) = 0;
		virtual bool IsOpen() = 0;
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