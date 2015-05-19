#include "../include/cameraGrabber.h"

int main(void)
{
	cameras::CameraGrabber grabber;

	if(grabber.initializeCameras())
		return -1;
	if(grabber.startGrabb(false,"FalseSave"))
		return -1;
	std::cout<<"Finished"<<std::endl;


}
