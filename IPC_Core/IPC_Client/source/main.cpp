#include "sensors/Kinect.h"

#include <iostream>

int main()
{
	ipc::CKinect sensor;

	ipc::ESensorResult res = sensor.Open();
	if (res != ipc::ESensorResult::SUCCESS) return;

	while (true)
	{
		sensor.GetPointCloudData(nullptr);
		Sleep(100);
	}

	sensor.Close();
	return 0;
}