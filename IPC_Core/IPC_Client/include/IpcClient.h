#pragma once

#include "Common.hpp"
#include "PointCloudSender.h"

#include <memory>

namespace ipc
{
	class CIpcClient
	{
	public:
		CIpcClient() = default;
		CIpcClient(ISensor* pSensor, const std::string& ip_addr, const uint16_t port, const uint32_t& deltaTime);

		~CIpcClient();

		void Run();

	private:
		uint32_t m_DeltaTime_ms = 60;

		bool m_IsConnected = false;

		std::unique_ptr<ISensor> m_pSensor;
		std::unique_ptr<CPointCloudSender> m_pSender;
	};
}