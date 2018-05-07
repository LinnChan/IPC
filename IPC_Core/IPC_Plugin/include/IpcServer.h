#pragma once

#include "Common.h"
#include "rpc/server.h"

#include <map>

namespace ipc
{
	class CIpcServer :private rpc::server
	{
	public:
		static void StartServer(std::string ip, uint16_t& port);
		static void StopServer();
		static CIpcServer* GetInstance();

	private:
		CIpcServer(std::string ip, uint16_t& port);
		CIpcServer* Instance = nullptr;

		std::map<int, FPointCloud*> m_BufferMap;
	};
}