#pragma once

#include "Common.hpp"
#include "rpc/client.h"

#include <memory>

namespace ipc
{
	class CPointCloudSender:public rpc::client
	{
	public:
		CPointCloudSender(const std::string& ip_address = "127.0.0.1", const uint16_t& port = 19384);

		virtual ~CPointCloudSender();

		bool Connect();

		void SendPointCloudData(const FPointCloud_Send& pts);

	};
}