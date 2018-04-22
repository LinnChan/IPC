#include "IPC_Client.h"
#include "processor/PointCloudProcessor.h"

#include <conio.h>
#include <windows.h>

ipc::CIPC_Client::CIPC_Client(ISensor* pSensor, const std::string& ip_addr, const uint16_t port, const uint32_t& deltaTime)
{
	m_DeltaTime_ms = deltaTime;

	m_pSensor.reset(pSensor);

	if (ip_addr.size() > 0)
		m_pSender.reset(new CPointCloudSender(ip_addr, port));
	else
		m_pSender.reset(new CPointCloudSender());

	m_pSensor->Open();
}

ipc::CIPC_Client::~CIPC_Client()
{
	m_pSensor.release();
	m_pSender.release();
}

void ipc::CIPC_Client::Run()
{
	int key = 0;
	while (key != 'q')
	{
		key = _getch();

		if (!m_pSensor->IsOpen())
		{
			m_pSensor->Open();
			continue;
		}
		
		ipc::FPointCloud_Raw* pPointCloudRaw = nullptr;
		auto res = m_pSensor->GetPointCloudData(&pPointCloudRaw);
		if (res == ESensorResult::SUCCESS)
		{
			FPointCloud_Send pointCloudSend(*pPointCloudRaw);
			m_pSender->SendPointCloudData(pointCloudSend);
		}

		Sleep(static_cast<DWORD>(m_DeltaTime_ms));
	}
}