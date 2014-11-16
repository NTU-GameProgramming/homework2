#include "FlyWin32.h"
#define _USE_MATH_DEFINES
#include <math.h>


class Camera {
	public:
		Camera(const SCENEid &scene_id, const ROOMid &terrian_room_id);

		~Camera(void);

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
		ROOMid terrian_room_id;
		FnCamera camera;
		FnObject camera_base;
		float cam_disp[2]; // displacement radius and lifted angle in degree
}

