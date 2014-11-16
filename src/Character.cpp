#include "Character.h"


Character::Character(void)
{
}




Character::~Character(void)
{
}

void Character::initialize(const SCENEid &sceneId, const OBJECTid &dummyCameraId, const ROOMid &terrianRoomId){ 
	m_sceneId = sceneId;
	m_dummyCameraId = dummyCameraId;
	m_terrianRoomId = terrianRoomId;

	//load character
	FnScene scene(sceneId); 
	m_actorId =   scene.LoadCharacter("Lyubu2");
	m_actor.ID(m_actorId);

	//put on terrian
	FnRoom room(terrianRoomId);
	room.AddObject(m_actorId);
	
	m_fDir3[0] = 1; m_fDir3[1] = 0; m_fDir3[2] = 0;
	m_uDir3[0] = 0, m_uDir3[1] = 0; m_uDir3[2] = 1;
	m_fPos3[0] = 3569.0; m_fPos3[1] = -3208.0; m_fPos3[2] = 0;
	m_actor.SetDirection(m_fDir3, m_uDir3);
	m_actor.SetTerrainRoom(terrianRoomId, 400.f);

	m_actor.PutOnTerrain(m_fPos3, 200);
	m_actor.SetPosition(m_fPos3);
	
	//set up action
	ACTIONid idleId = m_actor.GetBodyAction(NULL, "Idle");
	ACTIONid runId = m_actor.GetBodyAction(NULL, "Run");
	ACTIONid walkId = m_actor.GetBodyAction(NULL, "Walk");
	m_mapAction2Name.insert(std::pair<ACTIONid, std::string>(idleId, "Idle"));
	m_mapAction2Name.insert(std::pair<ACTIONid, std::string>(runId, "Run"));
	m_mapAction2Name.insert(std::pair<ACTIONid, std::string>(walkId, "Walk"));
	m_mapIndex2Action.insert(std::pair<ActionType, ACTIONid>(ACTION_IDLE, idleId));
	m_mapIndex2Action.insert(std::pair<ActionType, ACTIONid>(ACTION_RUN, runId));
	m_mapIndex2Action.insert(std::pair<ActionType, ACTIONid>(ACTION_WALK, walkId));
	
	m_mapState2Action.insert(std::pair<MotionState, ACTIONid>(MotionState::IDLE, idleId));
	m_mapState2Action.insert(std::pair<MotionState, ACTIONid>(MotionState::MOVE_BACKWARD, runId));
	m_mapState2Action.insert(std::pair<MotionState, ACTIONid>(MotionState::MOVE_FORWARD, runId));
	m_mapState2Action.insert(std::pair<MotionState, ACTIONid>(MotionState::MOVE_LEFT, runId));
	m_mapState2Action.insert(std::pair<MotionState, ACTIONid>(MotionState::MOVE_LEFT_BACKWARD, runId));
	m_mapState2Action.insert(std::pair<MotionState, ACTIONid>(MotionState::MOVE_LEFT_FORWARD, runId));
	m_mapState2Action.insert(std::pair<MotionState, ACTIONid>(MotionState::MOVE_RIGHT, runId));
	m_mapState2Action.insert(std::pair<MotionState, ACTIONid>(MotionState::MOVE_RIGHT_BACKWARD, runId));
	m_mapState2Action.insert(std::pair<MotionState, ACTIONid>(MotionState::MOVE_RIGHT_FORWARD, runId));
	
	m_curActionId = idleId;
	m_actor.SetCurrentAction(NULL, 0, m_curActionId);
	m_actor.Play(START, 0.0f, FALSE, TRUE);

	m_curState = MotionState::IDLE;
	m_preState = MotionState::IDLE;

	m_rotateVel = 10;
	m_moveVel = 10;

}

int Character::update(int skip){
	//detect keyevent
	int success = 0;
	int newState = 0;
	if(FyCheckHotKeyStatus(FY_UP)){
		newState = newState|MOVE_FORWARD;
		std::cout<<"up key\n";
	}												  
	if(FyCheckHotKeyStatus(FY_DOWN)){
		newState = newState|MOVE_BACKWARD;
		std::cout<<"down key\n";
	}
	if(FyCheckHotKeyStatus(FY_LEFT)){
		newState = newState|MOVE_LEFT;
		std::cout<<"left key\n";
	}
	if(FyCheckHotKeyStatus(FY_RIGHT)){
		newState = newState|MOVE_RIGHT;
		std::cout<<"right key\n";
	}
	std::cout.flush();

	switch(newState){
	case (int)MotionState::IDLE:
	case (int)MotionState::MOVE_FORWARD:
	case (int)MotionState::MOVE_BACKWARD:
	case (int)MotionState::MOVE_LEFT_FORWARD:
	case (int)MotionState::MOVE_LEFT_BACKWARD:
	case (int)MotionState::MOVE_RIGHT_FORWARD:
	case (int)MotionState::MOVE_RIGHT_BACKWARD:
	case (int)MotionState::MOVE_LEFT:
	case (int)MotionState::MOVE_RIGHT:
		m_curState = (MotionState)newState;
		break;
	default:
		m_curState = m_preState;
	}

   //animation
	if(m_mapState2Action[m_curState] != m_curActionId){
		 m_curActionId = m_mapState2Action[m_curState];
		 m_actor.SetCurrentAction(NULL, 0, m_curActionId, 5.0f);
		 m_actor.Play(START, 0.0f, FALSE, TRUE);
					  
	}else{
		 m_actor.Play(LOOP, (float)skip, FALSE, TRUE);
	}

	//check motion state
	FnObject dummy(m_dummyCameraId);
	float fDummyFDir[3];
	float fDummyUDir[3];
	float fDummyPos[3];
	float fDstDir[3];
	dummy.GetDirection(fDummyFDir, fDummyUDir);

	switch(m_curState){
	case MotionState::IDLE:
		break;
	case MotionState::MOVE_FORWARD:
	case MotionState::MOVE_BACKWARD:
	case MotionState::MOVE_LEFT_FORWARD:
	case MotionState::MOVE_LEFT_BACKWARD:
	case MotionState::MOVE_RIGHT_FORWARD:
	case MotionState::MOVE_RIGHT_BACKWARD:
		getDstDirection(fDummyFDir, fDummyUDir, fDstDir, m_curState);
		float fAngleDiff;
		getVectorAngle(m_fDir3, fDstDir, fAngleDiff);
		if(std::fabs(fAngleDiff) <= m_rotateVel){
			m_actor.TurnRight(fAngleDiff);
			//int success = m_actor.MoveForward(m_moveVel, TRUE, FALSE,  2, TRUE );
			success = m_actor.MoveForward(m_moveVel, TRUE, false, FALSE, TRUE);
		}else{
			float fSign = fAngleDiff/std::fabs(fAngleDiff);
			success = m_actor.TurnRight(fSign*m_rotateVel);
		}
		break;
	case MotionState::MOVE_LEFT:
		dummy.GetPosition(fDummyPos);
		getDstDirection(fDummyFDir, fDummyUDir, fDstDir, m_curState);
		getVectorAngle(m_fDir3, fDstDir, fAngleDiff);
		if(std::fabs(fAngleDiff) <= m_rotateVel){
			m_actor.TurnRight(fAngleDiff);
			float fDistance;
			getPositionDist2D(fDummyPos, m_fPos3, fDistance);
			double fHalfAngle = -asin(0.5*m_moveVel/fDistance)*180/3.14159265;
			m_actor.TurnRight(fHalfAngle);
			success = m_actor.MoveForward(m_moveVel, TRUE, false, FALSE, TRUE);

		//	dummy.TurnRight(2*fHalfAngle);
		}else{
			float fSign = fAngleDiff/std::fabs(fAngleDiff);
			success = m_actor.TurnRight(fSign*m_rotateVel);
		}
		
		break;
	case MotionState::MOVE_RIGHT:
		dummy.GetPosition(fDummyPos);
		getDstDirection(fDummyFDir, fDummyUDir, fDstDir, m_curState);
		getVectorAngle(m_fDir3, fDstDir, fAngleDiff);
		if(std::fabs(fAngleDiff) <= m_rotateVel){
			m_actor.TurnRight(fAngleDiff);
			float fDistance;
			getPositionDist2D(fDummyPos, m_fPos3, fDistance);
			double fHalfAngle = asin(0.5*m_moveVel/fDistance)*180/3.14159265;
			m_actor.TurnRight(fHalfAngle);
			success = m_actor.MoveForward(m_moveVel, TRUE, false, FALSE, TRUE);

		//	dummy.TurnRight(2*fHalfAngle);		
		}else{
			float fSign = fAngleDiff/std::fabs(fAngleDiff);
			success = m_actor.TurnRight(fSign*m_rotateVel);
		}
		break;
	}

	//update state 
	m_preState = m_curState;
	m_actor.GetPosition(m_fPos3);
	m_actor.GetDirection(m_fDir3, m_uDir3);
	collision = success;
	return success;
}
