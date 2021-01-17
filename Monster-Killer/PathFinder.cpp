#include "stdafx.h"
#include "PathFinder.h"


//Find path object constructer
CPathFinder::CPathFinder(void) : mapData(nullptr), mapWidth(0), mapHeight(0)
{
}


//Find path object deconstructer
CPathFinder::~CPathFinder(void)
{
	if(mapData != nullptr) {
		for(int i = 0; i < mapHeight; i++)
			delete[] mapData[i];
		delete[] mapData;
	}
}


//Load scene's collision data
GLboolean CPathFinder::LoadMapData(GLboolean *m_data, GLuint width, GLuint height)
{
	mapWidth = width;
	mapHeight = height;

	mapData = new mapStruct*[mapHeight];
	for(int i = 0; i < mapHeight; i++) {
		mapData[i] = new mapStruct[mapWidth];
		for(int j = 0; j < mapWidth; j++) {
			if(m_data[mapWidth * i + j])
				mapData[i][j].state = Collision;
			else
				mapData[i][j].state = Unhandle;
			mapData[i][j].next = nullptr;
			mapData[i][j].parent = nullptr;
			mapData[i][j].locX = j;
			mapData[i][j].locY = i;
		}
	}

	return GL_TRUE;
}


//Find path by A* Search Algorithm
GLboolean CPathFinder::FindPath(GLuint locX, GLuint locY, GLuint desX, GLuint desY)
{
	if(locX < 0 || locX >= mapWidth || locY < 0 || locY >= mapHeight || desX < 0 || desX >= mapWidth || desY < 0 || desY >= mapHeight || mapData == nullptr)
		return GL_FALSE;

	if(mapData[desY][desX].state == Collision)
		return GL_FALSE;

	if(locX == desX && locY == desY)
		return GL_FALSE;

	mapData[locY][locX].state = Checked;

	int currentX = locX, currentY = locY;
	while(mapData[desY][desX].state != Checked) {
		for(int i = (currentY - 1); i <= (currentY + 1); i++) {
			for(int j = (currentX - 1); j <= (currentX + 1); j++) {
				if(i < 0 || i >= mapHeight || j < 0 || j >= mapWidth)
					continue;

				mapStruct *tempMap;
				switch(mapData[i][j].state) {
				case Unhandle:
					mapData[i][j].state = Checking;
					mapData[i][j].parent = &mapData[currentY][currentX];
					
					if(abs((i - currentY) + (j - currentX)) & 1)
						mapData[i][j].gValue = 10;
					else
						mapData[i][j].gValue = 14;

					mapData[i][j].hValue = (GLuint)(abs((int)(desX - i) * 10) + abs((int)(desY - j) * 10));
					mapData[i][j].fValue = mapData[i][j].gValue + mapData[i][j].hValue;
					break;

				case Collision:
					break;

				case Checking:
					if(abs((i - currentX) + (j - currentY)) & 1)
						if(mapData[i][j].gValue > mapData[currentY][currentX].gValue + 10) {
							mapData[i][j].gValue = mapData[currentY][currentX].gValue + 10;
							mapData[i][j].parent = &mapData[currentY][currentX];
						}
					else
						if(mapData[i][j].gValue > mapData[currentY][currentX].gValue + 14) {
							mapData[i][j].gValue = mapData[currentY][currentX].gValue + 14;
							mapData[i][j].parent = &mapData[currentY][currentX];
						}
					break;

				case Checked:
					break;
				}
			}
		}

		for(int i = (currentY - 1); i <= (currentY + 1); i++) {
			for(int j = (currentX - 1); j <= (currentX + 1); j++) {
				if(i <= 0 || i >= mapHeight - 1 || j <= 0 || j >= mapWidth - 1)
					break;
				if(mapData[i][j].state == Checked || mapData[i][j].state == Checking)
					continue;
				if((i - currentY) == 0 && mapData[i][j].state == Collision) {
					if(mapData[i - 1][j].state == Checking)
						mapData[i - 1][j].state = Unhandle;
					if(mapData[i + 1][j].state == Checking)
						mapData[i + 1][j].state = Unhandle;
				}
				if((j - currentX) == 0 && mapData[i][j].state == Collision) {
					if(mapData[i][j - 1].state == Checking)
						mapData[i][j - 1].state = Unhandle;
					if(mapData[i][j + 1].state == Checking)
						mapData[i][j + 1].state = Unhandle;
				}
			}
		}

		//Find the best path by the minimum value of F value
		mapStruct minStep = { MAXDWORD32, 0, 0, 0, 0, 0, nullptr, nullptr };
		for(int i = 0; i < mapHeight; i++) {
			for(int j = 0; j < mapWidth; j++) {
				if(mapData[i][j].state == Checking) {
					if(mapData[i][j].fValue < minStep.fValue)
						minStep = mapData[i][j];
				}
				else
					continue;
			}
		}

		if(mapData[minStep.locY][minStep.locX].state != Collision)
			mapData[minStep.locY][minStep.locX].state = Checked;

		if(minStep.fValue == MAXDWORD32) {
			return GL_FALSE;
		}
		else {
			currentX = minStep.locX;
			currentY = minStep.locY;
		}
	}

	mapStruct *temp = &mapData[desY][desX];	//Construct the best path
	temp->state = Path;
	temp->next = nullptr;
	while(temp != &mapData[locY][locX]) {
		temp->parent->next = temp;
		temp->parent->state = Path;
		temp = temp->parent;
	}
	location = &mapData[locY][locX];

	for(int i = 0; i < mapHeight; i++)	//Reset the scene data
		for(int j = 0; j < mapWidth; j++)
			if(mapData[i][j].state != Collision)
				mapData[i][j].state = Unhandle;

	return GL_TRUE;
}


//Get the next destination
CVector CPathFinder::GetNextLoc(void)
{
	if(location->next != nullptr)
		location = location->next;
	return CVector(location->locX, 0, location->locY);
}
