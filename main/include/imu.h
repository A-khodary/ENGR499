#ifndef IMU
#define IMU

#include <mutex>
#include <condition_variable>
struct imuInfo{
 	float x;
	float y;
	float velx;
	float vely;
	float roll;
	float pitch;
	float yaw;
};

void IMUExecution(struct imuInfo &data, std::mutex &imuDataMutex, std::condition_variable &writecv);

#endif
