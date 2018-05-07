#include "IpcServer.h"
#include "rpcAdaptor.h"

ipc::CIpcServer::CIpcServer(std::string ip, uint16_t& port) :rpc::server(ip, port)
{
	bind("TransmitPointCloud", [&](const adaptor::FPointCloud_Send& pts)
	{
		//  translate the point-cloud data to unreal.

	});

	Instance = this;
}

