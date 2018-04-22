#include "Common.hpp"
#include "sensors/Kinect.h"

int main()
{
	ipc::CKinect sensor;

	ipc::ESensorResult res = sensor.Open();
	if (res != ipc::ESensorResult::SUCCESS) return 0;



	sensor.Close();
	return 0;
}