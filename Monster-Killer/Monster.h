/*
 * Monster.h: Class of monster
 * Author: Wei-Yun
 * Email: rm3028@hotmail.com.tw
 * Copyright © 2021 Wei-Yun.
 * All rights reserved.
 */

#pragma once
#include "Vector.h"
#include "PathFinder.h"
#include "SceneMap.h"
#include <ctime>
#define _USE_MATH_DEFINES
#include <cmath>
#include <GL\glut.h>


#define MAXHEALTH 100.0	//Define monster's max life point


//ID of monster's state
enum MonState {
	MonIdle, 
	MonMove, 
	MonAttacked, 
	MonDeathing, 
	MonDeath
};


class CMonster
{
private:
	GLuint state;	//State of monster
	GLfloat locAngle, locAngleX, leftHand, rightHand;	//Angle of body
	CVector *location, *desLocation;	//Current location and destination location
	CSceneMap *sceneData;	//Data of scene
	CPathFinder *pathFinder;	//Find path object

	CVector nextLoc, oneLength;	//The next location and lenth of one step
	GLboolean step, moved;	//Flag of movement

	GLfloat health;	//Life point
	GLfloat attackPower;	//Attack power

public:
	CMonster(void);	//Construct the monster
	CMonster(GLfloat locationX, GLfloat locationY, GLfloat locationZ);	//Construct the monster with location
	~CMonster(void);	//Deconstruct the monster

	//Set location coordinate
	GLvoid SetLocation(GLfloat locationX, GLfloat locationY, GLfloat locationZ) { location->SetVector(locationX, locationY, locationZ); }
	//Get location coordinate
	CVector* GetLocation(GLvoid) { return location; }
	//Set direction that the character face to
	GLvoid SetLocAngle(GLfloat angle) { locAngle = angle; }
	//Get direction that the character face to
	GLfloat GetLocAngle(GLvoid) { return locAngle; }
	//Set scene data
	GLvoid SetScene(CSceneMap *scene) { sceneData = scene; pathFinder->LoadMapData(sceneData->GetMapData(), sceneData->GetMapWidth(), sceneData->GetMapHeight()); }
	//Get scene data
	CSceneMap* GetScene(GLvoid) { return sceneData; }
	//Get the state of monster
	GLuint GetState(GLvoid) { return state; }
	//Set the attack power of monster
	GLvoid SetAttackPower(GLfloat power) { attackPower = power; }
	//Get the attack power of monster
	GLfloat GetAttackPower(GLvoid) { return attackPower; }

	//Rebirth event
	GLvoid OnRebirth(GLvoid);
	//Collision event
	GLvoid OnCollision(GLvoid);
	//Attacked event
	GLvoid OnAttacked(GLfloat damage);
	//Render the monster by OpenGL function
	GLvoid OnDisplay(GLvoid);
	//Rendering the monster's life by OpenGL function
	GLvoid OnDisplayLife(GLfloat lookAngle);

	//Monster AI
	GLvoid StateLoop(GLvoid);
};

