/*
 * MonsterKiller.h: Class of game
 * Author: Wei-Yun
 * Email: rm3028@hotmail.com.tw
 * Copyright © 2021 Wei-Yun.
 * All rights reserved.
 */

#pragma once
#include "SceneMap.h"
#include "Camera.h"
#include "Master.h"
#include "Monster.h"
#include <thread>
#include <GL\glut.h>
#include <GL\freeglut_ext.h>

using namespace std;

#define MonsterCount 3	//Define the count of monster

namespace MonsterKiller {

	class CMonsterKiller
	{
	private:
		static CMonsterKiller*instance;	//Pointer to the Homework4 object
		static GLfloat LightPos[];	//Light0 position
		CCamera *camera;	//Camera object pointer
		CSceneMap *sceneMap;	//SceneMap object pointer
		CMaster *master;	//Character object pointer
		CMonster *monster;	//Monster object pointer
		thread *monState;	//Thread of monster
		GLboolean aerialMap;	//Aerial map switch
		GLboolean lifeBar;	//Life bar of monster switch
		GLboolean exitWindow;	//Exit flag

	public:
		//Construct the game object
		CMonsterKiller();
		//Construct the glut OpenGL environment window
		CMonsterKiller(int argc, char* argv[]);
		//Deconstruct the game object
		~CMonsterKiller(GLvoid);

		//Create the glut OpenGL environment window
		GLvoid OnCreate(int argc, char* argv[]);
		//OnClose event
		GLvoid OnClose(GLvoid);

		//OnTimer event
		GLvoid OnTimer(int id);
		//OnDisplay event
		GLvoid OnDisplay(GLvoid);
		//OnReshap event
		GLvoid OnReshape(int width, int height);
		//OnMouse event
		GLvoid OnMouseFunc(int button, int state, int x, int y) ;
		//OnKeyboard event
		GLvoid OnKeyboardFunc(unsigned char key, int x, int y);
		//OnKeyboardUp event
		GLvoid OnKeyboardUpFunc(unsigned char key, int x, int y);

		//Set close event
		static GLvoid SetClose(GLvoid);
		//Set timer event
		static GLvoid SetTimer(int id);
		//Set display event
		static GLvoid SetDisplay(GLvoid);
		//Set reshape event
		static GLvoid SetReshape(int width, int height);
		//Set mouse event
		static GLvoid SetMouseFunc(int button, int state, int x, int y);
		//Set keyboard event
		static GLvoid SetKeyboardFunc(unsigned char key, int x, int y);
		//Set keyboardUp event
		static GLvoid SetKeyboardUpFunc(unsigned char key, int x, int y);
		
		//Thread function of monster
		GLvoid MonsterTimer(GLuint id);
		//Set the thread function of monster
		static GLvoid SetMonsterTimer(GLuint id);
	};

}
