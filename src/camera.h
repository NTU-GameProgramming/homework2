#pragma once
#include "FlyWin32.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Character.h"

class Camera {
	public:
		Camera(void);
		~Camera(void);

		void initialize(const SCENEid &scene_id, const ROOMid &terrian_room_id, Character *character);

		void GameAIupdate(int skip);
		void resetCamera(void);
		OBJECTid getCameraId(){
			return camera_id;
		}

		OBJECTid getCameraBaseId(){
			return camera_base_id;
		}

		FnCamera& getCamera() {
			return camera;
		}

		FnObject& getCameraBase() {
			return camera_base;
		}

	private:
		
		SCENEid scene_id;
		OBJECTid camera_id, camera_base_id;
		ROOMid terrian_room_id;
		
		FnCamera camera;
		FnObject camera_base;
		Character *character;
		FnCharacter *char_ctrl;

		float cam_disp_polar[2]; // displacement radius and lifted angle in degree
		float cam_disp_cart[2];  // displacement in cartesian

		float char_fDir[3];		 // it is needed to record the old face direction of character
};

