/*
 * camerasMain.cpp
 *
 *  Created on: Oct 14, 2013
 *      Author: luzdora
*/
/*
 * Get one gray image using libdc1394 and store it as portable gray map
 *    (pgm). Based on 'samplegrab' from Chris Urmson
 *
 * Written by Damien Douxchamps <ddouxchamps@users.sf.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */



#include "../include/cameraslib.hpp"

/*-----------------------------------------------------------------------
 *  Main
 *-----------------------------------------------------------------------*/
using namespace cameras;
int main()
{
	dc1394camera_t *cameras[maxCameras];
	dc1394error_t err;
	dc1394_t *d;
	uint32_t numCameras, ni, option;
	dc1394video_mode_t video_mode;
	dc1394framerate_t framerate;
	dc1394operation_mode_t op_mode;
	dc1394speed_t speed;
	uint mono;

	printf("Start\n");
	/*-----------------------------------------------------------------------
	 *  Find cameras
	 *-----------------------------------------------------------------------*/
	d = dc1394_new ();

	find_cameras(d, &numCameras, cameras);

	/*-----------------------------------------------------------------------
	 *  Selector menu, mode and frame
	 *-----------------------------------------------------------------------*/
	select_menu(&video_mode, &framerate, &op_mode, &speed, &ni, &mono);

	/*-----------------------------------------------------------------------
	 *  Modes
	 *-----------------------------------------------------------------------*/
	err = modes(&op_mode, &speed, &video_mode, &framerate, d, &numCameras, cameras);
	DC1394_ERR_CLN_RTN(err,cleanup_and_exit(&numCameras, cameras),"Failed in modes function");

	printf("Selector menu, capture:\n1--> Capture and save\n2--> Capture and show\n"
			"3--> Capture, save and show\n");
	fflush(stdout);
	scanf("%d", &option);

	switch(option){
	case 1:
		err = capture_and_save_images(&ni, &numCameras,&video_mode, cameras, d, &mono);
		DC1394_ERR_CLN_RTN(err,cleanup_and_exit(&numCameras, cameras),"Failed in capture and save");
		break;
	case 2:
		err = capture_and_show_images(&ni, &numCameras,&video_mode, cameras, d, &mono);
		DC1394_ERR_CLN_RTN(err,cleanup_and_exit(&numCameras, cameras),"Failed in capture and show");
		break;
	case 3:
		err = capture_save_and_show_images(&ni, &numCameras,&video_mode, cameras, d, &mono);
		DC1394_ERR_CLN_RTN(err,cleanup_and_exit(&numCameras, cameras),"Failed in capture save, and show");
		break;
	default:
		err = capture_save_and_show_images(&ni, &numCameras,&video_mode, cameras, d, &mono);
		DC1394_ERR_CLN_RTN(err,cleanup_and_exit(&numCameras, cameras),"Failed in capture save, and show");
		break;
	}

	/*-----------------------------------------------------------------------
	 *  Releases the cameras and exits
	 *-----------------------------------------------------------------------*/
	cleanup_and_exit(&numCameras, cameras);

	dc1394_free (d);
	printf("Finish\n");
    return 0;
}
