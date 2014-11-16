#include "Camera.h"

Camera::Camera(const SCENEid &scene_id, const ROOMid &terrian_room_id) {
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

	cam_disp[0] = 500.0f; cam_disp[1] = 10.0f;
   
   //room.AddObject(cameraBaseID); // add to room


}
//int action, OBJECTid tID, OBJECTid cID, OBJECTid dummyID, OBJECTid actorID, float actor_height, float *cam_disp, BOOL4 *hit_test

Camera::~Camera(void) {
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