/*
 * Master.h: Class of master
 * Author: Wei-Yun
 * Email: rm3028@hotmail.com.tw
 * Copyright © 2021 Wei-Yun.
 * All rights reserved.
 */

#pragma once
#include "SceneMap.h"
#include "Vector.h"
#include <GL\glut.h>


#define MAXHEALTH 100.0	//Define character's max life point

//ID of Attack animation
enum AttackAnm {
	LeftArm, 
	RightArm, 
	AttackAnm
};

//ID of character's state
enum MasState {
	MasIdle, 
	MasDeath
};


class CMaster
{
private:
	//Data of attack
	struct AttackData {
		GLfloat AttackAngle;	//Angle of katana
		GLfloat AttackSpeed;	//Speed of attack
		GLfloat AttackPower;	//Power of attack
		GLboolean AttackAnimation;	//Whether character is attacking or not
		GLboolean AttackTiming;	//Timing of the attack has damage
	};

private:
	static GLfloat spotDif[];	//Diffuse of spotlight
	static GLfloat spotSpe[];	//Specular of spotlight
	static GLfloat spotDir[];	//Direction of spotlight

	GLfloat locAngle;	//Direction of the character face to
	CVector *location;	//Location coordinate of the character
	GLboolean m_front, m_back, m_left, m_right, m_jump, spotLight;	//Movement flag
	GLfloat m_speed;	//Move speed

	CSceneMap *sceneData;	//Point to scene data (for collision detection)

	GLuint state;	//State of character

	GLuint m_katana, lifeTex, deathTex, winTex;	//Model or texture data or character
	AttackData attackData[AttackAnm];	//Attack data
	GLfloat katanaLocZ, katanaSpeedZ;	//Parameter of moving animation

	GLfloat health;	//Life point

public:
	CMaster(GLvoid);	//Construct the character
	CMaster(GLfloat locationX, GLfloat locationY, GLfloat locationZ);	//Construct the character with location coordinate
	~CMaster(GLvoid);	//Deconstruct the character

	//Set location coordinate
	GLvoid SetLocation(GLfloat locationX, GLfloat locationY, GLfloat locationZ) { location->SetVector(locationX, locationY, locationZ); }
	//Get location coordinate
	CVector* GetLocation(GLvoid) { return location; }
	//Set direction that the character face to
	GLvoid SetLocAngle(GLfloat angle) { locAngle = angle; }
	//Get direction that the character face to
	GLfloat GetLocAngle(GLvoid) { return locAngle; }
	//Set move speed
	GLvoid SetSpeed(GLfloat speed) { m_speed = speed; }
	//Get move speed
	GLfloat GetSpeed(GLvoid) { return m_speed; }
	//Set scene data
	GLvoid SetScene(CSceneMap *scene) { sceneData = scene; }
	//Get scene data
	CSceneMap* GetScene(GLvoid) { return sceneData; }
	//Get whether the spotlight is on or not
	GLboolean GetSpotState(GLvoid) { return spotLight; }
	//Get the attack timing
	GLboolean GetAttackState(GLuint id) { return attackData[id].AttackTiming; }
	//Get the attack power
	GLvoid SetAttackPower(GLuint id, GLfloat power) { attackData[id].AttackPower = power; }
	//Get the attack power
	GLfloat GetAttackPower(GLuint id) { return attackData[id].AttackPower; }

	//Load obj data to memory
	GLboolean LoadObj(GLuint *id, GLfloat scale, GLuint mode, char *path);
	//Load texture data to memory
	GLboolean LoadTexture(GLuint *id, char *path);

	//Rebirth event
	GLvoid OnRebirth(GLvoid);
	//Timer event
	GLvoid OnTimer(GLvoid);
	//Rendering the character by OpenGL function
	GLvoid OnDisplay(GLvoid);
	//Rendering the character's life by OpenGL function
	GLvoid OnDisplayLife(GLvoid);
	//Attack event
	GLvoid OnAttack(GLuint id);
	//Attacked event
	GLvoid OnAttacked(GLfloat damage);
	//Attack animation function
	GLvoid AttackAnimation(GLvoid);
	//Rendering the victory texture by OpenGL function
	GLvoid OnVictory(GLvoid);

	//Handling movement event
	GLvoid Movement(GLfloat speed);
	//Switching the move front event
	GLvoid MoveFront(GLboolean m_enable);
	//Switching the move back event
	GLvoid MoveBack(GLboolean m_enable);
	//Switching the move left event
	GLvoid MoveLeft(GLboolean m_enable);
	//Switching the move right event
	GLvoid MoveRight(GLboolean m_enable);
	//Switching the jump event
	GLvoid MoveJump(GLboolean m_enable);
	//Reset movement parameter
	GLvoid ResetMove(GLvoid) { m_front = GL_FALSE, m_back = GL_FALSE, m_left = GL_FALSE, m_right = GL_FALSE, m_jump = GL_FALSE; }
	//Turn on or off the spotlight
	GLvoid EnableSpotLight(GLboolean m_enable);
};

