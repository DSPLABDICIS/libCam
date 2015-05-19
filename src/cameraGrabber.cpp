/*
 * cameraGrabber.cpp
 *
 *  Created on: Mar 24, 2014
 *      Author: dsp
 */

#include "../include/cameraGrabber.h"

namespace cameras {

CameraGrabber::CameraGrabber() :f3D(false),fStereo(false),
		camerasContext(NULL),numCameras(0),cameras(NULL),cameraNumber(0),
		video_mode(DC1394_VIDEO_MODE_800x600_MONO8), mono(false){

}

int CameraGrabber::initializeCameras() {
	dc1394error_t err;
	dc1394framerate_t framerate;
	dc1394operation_mode_t op_mode;
	dc1394speed_t speed;

	cameras = new dc1394camera_t* [MAXCAMERAS];

	/******************Find Cameras*****************************/
	camerasContext = dc1394_new ();
	find_cameras(camerasContext, &numCameras, cameras);

	/*-----------------------------------------------------------------------
	 *  Selector menu, mode and frame
	 *-----------------------------------------------------------------------*/

	//Stingray for the moment todo: add the other cameras
	op_mode = DC1394_OPERATION_MODE_1394B;
	speed = DC1394_ISO_SPEED_800;
	video_mode = DC1394_VIDEO_MODE_640x480_MONO8;
	framerate = DC1394_FRAMERATE_15;
	cameraNumber = 2;
	mono = true;

	/*-----------------------------------------------------------------------
	 *  Modes
	 *-----------------------------------------------------------------------*/
	err = modes(&op_mode, &speed, &video_mode, &framerate, camerasContext, &numCameras, cameras);
	DC1394_ERR_CLN_RTN(err,cleanup_and_exit(&numCameras, cameras),"Failed in modes function");
	return 0;
}

int CameraGrabber::startGrabb(bool save, const char* name) {
	dc1394video_frame_t *frame[cameraNumber];
	dc1394error_t err0, err1;
	uint i, width, height;
	uint ind=0;
	cv::Mat image;
	cv::Mat *gray_image;
	boost::posix_time::ptime startTime;
	boost::posix_time::time_duration dur;
	char buffer[100];

	const char* sside[6] = {"Left","Right"};

	dc1394_get_image_size_from_video_mode(cameras[0],video_mode, &width, &height);


	gray_image = &image;

	if (mono == true)
		*gray_image = cvCreateMat(height, width, CV_8UC1);
	else
		*gray_image = cvCreateMat(height, width, CV_8UC3);

	std::cout<<gray_image->rows<<" "<<gray_image->cols<<std::endl;

	for (i = 0; i < cameraNumber; i++) {
		err0=dc1394_video_set_transmission(cameras[i], DC1394_ON);
		DC1394_ERR_CLN_RTN(err0, ,"Could not start camera iso transmission");
	}

	cv::namedWindow("Image_r",CV_WINDOW_AUTOSIZE);
	cv::namedWindow("Image_l",CV_WINDOW_AUTOSIZE);

	char exit_key_press = 0;
	while (exit_key_press != 'q') // or key != ESC
	{
		startTime =  boost::posix_time::microsec_clock::local_time();
		err0 = dc1394_capture_dequeue(cameras[0], DC1394_CAPTURE_POLICY_WAIT, &frame[1]);
		err1 = dc1394_capture_dequeue(cameras[1], DC1394_CAPTURE_POLICY_WAIT, &frame[0]);
		DC1394_ERR_CLN_RTN(err0, ,"Could not capture a frame");
		DC1394_ERR_CLN_RTN(err1, ,"Could not capture a frame");
		for (i = 0; i < cameraNumber; i++) {
			gray_image->data = frame[i]->image;

			if (i == 0){
				imshow("Image_l", *gray_image);}
			else{
				imshow("Image_r", *gray_image);}

			if(save){
				sprintf(buffer,"%s%s%i.png",name,sside[i],ind);
				cv::imwrite(buffer,*gray_image);
			}

			dc1394_capture_enqueue (cameras[1-i], frame[i]);

			if(save == false && exit_key_press == 's'){
				sprintf(buffer,"%s%s%i.png",name,sside[i],ind);
				cv::imwrite(buffer,*gray_image);
			}
		}
		if(save == true)
			ind++;
		else if(exit_key_press == 's')
			ind++;


		exit_key_press = cv::waitKey(1);
		dur = boost::posix_time::microsec_clock::local_time() - startTime;
		double milliseconds = dur.total_milliseconds();
		std::cout<<std::endl<<"\r"<<milliseconds/1000<<" seconds";
	}
	return 0;
}

} /* namespace cameras */
