#include "Camera.h"

Camera::Camera(void) {
}
Camera::~Camera(void) {
}

void Camera::initialize(const SCENEid &scene_id, const ROOMid &terrian_room_id, Character *character) {
	this->character = character;
	this->char_ctrl = character->getController();
	this->scene_id = scene_id;
	this->terrian_room_id = terrian_room_id;
	
	FnScene scene(scene_id);
	camera_base_id = scene.CreateObject(OBJECT);
	camera_base.ID(camera_base_id);

	camera_id = scene.CreateObject(CAMERA);
	camera.ID(camera_id);
	
	camera.SetNearPlane(5.0f);
	camera.SetFarPlane(100000.0f);
	camera.SetParent(camera_base_id);



	cam_disp_polar[0] = 500.0f; cam_disp_polar[1] = 10.0f;
	cam_disp_cart[0] = cam_disp_polar[0] * cos(cam_disp_polar[1] * M_PI / 180.0);
	cam_disp_cart[1] = cam_disp_polar[0] * sin(cam_disp_polar[1] * M_PI / 180.0);
   //room.AddObject(cameraBaseID); // add to room
}

void Camera::resetCamera() {
	float pos[3], fDir[3], uDir[3];

	// set camera_base position and direction
	char_ctrl->GetPosition(pos);
	pos[2] = character->getCharacterHeight();

	char_ctrl->GetDirection(fDir, NULL);
	fDir[2] = 0.0f; // force to set to projection on horizontal plane
	char_old_fDir[0] = fDir[0];	char_old_fDir[1] = fDir[1];	char_old_fDir[2] = fDir[2];

	uDir[0] = 0.0f; uDir[1] = 0.0f; uDir[2] = 1.0f;
	
	camera_base.SetPosition(pos);
	camera_base.SetDirection(NULL, uDir);
	camera_base.SetDirection(fDir, NULL);
	camera.SetDirection(fDir, uDir);
	

	// move to displaced position
	camera_base.MoveForward(-cam_disp_cart[0]);

	float pos_camera[3];
	camera_base.GetPosition(pos_camera);
	pos_camera[2] = pos[2] + cam_disp_cart[1];
	fDir[0] = pos[0] - pos_camera[0]; fDir[1] = pos[1] - pos_camera[1]; fDir[2] = pos[2] - pos_camera[2];
	camera.SetPosition(pos_camera);
	camera.SetDirection(NULL, uDir);
	camera.SetDirection(fDir, NULL);
}

void Camera::GameAIupdate(int skip) {
	float pos_char[3], pos_camera[3], pos_camerabase[3], fDir[3], uDir[3];
	MotionState ms = character->getCurrentState();


	
	switch(ms) {
		// if character is moving in left/right
		case MotionState::MOVE_LEFT:
		case MotionState::MOVE_RIGHT:
			char_ctrl->GetPosition(pos_char);
			camera_base.GetPosition(pos_camerabase);
			fDir[0] = pos_char[0] - pos_camerabase[0];fDir[1] = pos_char[1] - pos_camerabase[1];fDir[2]=0.0f;
			char_old_fDir[0] = fDir[0];
			char_old_fDir[1] = fDir[1];
			char_old_fDir[2] = 0;
			camera_base.SetDirection(fDir, NULL);
			
			break;
		
		// if character is moving other than left/right
		case MotionState::MOVE_BACKWARD:
		case MotionState::MOVE_FORWARD:
		case MotionState::MOVE_LEFT_BACKWARD:
		case MotionState::MOVE_RIGHT_BACKWARD:
		case MotionState::MOVE_LEFT_FORWARD:
		case MotionState::MOVE_RIGHT_FORWARD:
			//先決定BaseCamera的位置，但是位移方向使用舊的
			char_ctrl->GetPosition(pos_char);
			pos_char[2] = character->getCharacterHeight();
			camera_base.SetPosition(pos_char);
			camera_base.SetDirection(char_old_fDir, NULL);
			// move to displaced position
			camera_base.MoveForward(-cam_disp_cart[0]);
			break;
		case MotionState::IDLE:
		default:
			return;
			break;
			
	}

	// 決定Camera的fDir
	uDir[0] = 0.0f; uDir[1] = 0.0f; uDir[2] = 1.0f;

	char_ctrl->GetPosition(pos_char);
	camera_base.GetPosition(pos_camerabase);
	pos_camera[0] = pos_camerabase[0];
	pos_camera[1] = pos_camerabase[1];
	pos_camera[2] = pos_camerabase[2] + cam_disp_cart[1];
	
	fDir[0] = pos_char[0] - pos_camera[0]; fDir[1] = pos_char[1] - pos_camera[1]; fDir[2] = pos_char[2] - pos_camera[2];
	
	camera.SetPosition(pos_camera);
	camera.SetDirection(NULL, uDir);
	camera.SetDirection(fDir, NULL);
	// 再做碰撞偵測
/*		float pos_tmp[3], ray[3];
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
		*/
}

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

/*
		!Camera(void);

		void GameAIupdate(int skip);

		OBJECTid getCameraId(){
			return camera_id;
		}

		OBJECTid getCameraBaseId(){
			return camera_base_id;
		}

	private:
		SCENEid scene_id;
		OBJECTid camera_id, camera_base_id;
}
*/