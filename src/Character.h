#pragma once
#include <FlyWin32.h>
#include <map>
#include <iostream>
#include <math.h>

enum MotionState{
	IDLE = 0,
	MOVE_FORWARD = 1,
	MOVE_BACKWARD = 1<<1,
	MOVE_LEFT = 1<<2,
	MOVE_RIGHT = 1<<3,
	MOVE_LEFT_FORWARD = MOVE_LEFT + MOVE_FORWARD,
	MOVE_LEFT_BACKWARD = MOVE_LEFT+ MOVE_BACKWARD,
	MOVE_RIGHT_FORWARD = MOVE_RIGHT + MOVE_FORWARD,
	MOVE_RIGHT_BACKWARD = MOVE_RIGHT + MOVE_BACKWARD
};



enum ActionType{
	ACTION_IDLE = 0,
	ACTION_RUN = 1,
	ACTION_WALK = 2
};


class GameObject{
public:
	GameObject(){};


	 void getLeftVertDir(float *srcFDir, float *srcUDir, float *dstDir){
		FyCross(dstDir, srcUDir, srcFDir);
		normalizeVector(dstDir, 3);
	 }

	 void getRightVertDir(float *srcFDir, float *srcUDir, float *dstDir){
		FyCross(dstDir, srcFDir, srcUDir);
		normalizeVector(dstDir, 3);
	 }

	 void normalizeVector(float *vec, int size){
		float fSquareSum = 0;
		for(int i = 0; i < size; ++i){
		fSquareSum += vec[i]*vec[i];
		}
		fSquareSum = std::sqrt(fSquareSum);
			 
		for(int i = 0; i < size; ++i){
		vec[i] = vec[i]/fSquareSum;
		}
	 } 

	 void getPositionDist2D(float *vec1, float *vec2, float &dst){
		 float fDiff[2];
		 fDiff[0] = vec1[0] - vec2[0];
		 fDiff[1] = vec1[1] - vec2[1];
		float fSquareDst = fDiff[0]*fDiff[0] + fDiff[1]*fDiff[1];
		dst = std::sqrt(fSquareDst);
	 }

	 void getDstDirection(float *srcFDir, float *srcUDir, float *dstDir, MotionState state){ //得到目標位置的向量
		  float leftDir[3];
		  float rightDir[3];
		 switch(state){
		case MotionState::MOVE_FORWARD:
			dstDir[0] = srcFDir[0]; dstDir[1] = srcFDir[1]; dstDir[2] = srcFDir[2];
			break;
		case MotionState::MOVE_BACKWARD:
			dstDir[0] = -srcFDir[0]; dstDir[1] = -srcFDir[1]; dstDir[2] = -srcFDir[2];
			break;
		case MotionState::MOVE_LEFT:
			getLeftVertDir(srcFDir, srcUDir, dstDir);
			break;
		case MotionState::MOVE_LEFT_FORWARD:
			
			getLeftVertDir(srcFDir, srcUDir,leftDir);
			dstDir[0] = leftDir[0] + srcFDir[0];
			dstDir[1] = leftDir[1] + srcFDir[1];
			dstDir[2] = leftDir[2] + srcFDir[2];
			normalizeVector(dstDir, 3);
			break;
		case MotionState::MOVE_LEFT_BACKWARD:
			getLeftVertDir(srcFDir, srcUDir,leftDir);
			dstDir[0] = leftDir[0] - srcFDir[0];
			dstDir[1] = leftDir[1] - srcFDir[1];
			dstDir[2] = leftDir[2] - srcFDir[2];
			normalizeVector(dstDir, 3);
			break;
		case MotionState::MOVE_RIGHT:
			getRightVertDir(srcFDir, srcUDir, dstDir);
			break;
		case MotionState::MOVE_RIGHT_FORWARD:
			getRightVertDir(srcFDir, srcUDir, rightDir);
			dstDir[0] = rightDir[0] + srcFDir[0];
			dstDir[1] = rightDir[1] + srcFDir[1];
			dstDir[2] = rightDir[2] + srcFDir[2];
			normalizeVector(dstDir, 3);
			break;
		case MotionState::MOVE_RIGHT_BACKWARD:
			getRightVertDir(srcFDir, srcUDir, rightDir);
			dstDir[0] = rightDir[0] - srcFDir[0];
			dstDir[1] = rightDir[1] - srcFDir[1];
			dstDir[2] = rightDir[2] - srcFDir[2];
			normalizeVector(dstDir, 3);
			break;
		default:
			dstDir[0] = 0, dstDir[1], dstDir[2] = 0;
		}
	 }

	 void getVectorAngle(float *vec1, float *vec2, float &angle){
		float fDotProduct = vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2] ;
		float fNorm = std::sqrt(vec1[0]*vec1[0] + vec1[1]*vec1[1] + vec1[2]*vec1[2])* 
			std::sqrt(vec2[0]*vec2[0] + vec2[1]*vec2[1] + vec2[2]*vec2[2]);
		float cosValue = fDotProduct/fNorm;
		if(cosValue > 1){
			cosValue = 1;
		}else if(cosValue < -1){
			cosValue = -1;
		}
		angle = 180*std::acos(cosValue)/3.14159265;

		float fVertVector[3];
		FyCross(fVertVector, vec1, vec2);
		if(fVertVector[2] > 0){
			angle = -angle;
		}
	 }
};

class Character
	:public GameObject
{
public:
	Character(void);

	~Character(void);

	void initialize(const SCENEid &sceneId, const OBJECTid &dummyCameraId, const ROOMid &terrianRoomId);  //dummyCameraId 輸入camera的ID
	
	int update(int skip); //在gameAI時，更新人物動作 return 是否碰撞

	void setBaseCameraId(OBJECTid baseCameraId) {
		this->m_dummyCameraId = baseCameraId;
	}

	CHARACTERid getCharacterId(){
		return m_actorId;
	}

	MotionState getCurrentState() {
		return m_curState;
	}

	float getCharacterHeight() {
		return character_height;
	}

	FnCharacter* getController() {
		return &m_actor;
	}

	int collision; //人物是否碰撞中
private:
	float getNewRotateAngle();

private:

	//friend object id
	SCENEid m_sceneId;
	OBJECTid m_terrianId;
	CHARACTERid m_actorId;
	OBJECTid m_dummyCameraId;
	ROOMid m_terrianRoomId;

	//player controller
	FnCharacter m_actor;

	//state info
	float m_fPos3[3], m_fDir3[3], m_uDir3[3];
	float m_moveVel, m_rotateVel;
	MotionState m_curState, m_preState;
	ACTIONid m_curActionId;

	//action mapping
	std::map<MotionState, ACTIONid> m_mapState2Action;
	std::map<ACTIONid, std::string> m_mapAction2Name;
	std::map<ActionType, ACTIONid> m_mapIndex2Action;

	//character info
	float character_height;
};

