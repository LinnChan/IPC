#include "Common.hpp"
#include "sensors/Kinect.h"
#include "IpcClient.h"

int main()
{
	ipc::CKinect sensor;
	ipc::CIpcClient client(&sensor, "127.0.0.1", 19385, 10);

	client.Run();

	return 0;
}