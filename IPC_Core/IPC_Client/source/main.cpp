#include "sensors/Kinect.h"

#include <iostream>

int main()
{
	ipc::CKinect sensor;

	ipc::ESensorResult res = sensor.Open();
	if (res == ipc::ESensorResult::SUCCESS)
		std::cout << "Device launch successfully! " << std::endl;

	Sleep(5000);

	sensor.Close();

	return 0;
}