#ifndef CAMERASLIB_HPP_
#define CAMERASLIB_HPP_

#include <stdio.h>
#include <stdint.h>
#include <dc1394/dc1394.h>
#include <stdlib.h>
#include <inttypes.h>
#include "opencv/cv.h"
#include "opencv/highgui.h"

#define maxCameras 2
#define GUDUNIBRAIN 582164335768605236
#define GUIDSRR 2892819673564372
#define GUIDSRL 2892819673564371

namespace cameras{

void cleanup_and_exit(uint32_t *pNumCameras, dc1394camera_t *pCameras[]);

void find_cameras(dc1394_t *d,uint32_t *pNumCameras, dc1394camera_t *pCameras[]);

void select_menu(dc1394video_mode_t *pvideo_mode, dc1394framerate_t *pframerate,
		dc1394operation_mode_t *pop_mode, dc1394speed_t *pspeed, uint *pni, uint *mono);

dc1394error_t modes(dc1394operation_mode_t *pop_mode, dc1394speed_t *pspeed,
		dc1394video_mode_t *pvideo_mode, dc1394framerate_t *pframerate,
		dc1394_t *d,uint32_t *pNumCameras, dc1394camera_t *pCameras[]);

void save_images(uint *i, uint *j, uint *width, uint *height, FILE *imagefile,
		dc1394video_frame_t *frame[], dc1394_t *d, uint *mono);

void show_images(uint *i, uint *j, uint *width, uint *height, cv::Mat *image, cv::Mat *gray_image,
		dc1394video_frame_t *frame[], dc1394_t *d, uint *mono);

dc1394error_t capture_and_save_images(uint *pni, uint *pNumCameras,dc1394video_mode_t *pvideo_mode,
		dc1394camera_t *pCameras[], dc1394_t * d, uint *mono);

dc1394error_t capture_and_show_images(uint *pni, uint *pNumCameras,dc1394video_mode_t *pvideo_mode,
		dc1394camera_t *pCameras[], dc1394_t * d, uint *mono);

dc1394error_t capture_save_and_show_images(uint *pni, uint *pNumCameras,dc1394video_mode_t *pvideo_mode,
		dc1394camera_t *pCameras[], dc1394_t * d, uint *mono);
}//end namespace

#endif
