// HW1.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <map>
#include <FlyWin32.h>
#include "Character.h"

VIEWPORTid viewportID;	//major viewe port
SCENEid sceneID;	//3d scene
OBJECTid cameraID, cameraBaseID, terrainID, lightID;
CHARACTERid actorID;
ACTIONid idleID, runID, curPoseID;
ROOMid terrainRoomID = FAILED_ID;
TEXTid textID = FAILED_ID;
Character actor;

//global value

int frame = 0;

int oldX, oldY, oldXM, oldYM, oldXMM, oldYMM;
std::map<MotionState, ACTIONid> state2ActionTable;
//BOOL4 poseChange = FALSE;

//hotkey callback
void QuitGame(BYTE, BOOL4);
void Movement(BYTE, BOOL4);

//timer callback
void GameAI(int);
void RenderIt(int);

//mouse callback
void InitPivot(int, int);
void PivotCam(int, int);
void InitMove(int, int);
void MoveCam(int, int);
void InitZoom(int, int);
void ZoomCam(int, int);

void FyMain(int argc, char **argv)
{
	
	std::cout<<"Start Game";
	std::cout.flush();
	//create a new window
	BOOL4 success = FyStartFlyWin32("HomeWork 1 - with Fly2", 0, 0, 1024, 768, FALSE);
	
	//set up path
	FySetShaderPath("Data\\NTU5\\Shaders");
	FySetModelPath("Data\\NTU5\\Scenes");
	FySetTexturePath("Data\\NTU5\\Scenes\\Textures");
	FySetScenePath("Data\\NTU5\\Scenes");

	//create a viewport
	viewportID = FyCreateViewport(0, 0, 1024, 768);
	FnViewport viewport(viewportID);

	//create 3D scene
	sceneID = FyCreateScene(10);
	FnScene scene(sceneID);

	success = scene.Load("gameScene01");
	scene.SetAmbientLights(1.0f, 1.0f, 1.0f, 0.6f, 0.6f, 0.6f);

	//load the terrain
	terrainID = scene.CreateObject(OBJECT);
	FnObject terrain;
	terrain.ID(terrainID);
	terrain.Load("terrain");
	terrain.Show(FALSE);

	//set terrain environment
	terrainRoomID = scene.CreateRoom(SIMPLE_ROOM, 10);
	FnRoom room;
	room.ID(terrainRoomID);
	room.AddObject(terrainID);

	//load the character
	FySetModelPath("Data\\NTU5\\Characters");
	FySetTexturePath("Data\\NTU5\\Characters");
	FySetCharacterPath("Data\\NTU5\\Characters");


   //camera parent
	float pos[3], fDir[3], uDir[3];
	pos[0] = 3000, pos[1] = -3208; pos[2] = 0;
	fDir[0] = 1, fDir[1] = 0; fDir[2] = 0;
	uDir[0] = 0, uDir[1] = 0, uDir[2] = 1;
   cameraBaseID = scene.CreateObject(OBJECT);
   FnObject cameraBase;
   cameraBase.ID(cameraBaseID);
   cameraBase.SetPosition(pos);
   cameraBase.SetDirection(fDir, uDir);

   
   actor.initialize(sceneID, cameraBaseID, terrainRoomID);
   actorID = actor.getCharacterId();
   //camera
   cameraID = scene.CreateObject(CAMERA);
   FnCamera camera;
   camera.ID(cameraID);
   camera.SetNearPlane(5.0f);
   camera.SetFarPlane(50000.f);
   

   //fDir[0] = 0.0f;
  // camera.SetDirection(fDir, uDir, FALSE);
   //set camera initial position and orientation
   
   pos[0] = 3000; pos[1] = -3200; pos[2] = 100;
   fDir[0] = 0.983f; fDir[1] = -0.143f; fDir[2] = -0.119f;
   uDir[0] = -0.116f; uDir[1] = -0.031f; uDir[2] = 0.993f;
   camera.SetPosition(pos);
   camera.SetDirection(fDir, uDir);   

   FnLight light;
   lightID = scene.CreateObject(LIGHT);
   light.ID(lightID);
   light.Translate(70.0f, -70.0f, 70.0f, REPLACE);
   light.SetColor(1.0f, 1.0f, 1.0f);
   light.SetIntensity(1.0f);

   //create a text object for display message on screen
   textID = FyCreateText("Trebuchet MS", 18, FALSE, FALSE);

   //set hotkey	
   /*
   FyDefineHotKey(FY_ESCAPE, QuitGame, FALSE);
   FyDefineHotKey(FY_UP, Movement, FALSE);
   FyDefineHotKey(FY_RIGHT, Movement, FALSE);
   FyDefineHotKey(FY_LEFT, Movement, FALSE);
   FyDefineHotKey(FY_DOWN, Movement, FALSE);
	*/
   //define some mouse function
   FyBindMouseFunction(LEFT_MOUSE, InitPivot, PivotCam, NULL, NULL);
   FyBindMouseFunction(MIDDLE_MOUSE, InitZoom, ZoomCam, NULL, NULL);
   FyBindMouseFunction(RIGHT_MOUSE, InitMove, MoveCam, NULL, NULL);

   //bind timers, frame rate = 30 fps
   FyBindTimer(0, 30.0f, GameAI, TRUE);
   FyBindTimer(1, 30.0f, RenderIt, TRUE);
   
	//invoke the system
   FyInvokeFly(TRUE);
}

void RenderIt(int skip){
	FnCharacter actor;
	actor.ID(actorID);
	float pos[3], fDir[3], uDir[3];
	actor.GetPosition(pos, FALSE);
	actor.GetDirection(fDir, uDir);

	FnViewport vp;

	//render the whole scene
	vp.ID(viewportID);
	vp.Render3D(cameraID, TRUE, TRUE);

	//get camera's data
	FnCamera camera;
	camera.ID(cameraID);

//	camera.GetPosition(pos);
//	camera.GetDirection(fDir, uDir);
//	camera.SetDirection(fDir, uDir);
//	pos[1] -= 400;
//	pos[2] += 40;
//	camera.SetPosition(pos);

	//show frame rate
	static char string[128];
	if(frame == 0){
		FyTimerReset(0);
	}

	if((frame/10)*10 == frame){
		float curTime;
		curTime = FyTimerCheckTime(0);
		sprintf_s(string, "Fps: %6.2f", frame/curTime);
	}

	frame += skip;
	if(frame >= 1000){
		frame = 0;
	}

	FnText text;
	text.ID(textID);

	text.Begin(viewportID);
	text.Write(string, 20, 20, 255, 0, 0);

	char posS[256], fDirS[256], uDirS[256];
	sprintf_s(posS, "pos: %8.3f %8.3f %8.3f", pos[0], pos[1], pos[2]);
	sprintf_s(fDirS, "facing: %8.3f %8.3f %8.3f", fDir[0], fDir[1], fDir[2]);
	sprintf_s(uDirS, "up: %8.3f %8.3f %8.3f", uDir[0], uDir[1], uDir[2]);

   text.Write(posS, 20, 35, 255, 255, 0);
   text.Write(fDirS, 20, 50, 255, 255, 0);
   text.Write(uDirS, 20, 65, 255, 255, 0);

   text.End();

   FySwapBuffers();
}

void GameAI(int skip){
	 actor.update(skip);
}



void Movement(BYTE code, BOOL4 value){
	/*
	std::cout<<"Movement\n";
	std::cout.flush();
	FnCharacter actor;
	actor.ID(actorID);
	switch(code){
	case FY_UP:
		if(value){
			if(curPoseID != runID){
				curPoseID = runID;
				poseChange = TRUE;
			}
			playerState = MOVE_FORWARD;
		}else{
				if(curPoseID != idleID){
				curPoseID = idleID;
				poseChange = TRUE;
			}
			playerState = IDLE;
		}
		break;
	case FY_DOWN:
		if(value){
			if(curPoseID != runID){
				curPoseID = runID;
				poseChange = TRUE;
			}
			playerState = MOVE_BACKWARD;
		}else{
				if(curPoseID != idleID){
				curPoseID = idleID;
				poseChange = TRUE;
			}
			playerState = IDLE;
		}
		break;
	case FY_LEFT:
		if(value){
			if(curPoseID != runID){
				curPoseID = runID;
				poseChange = TRUE;
			}
			playerState = ROTATE_LEFT;
		}else{
				if(curPoseID != idleID){
				curPoseID = idleID;
				poseChange = TRUE;
			}
			playerState = IDLE;
		}
		break;
	case FY_RIGHT:
		if(value){
			if(curPoseID != runID){
				curPoseID = runID;
				poseChange = TRUE;
			}
			playerState = ROTATE_RIGHT;
		}else{
			if(curPoseID != idleID){
				curPoseID = idleID;
				poseChange = TRUE;
			}
			playerState = IDLE;
		}
		break;
	}
	*/
}

/*------------------
  quit the demo
  C.Wang 0327, 2005
 -------------------*/
void QuitGame(BYTE code, BOOL4 value)
{
   if (code == FY_ESCAPE) {
      if (value) {
         FyQuitFlyWin32();
      }
   }
}

/*-----------------------------------
  initialize the pivot of the camera
  C.Wang 0329, 2005
 ------------------------------------*/
void InitPivot(int x, int y)
{
   oldX = x;
   oldY = y;
   frame = 0;
}


/*------------------
  pivot the camera
  C.Wang 0329, 2005
 -------------------*/
void PivotCam(int x, int y)
{
   FnObject model;

   if (x != oldX) {
      model.ID(cameraID);
      model.Rotate(Z_AXIS, (float) 0.2*(x - oldX), GLOBAL);
      oldX = x;
   }

   if (y != oldY) {
      model.ID(cameraID);
      model.Rotate(X_AXIS, (float) 0.2*(y - oldY), GLOBAL);
      oldY = y;
   }
}


/*----------------------------------
  initialize the move of the camera
  C.Wang 0329, 2005
 -----------------------------------*/
void InitMove(int x, int y)
{
   oldXM = x;
   oldYM = y;
   frame = 0;
}


/*------------------
  move the camera
  C.Wang 0329, 2005
 -------------------*/
void MoveCam(int x, int y)
{
   if (x != oldXM) {
      FnObject model;

      model.ID(cameraID);
      model.Translate((float)(x - oldXM)*2.0f, 0.0f, 0.0f, LOCAL);
      oldXM = x;
   }
   if (y != oldYM) {
      FnObject model;

      model.ID(cameraID);
      model.Translate(0.0f, (float)(oldYM - y)*2.0f, 0.0f, LOCAL);
      oldYM = y;
   }
}


/*----------------------------------
  initialize the zoom of the camera
  C.Wang 0329, 2005
 -----------------------------------*/
void InitZoom(int x, int y)
{
   oldXMM = x;
   oldYMM = y;
   frame = 0;
}


/*------------------
  zoom the camera
  C.Wang 0329, 2005
 -------------------*/
void ZoomCam(int x, int y)
{
   if (x != oldXMM || y != oldYMM) {
      FnObject model;

      model.ID(cameraID);
      model.Translate(0.0f, 0.0f, (float)(x - oldXMM)*10.0f, LOCAL);
      oldXMM = x;
      oldYMM = y;
   }
}

