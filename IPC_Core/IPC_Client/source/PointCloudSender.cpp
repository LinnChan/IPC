#include "PointCloudSender.h"

ipc::CPointCloudSender::CPointCloudSender(const std::string& ip_address, const uint16_t& port):rpc::client(ip_address,port)
{
	//m_pClient = new rpc::client(ip_address, port);
}

ipc::CPointCloudSender::~CPointCloudSender()
{
	wait_all_responses();
	//delete m_pClient;
}

void ipc::CPointCloudSender::SendPointCloudData(const FPointCloud_Send& pts)
{
	if (pts.data.size() > 0)
	{
		call("SendPointCloudData", pts);
	}
}