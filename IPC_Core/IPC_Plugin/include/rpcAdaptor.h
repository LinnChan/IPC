#pragma once

#include "rpc/msgpack.hpp"

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
	};

	struct FPointCloud_Send
	{
		std::vector<FPointXYZRGB> data;

		MSGPACK_DEFINE_MAP(data);

		FPointCloud_Send() = default;
	};

}
}
