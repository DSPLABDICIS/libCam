#include "../include/cameraslib.hpp"

namespace cameras{
char imaPath[50];

void cleanup_and_exit(uint32_t *pNumCameras, dc1394camera_t *pCameras[]){
	uint i;
    for (i=0; i < *pNumCameras; i++)
    {
    dc1394_video_set_transmission(pCameras[i], DC1394_OFF);
    dc1394_capture_stop(pCameras[i]);
    dc1394_camera_free(pCameras[i]);
    }
}

void find_cameras(dc1394_t *pd,uint32_t *pNumCameras, dc1394camera_t *pCameras[]){

	dc1394error_t err;
	dc1394camera_list_t * list;
	uint i,j;

    if (!pd)
    	exit (1);
    err=dc1394_camera_enumerate (pd, &list);
    if (err != 0){
    	dc1394_log_error("Failed to enumerate cameras");
    	exit (1);
    }

    if (list->num == 0){
        dc1394_log_error("Failed to enumerate cameras");
        exit (1);
    }

    j = 0;
    for (i = 0; i < list->num; i++)
    {
        if (j >= maxCameras)
            break;
        std::cout<<"Guid : "<< list->ids[i].guid<< " UINT : " << list->ids[i].unit<<std::endl;
        pCameras[j] = dc1394_camera_new (pd, list->ids[i].guid);
        if (!pCameras[j]){
            dc1394_log_warning("Failed to initialize camera with guid");
            continue;
        }
        j++;
    }
    *pNumCameras = j;
    dc1394_camera_free_list (list);

    if (*pNumCameras == 0) {
        dc1394_log_error("No cameras found");
        exit (1);
    }

    printf("Cameras found = %d \n", *pNumCameras);
}

void select_menu(dc1394video_mode_t *pvideo_mode, dc1394framerate_t *pframerate,
		dc1394operation_mode_t *pop_mode, dc1394speed_t *pspeed, uint *pni, uint *mono){

	uint option;
	*mono = 1;

	printf("Select menu, camera type\n");
	printf("1--> UniBrain\n2--> Stingray\n");
	fflush(stdout);
	scanf("%d", &option);


	if (option == 2){
		*pop_mode = DC1394_OPERATION_MODE_1394B;
		*pspeed = DC1394_ISO_SPEED_800;
		printf("Select menu, video mode for Stingray Camera\n");
		printf("1--> MONO8 640x480\n2--> MONO8 800x600\n3--> MONO8 1024x768\n4--> MONO8 1280x960\n");
		fflush(stdout);
		scanf("%d", &option);
		switch (option){
			case 1:*pvideo_mode = DC1394_VIDEO_MODE_640x480_MONO8;
				break;
			case 2:*pvideo_mode = DC1394_VIDEO_MODE_800x600_MONO8;
				break;
			case 3:*pvideo_mode = DC1394_VIDEO_MODE_1024x768_MONO8;
				break;
			case 4:*pvideo_mode = DC1394_VIDEO_MODE_1280x960_MONO8;
				break;
			default:*pvideo_mode = DC1394_VIDEO_MODE_1280x960_MONO8;
				break;
		}
		printf("Select menu, frames per second\n");
		printf("1--> 3.75 frames per second\n"
				"2--> 7.5 frames per second\n3--> 15 frames per second\n");
		fflush(stdout);
		scanf("%d", &option);
		switch (option){
			case 1:*pframerate = DC1394_FRAMERATE_3_75;
				break;
			case 2:*pframerate = DC1394_FRAMERATE_7_5;
				break;
			case 3:*pframerate = DC1394_FRAMERATE_15;
				break;
			default:*pframerate = DC1394_FRAMERATE_15;
				break;
		}
		}
	else{
		*pop_mode = DC1394_OPERATION_MODE_LEGACY;
		*pspeed = DC1394_ISO_SPEED_400;
		printf("Select menu, video mode for UniBrain Camera\n");
		printf("1--> YUV411 640x480\n2--> MONO8 640x480\n");
		fflush(stdout);
		scanf("%d", &option);
		switch (option){
			case 1:*pvideo_mode = DC1394_VIDEO_MODE_640x480_YUV411;*mono = 0;
				break;
			case 2:*pvideo_mode = DC1394_VIDEO_MODE_640x480_MONO8;
				break;
			default:*pvideo_mode = DC1394_VIDEO_MODE_640x480_MONO8;
				break;
		}
		printf("Select menu, frames per second\n");
		printf("1--> 15 frames per second\n2--> 30 frames per second\n");
		fflush(stdout);
		scanf("%d", &option);
		switch (option){
			case 1:*pframerate = DC1394_FRAMERATE_15;
				break;
			case 2:*pframerate = DC1394_FRAMERATE_30;
				break;
			default:*pframerate = DC1394_FRAMERATE_30;
				break;
		}
		}
	printf("Tap the number of images\n");
	fflush(stdout);
	scanf("%d", & *pni);
	if (*pni == 0)
	{
		*pni=5;
	}

}

dc1394error_t modes(dc1394operation_mode_t *pop_mode, dc1394speed_t *pspeed,
		dc1394video_mode_t *pvideo_mode, dc1394framerate_t *pframerate,
		dc1394_t *d,uint32_t *pNumCameras, dc1394camera_t *pCameras[]){

	uint i;
	dc1394error_t err;

	for (i = 0; i < *pNumCameras; i++) {
		err=dc1394_video_set_operation_mode(pCameras[i], *pop_mode);
		DC1394_ERR_CLN_RTN(err,cleanup_and_exit(pNumCameras, pCameras),"Could not get operation mode");

		err=dc1394_video_set_iso_speed(pCameras[i], *pspeed);
		DC1394_ERR_CLN_RTN(err,cleanup_and_exit(pNumCameras, pCameras),"Could not set iso speed");

		err=dc1394_video_set_mode(pCameras[i], *pvideo_mode);
		DC1394_ERR_CLN_RTN(err,cleanup_and_exit(pNumCameras, pCameras),"Could not set video mode");

		err=dc1394_video_set_framerate(pCameras[i],*pframerate);
		DC1394_ERR_CLN_RTN(err,cleanup_and_exit(pNumCameras, pCameras),"Could not set framerate");

		err=dc1394_capture_setup(pCameras[i], 8, DC1394_CAPTURE_FLAGS_DEFAULT);
		DC1394_ERR_CLN_RTN(err,cleanup_and_exit(pNumCameras, pCameras),"Could not setup camera-make sure that the video mode and framerate are\nsupported by your camera");
	}
	return err;
}


/*-----------------------------------------------------------------------
 *  Save images
 *-----------------------------------------------------------------------*/

void save_images(uint *pi, uint *pj, uint *pwidth, uint *pheight, FILE *pimagefile,
		dc1394video_frame_t *pframe[],dc1394_t *d, uint *mono){

	if(*pi == 0){
	sprintf(imaPath, "/home/dsp/workspace_images/SerieImage_1/test_l_%04i.pgm", *pj+1);}
	else{
	sprintf(imaPath, "/home/dsp/workspace_images/SerieImage_1/test_r_%04i.pgm", *pj+1);}

	pimagefile = fopen(imaPath, "wb");

	if( pimagefile == NULL) {
		perror( "Can't create Image \n");
	}

	if (*mono == 1){
		fprintf(pimagefile,"P5\n%u %u 255\n", *pwidth, *pheight);
		fwrite(pframe[*pi]->image, 1, (*pheight)*(*pwidth), pimagefile);}
	else{
		fprintf(pimagefile,"P6\n%u %u 255\n", *pwidth, *pheight);
		fwrite(pframe[*pi]->image, 1, (*pheight)*(*pwidth)*3, pimagefile);}
	fclose(pimagefile);
}

/*-----------------------------------------------------------------------
 *  Show images
 *-----------------------------------------------------------------------*/

void show_images(uint *pi, uint *pj, uint *pwidth, uint *pheight, cv::Mat *pimage, cv::Mat *pgray_image,
		dc1394video_frame_t *frame[], dc1394_t *d, uint *mono){

	pgray_image->data = frame[*pi]->image;

	if (*pi == 0){
		cv::namedWindow("Image_l",CV_WINDOW_AUTOSIZE);
		imshow("Image_l", *pgray_image);}
    else{
    	cv::namedWindow("Image_r",CV_WINDOW_AUTOSIZE);
    	imshow("Image_r", *pgray_image);}

}

/*-----------------------------------------------------------------------
 *  Capture and save images
 *-----------------------------------------------------------------------*/

dc1394error_t capture_and_save_images(uint *pni, uint *pNumCameras,dc1394video_mode_t *pvideo_mode,
		dc1394camera_t *pCameras[], dc1394_t *d, uint *mono){

	dc1394video_frame_t *frame[maxCameras];
	dc1394error_t err;
	uint i, j, width, height;
	FILE *imagefile;

	dc1394_get_image_size_from_video_mode(pCameras[0],*pvideo_mode, &width, &height);

	for (i = 0; i < *pNumCameras; i++) {
		err=dc1394_video_set_transmission(pCameras[i], DC1394_ON);
		DC1394_ERR_CLN_RTN(err,cleanup_and_exit(pNumCameras, pCameras),"Could not start camera iso transmission");
	}

	for (j=0; j<*pni; j++){
			for (i = 0; i < *pNumCameras; i++) {

				err=dc1394_capture_dequeue(pCameras[i], DC1394_CAPTURE_POLICY_WAIT, &frame[i]);
				DC1394_ERR_CLN_RTN(err,cleanup_and_exit(pNumCameras, pCameras),"Could not capture a frame");

				save_images(&i, &j, &width, &height, imagefile, frame, d, mono);

				dc1394_capture_enqueue (pCameras[i], frame[i]);
			}
	}
	return err;
}

/*-----------------------------------------------------------------------
 *  Capture and show images
 *-----------------------------------------------------------------------*/

dc1394error_t capture_and_show_images(uint *pni, uint *pNumCameras,dc1394video_mode_t *pvideo_mode,
		dc1394camera_t *pCameras[], dc1394_t *d, uint *mono){

	dc1394video_frame_t *frame[maxCameras];
	dc1394error_t err;
	uint i, j, width, height;
	cv::Mat image;
	cv::Mat *gray_image;

	dc1394_get_image_size_from_video_mode(pCameras[0],*pvideo_mode, &width, &height);

	gray_image = &image;

	if (*mono == 1)
		*gray_image = cvCreateMat(height, width, CV_8UC1);
	else
		*gray_image = cvCreateMat(height, width, CV_8UC3);

	for (i = 0; i < *pNumCameras; i++) {
		err=dc1394_video_set_transmission(pCameras[i], DC1394_ON);
		DC1394_ERR_CLN_RTN(err,cleanup_and_exit(pNumCameras, pCameras),"Could not start camera iso transmission");
	}

	for (j=0; j<*pni; j++){

			for (i = 0; i < *pNumCameras; i++) {

				err = dc1394_capture_dequeue(pCameras[i], DC1394_CAPTURE_POLICY_WAIT, &frame[i]);
				DC1394_ERR_CLN_RTN(err,cleanup_and_exit(pNumCameras, pCameras),"Could not capture a frame");

				show_images(&i, &j, &width, &height, &image, gray_image, frame, d, mono);

				dc1394_capture_enqueue (pCameras[i], frame[i]);
			}
    cv::waitKey(1);
	}
	return err;
}

/*-----------------------------------------------------------------------
 *  Capture save and show images
 *-----------------------------------------------------------------------*/

dc1394error_t capture_save_and_show_images(uint *pni, uint *pNumCameras,dc1394video_mode_t *pvideo_mode,
		dc1394camera_t *pCameras[], dc1394_t *d, uint *mono){

	dc1394video_frame_t *frame[maxCameras];
	dc1394error_t err;
	uint i, j, width, height;
	FILE *imagefile;
	cv::Mat image;
	cv::Mat *gray_image;

	dc1394_get_image_size_from_video_mode(pCameras[0],*pvideo_mode, &width, &height);

	gray_image = &image;

	if (*mono == 1)
		*gray_image = cvCreateMat(height, width, CV_8UC1);
	else
		*gray_image = cvCreateMat(height, width, CV_8UC3);

	for (i = 0; i < *pNumCameras; i++) {
		err=dc1394_video_set_transmission(pCameras[i], DC1394_ON);
		DC1394_ERR_CLN_RTN(err,cleanup_and_exit(pNumCameras, pCameras),"Could not start camera iso transmission");
	}

	for (j=0; j<*pni; j++){

			for (i = 0; i < *pNumCameras; i++) {

				err=dc1394_capture_dequeue(pCameras[i], DC1394_CAPTURE_POLICY_WAIT, &frame[i]);
				DC1394_ERR_CLN_RTN(err,cleanup_and_exit(pNumCameras, pCameras),"Could not capture a frame");

				save_images(&i, &j, &width, &height, imagefile, frame, d, mono);

				show_images(&i, &j, &width, &height, &image, gray_image, frame, d, mono);

				dc1394_capture_enqueue (pCameras[i], frame[i]);

			}
			cv::waitKey(0);
	}
	return err;
}
}//end namespace
