#pragma once

#include "IpcLib.h"
#include "rpc/msgpack.hpp"
#include <vector>

namespace ipc {
namespace adaptor {

	struct FColorRGB
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;

		MSGPACK_DEFINE_MAP(r, g, b);
	};

	struct FVector3f
	{
		float x_val;
		float y_val;
		float z_val;

		MSGPACK_DEFINE_MAP(x_val, y_val, z_val);
	};

	struct FPointXYZRGB
	{
		FVector3f position;
		FColorRGB color;

		MSGPACK_DEFINE_MAP(position, color);

		ipc::FPointXYZRGB to() const
		{
			ipc::FPointXYZRGB ret;
			ret.x = position.x_val;
			ret.y = position.y_val;
			ret.z = position.z_val;
			ret.r = color.r;
			ret.g = color.g;
			ret.b = color.b;

			return ret;
		}
	};

	struct FPointCloud_Send
	{
		std::vector<FPointXYZRGB> data;

		MSGPACK_DEFINE_MAP(data);

		ipc::FPointCloud* to()const
		{
			ipc::FPointCloud* ret=new ipc::FPointCloud;
			ret->buffer_id = 0;
			ret->length = static_cast<uint32_t>(data.size());
			ret->data = new ipc::FPointXYZRGB[ret->length];

			uint32_t index = 0;
			for (const auto& item : data)
			{
				ret->data[index++] = item.to();
			}

			return ret;
		}
	};

}
}
