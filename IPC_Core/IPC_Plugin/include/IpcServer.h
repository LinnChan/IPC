#pragma once

#include "IpcLib.h"
#include "rpc/server.h"
#include <memory>
#include <map>

namespace ipc
{
	class CIpcServer :private rpc::server
	{
	public:
		static void StartServer(IViewer* viewer, std::string ip, uint16_t& port);
		static void StopServer();
		static void ReleaseBuffer(const uint32_t& bufferId);

	private:
		static std::unique_ptr<CIpcServer> Instance;

	public:
		virtual ~CIpcServer();

	private:
		CIpcServer() = default;
		CIpcServer(IViewer* viewer, std::string ip, uint16_t& port);

		void Run();

		IViewer* m_Viewer = nullptr;
		std::map<uint32_t, FPointCloud*> m_BufferMap;
		uint32_t m_BufferCounter = 0;
	};
}