#include "IpcClient.h"
#include "processor/PointCloudProcessor.h"

#include <conio.h>
#include <windows.h>
#include <iostream>

ipc::CIpcClient::CIpcClient(ISensor* pSensor, const std::string& ip_addr, const uint16_t port, const uint32_t& deltaTime)
{
	m_DeltaTime_ms = deltaTime;

	m_pSensor.reset(pSensor);

	if (ip_addr.size() > 0)
		m_pSender.reset(new CPointCloudSender(ip_addr, port));
	else
		m_pSender.reset(new CPointCloudSender());

	if (!m_pSender->Connect())
	{
		m_IsConnected = false;
		std::cout << "Connect failed!" << std::endl;
		return;
	}

	m_IsConnected = true;
	m_pSensor->Open();
}

ipc::CIpcClient::~CIpcClient()
{
	m_pSensor.release();
	m_pSender.release();
}

void ipc::CIpcClient::Run()
{
	if (!m_IsConnected)
		return;

	int key = 0;
	while (key != 'q')
	{
		key = _getch();

		if (!m_pSensor->IsOpen())
		{
			m_pSensor->Open();
			Sleep(static_cast<DWORD>(m_DeltaTime_ms));
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