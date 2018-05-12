#include "IpcServer.h"
#include "rpcAdaptor.hpp"

std::unique_ptr<ipc::CIpcServer> ipc::CIpcServer::Instance;

ipc::CIpcServer::CIpcServer(IViewer* viewer, std::string& ip, uint16_t& port) :server(ip, port)
{
	m_Viewer = viewer;

	bind("TransmitPointCloud", [&](const adaptor::FPointCloud_Send& pts)
	{
		if (m_Viewer == nullptr)return;

		ipc::FPointCloud* pData = pts.to();
		pData->buffer_id = m_BufferCounter;
		m_BufferMap[m_BufferCounter] = pData;
		
		m_Viewer->ShowPointCloud(pData);

		++m_BufferCounter;
	});
}

ipc::CIpcServer::~CIpcServer()
{
	for (auto& item : m_BufferMap)
	{
		delete[] item.second->data;
		item.second->data = nullptr;
		m_BufferMap.erase(item.first);
		delete item.second;
	}
}

void ipc::CIpcServer::Run()
{
	async_run(4);
}

void ipc::CIpcServer::StartServer(IViewer* viewer, std::string& ip, uint16_t& port)
{
	Instance.reset(new CIpcServer(viewer, ip, port));
}

void ipc::CIpcServer::StopServer()
{
	Instance->stop();
	Instance.reset(nullptr);
}

void ipc::CIpcServer::ReleaseBuffer(const uint32_t& bufferId)
{
	FPointCloud* pData = Instance->m_BufferMap[bufferId];
	delete[] pData->data;
	pData->data = nullptr;
	Instance->m_BufferMap.erase(bufferId);
	delete pData;
}

void ipc::StartServer(IViewer* viewer, std::string ip, uint16_t port)
{
	CIpcServer::StartServer(viewer, ip, port);
}

void ipc::StopServer()
{
	CIpcServer::StopServer();
}

void ipc::ReleaseBuffer(const uint32_t id)
{
	CIpcServer::ReleaseBuffer(id);
}
