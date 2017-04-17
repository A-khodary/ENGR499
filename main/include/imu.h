#ifndef IMU
#define IMU

#include <mutex>
struct imuInfo{
 	float x;
	float y;
	float z;
	float roll;
	float pitch;
	float yaw;
};

void IMUExecution(struct imuInfo &data, std::mutex &dataLock);

#endif
