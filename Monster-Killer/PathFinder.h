/*
 * PathFinder.h: Find path by using A* Search Algorithm
 * Author: Wei-Yun
 * Email: rm3028@hotmail.com.tw
 * Copyright © 2021 Wei-Yun.
 * All rights reserved.
 */

#pragma once
#include "Vector.h"
#include <GL\glut.h>


class CPathFinder	//Find path by A* Search Algorithm
{
private:
	enum mapState { //Id of map's state data
		Unhandle, 
		Collision, 
		Checking, 
		Checked, 
		Path
	};

	struct mapStruct {
		GLuint fValue;	//F value of map
		GLuint gValue;	//G value of map
		GLuint hValue;	//H value of map
		GLuint state;	//State of map
		GLuint locX;	//Location x of map
		GLuint locY;	//Location y of map
		mapStruct *parent, *next;	//Next location and previous location pointer

		//== operator
		GLboolean operator==(mapStruct map) { if((locX == map.locX) && (locY == map.locY)) return GL_TRUE; else return GL_FALSE; }
		//!= operator
		GLboolean operator!=(mapStruct map) { if((locX != map.locX) || (locY != map.locY)) return GL_TRUE; else return GL_FALSE; }
	};

	GLuint mapWidth, mapHeight;	//Scene's width and height
	mapStruct **mapData, *location;	//Scene's two dimensional array data and current location pointer

public:
	CPathFinder(void);	//Find path object constructer
	~CPathFinder(void);	//Find path object deconstructer

	GLboolean LoadMapData(GLboolean *m_data, GLuint width, GLuint height);	//Load scene's collision data
	GLboolean FindPath(GLuint locX, GLuint locY, GLuint desX, GLuint desY);	//Find path by A* Search Algorithm

	CVector GetNextLoc(void);	//Get the next destination
};

