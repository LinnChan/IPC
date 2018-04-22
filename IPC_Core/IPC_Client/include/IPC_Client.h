#pragma once

#include "Common.hpp"
#include "PointCloudSender.h"

#include <memory>

namespace ipc
{
	class CIPC_Client
	{
	public:
		CIPC_Client() = default;
		CIPC_Client(ISensor* pSensor, const std::string& ip_addr, const uint16_t port, const uint32_t& deltaTime);

		~CIPC_Client();

		void Run();

	private:
		uint32_t m_DeltaTime_ms = 60;

		std::unique_ptr<ISensor> m_pSensor;
		std::unique_ptr<CPointCloudSender> m_pSender;
	};
}