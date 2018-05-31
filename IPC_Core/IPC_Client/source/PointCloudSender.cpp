#include "PointCloudSender.h"

#include<iostream>

ipc::CPointCloudSender::CPointCloudSender(const std::string& ip_address, const uint16_t& port):rpc::client(ip_address,port)
{

}

ipc::CPointCloudSender::~CPointCloudSender()
{
	wait_all_responses();
}

void ipc::CPointCloudSender::SendPointCloudData(const FPointCloud_Send& pts)
{
	if (pts.data.size() > 0)
	{
		call("TransmitData", pts);
	}
}

bool ipc::CPointCloudSender::Connect()
{
	int count = 0;

	std::cout << "Connecting";
	while (get_connection_state() != rpc::client::connection_state::connected && count < 50)
	{
		std::cout << ".";
		++count;
		Sleep(50);
	}
	std::cout << std::endl;

	if (count >= 50)
		return false;
	return true;
}