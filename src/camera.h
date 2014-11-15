#include "FlyWin32.h"
#define _USE_MATH_DEFINES
#include <math.h>
// camera
// [action]
// 0 : do nothing
// 1 : This is used when character is walking or for initial setup.
//     This function set the camera to position displaced ($\vec{d}$) from a point ($\vec{O}$) treated as character's focal point.
//     The position for camera is $\vec{O} + \vec{d}$, and face direction is $-\vec{d}$.
//     Then we will do collision detection to see if camera hit the wall or not.
//     Dummy object will serve as horizontal projection of camera if camera is forced to lift up.
// 2 : This is used when keeping camera facing direction the same

void moveCamera(int action, OBJECTid tID, OBJECTid cID, OBJECTid dummyID, OBJECTid actorID, float actor_height, float *cam_disp, BOOL4 *hit_test) {
	if(action == 0) {
		return;
	} else {

		FnObject dummy, camera, terrian;
		FnCharacter actor;
		float pos[3], fDir[3], uDir[3];
		dummy.ID(dummyID);
		actor.ID(actorID);
		camera.ID(cID);
		terrian.ID(tID);
		actor.GetPosition(pos);
		uDir[0] = 0.0f; uDir[1] = 0.0f; uDir[2] = 1.0f;

		// Set position and upward direction
		pos[2] = actor_height;
		dummy.SetPosition(pos);  dummy.SetDirection(NULL, uDir);
		camera.SetPosition(pos);camera.SetDirection(NULL, uDir);

		// Set facing direction
		if(action == 1) {
			actor.GetDirection(fDir, NULL);
			fDir[2] = 0.0f;
			dummy.SetDirection(fDir, NULL);
			camera.SetDirection(fDir, NULL);
			dummy.GetDirection(fDir, NULL);
		} else if(action == 2) {
			dummy.GetDirection(fDir, NULL);
		}
		
		// Hit test 
		float pos_tmp[3], ray[3];
		float cam_hor_disp, cam_ver_disp, angle;
		*hit_test = TRUE;
		angle = cam_disp[1];
		while(true) {
			cam_hor_disp = cam_disp[0] * cos(angle * M_PI / 180.0);
			cam_ver_disp = cam_disp[0] * sin(angle * M_PI / 180.0);
			pos_tmp[0] = pos[0] + fDir[0] * cam_hor_disp;
			pos_tmp[1] = pos[1] + fDir[1] * cam_hor_disp;
			pos_tmp[2] = pos[2];
			ray[0] =  0.0f;		ray[1] =  0.0f;		ray[2] = -1.0f;
			
			if(terrian.HitTest(pos_tmp, ray) > 0 || angle >= 90.0f)  { 
				break;
			} else {
			// no hit, need to tune the camera
				angle += 2.0f;
				*hit_test = FALSE;
			}
		} 

		dummy.MoveForward(-cam_hor_disp);	dummy.MoveUp(cam_ver_disp);
		camera.GetPosition(pos_tmp);

		fDir[0] = pos[0] - pos_tmp[0]; fDir[1] = pos[1] - pos_tmp[1]; fDir[2] = actor_height + cam_disp[2] - pos_tmp[2];
		camera.SetDirection(fDir, NULL);
	}
}