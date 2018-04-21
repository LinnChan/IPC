#include "Common.h"
#include "sensors/Kinect.h"

#include <conio.h>
#include <iostream>
#include <windows.h> 

int main()
{
	ipc::CKinect sensor;

	ipc::ESensorResult res = sensor.Open();
	if (res != ipc::ESensorResult::SUCCESS) return 0;

	int key = 0;
	while (key != 'q')
	{
		ipc::FPointCloudRaw* pPointCloudRaw = nullptr;
		sensor.GetPointCloudData(&pPointCloudRaw);


		key = _getch();
	}

	sensor.Close();
	return 0;
}