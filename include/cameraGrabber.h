/*
 * cameraGrabber.h
 *
 *  Created on: Mar 24, 2014
 *      Author: José Juan Hernández López
 */

#ifndef CAMERAGRABBER_H_
#define CAMERAGRABBER_H_

#include "cameraslib.hpp"
#include "boost/thread.hpp"
#include "boost/timer.hpp"
#define MAXCAMERAS 3

namespace cameras {

class CameraGrabber {
public:
	CameraGrabber();
	virtual ~CameraGrabber() {
		if (cameras != NULL)
		{
		cleanup_and_exit(&numCameras, cameras);
		delete cameras;
		}
	}

	int initializeCameras();
	int startGrabb(bool save, const char* character);

private:

	bool f3D;
	bool fStereo;

	dc1394_t *camerasContext;
	uint32_t numCameras;
	dc1394camera_t ** cameras;
	uint cameraNumber;
	dc1394video_mode_t video_mode;
	bool mono;

};

} /* namespace cameras */
#endif /* CAMERAGRABBER_H_ */
