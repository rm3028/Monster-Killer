#include "stdafx.h"
#include "Monster.h"


//Construct the monster
CMonster::CMonster(void) : locAngle(90.0), locAngleX(0.0), sceneData(nullptr), state(MonIdle), leftHand(0.0), rightHand(0.0), health(MAXHEALTH), step(GL_FALSE), moved(GL_FALSE), attackPower(0.0)
{
	location = new CVector();
	desLocation = new CVector();
	pathFinder = new CPathFinder();
}


//Construct the monster with location
CMonster::CMonster(GLfloat locationX, GLfloat locationY, GLfloat locationZ) : locAngle(90.0), locAngleX(0.0), sceneData(nullptr), state(MonIdle), leftHand(0.0), rightHand(0.0), health(MAXHEALTH), step(GL_FALSE), moved(GL_FALSE), attackPower(0.0)
{
	location = new CVector(locationX, locationY, locationZ);
	desLocation = new CVector();
	pathFinder = new CPathFinder();
}


//Deconstruct the monster
CMonster::~CMonster(void)
{
	delete location, desLocation, pathFinder;
}


//Rebirth event
GLvoid CMonster::OnRebirth(GLvoid)
{
	health = MAXHEALTH;
	state = MonIdle;
	step = GL_FALSE;
	moved = GL_FALSE;
}


//Collision event
GLvoid CMonster::OnCollision(GLvoid)
{
	if(state == MonMove && moved) {
		location->x -= oneLength.x;
		location->z -= oneLength.z;
		
		state = MonIdle;
		step = GL_FALSE;
		moved = GL_FALSE;
		health -= 1.0;
		if(health < 0.0) {
			health = 0.0;
			state = MonDeathing;
		}
	}
}


//Attacked event
GLvoid CMonster::OnAttacked(GLfloat damage)
{
	health -= damage;
	state = MonAttacked;
	if(health < 0.0) {
		health = 0.0;
		state = MonDeathing;
	}
}


//Render the monster by OpenGL function
GLvoid CMonster::OnDisplay(GLvoid)
{
	if(state != MonDeath) {
		leftHand += 5.0;
		rightHand += 5.0;

		if(leftHand == 360.0)
			leftHand = 0.0;
		if(rightHand == 360.0)
			rightHand = 0.0;

		if(health < MAXHEALTH && state < MonDeathing) {
			health += 0.1;
			if(health > MAXHEALTH)
				health = MAXHEALTH;
		}

		glPushMatrix();
			glTranslatef(location->x, location->y, location->z);

			glRotatef(locAngleX, 1.0, 0.0, 0.0);
			glRotatef(locAngle + 90.0, 0.0, 1.0, 0.0);
			static GLfloat bodyMtl[] = { 0.0, 1.0, 0.0, 1.0 };
			glMaterialfv(GL_FRONT, GL_DIFFUSE, bodyMtl);
			glutSolidSphere(30, 30, 30);	//Render body

			glPushMatrix();	//Render right hand
				glRotatef(-30, 0.0, 0.0, 1.0);
				glRotatef(leftHand, 0.0, 1.0, 0.0);
				glTranslatef(-35.0, 0.0, 0.0);
				static GLfloat rightHandMtl[] = { 0.0, 1.0, 1.0, 1.0 };
				glMaterialfv(GL_FRONT, GL_DIFFUSE, rightHandMtl);
				glutSolidSphere(10, 10, 10);
			glPopMatrix();

			glPushMatrix();	//Render left hand
				glRotatef(30, 0.0, 0.0, 1.0);
				glRotatef(leftHand, 0.0, 1.0, 0.0);
				glTranslatef(35.0, 0.0, 0.0);
				static GLfloat leftHandMtl[] = { 1.0, 1.0, 0.0, 1.0 };
				glMaterialfv(GL_FRONT, GL_DIFFUSE, leftHandMtl);
				glutSolidSphere(10, 10, 10);
			glPopMatrix();
		glPopMatrix();
	}

	return;
}


//Rendering the monster's life by OpenGL function
GLvoid CMonster::OnDisplayLife(GLfloat lookAngle)
{
	if(state != MonDeath) {
		glPushMatrix();
			glTranslatef(location->x, location->y, location->z);

			if(state < MonDeathing) {
				glPushAttrib(GL_ENABLE_BIT);
				glDisable(GL_LIGHTING);
				glPushMatrix();
					glRotatef(lookAngle + 90, 0.0, 1.0, 0.0);
					glTranslatef(0.0, 40.0, 0.0);
					glScalef(health / MAXHEALTH, 1.0, 1.0);
					glScalef(20.0, 1.0, 1.0);
					glColor3f(1.0, 0.0, 0.0);
					glutSolidCube(5.0);	//Render life point
					glColor3f(1.0, 1.0, 1.0);
				glPopMatrix();
				glPopAttrib();
			}
		glPopMatrix();
	}
}


//Monster AI
GLvoid CMonster::StateLoop(GLvoid)
{
	static CVector vVectorX(1.0, 0.0, 0.0);

	switch(state) {
	case MonIdle:
		do {
			desLocation->x = (GLfloat)(rand() % sceneData->GetMapWidth()) / sceneData->GetMapWidth() * SCENEWIDTH + ((GLfloat)SCENEWIDTH / sceneData->GetMapWidth() / 2) - SCENEOFFSET;
			desLocation->z = (GLfloat)(rand() % sceneData->GetMapHeight()) / sceneData->GetMapHeight() * SCENEWIDTH + ((GLfloat)SCENEWIDTH / sceneData->GetMapHeight() / 2) - SCENEOFFSET;
		}while(!pathFinder->FindPath((GLuint)((location->x + SCENEOFFSET) / SCENEWIDTH * (sceneData->GetMapWidth() - 1) + 0.5), (GLuint)((location->z + SCENEOFFSET) / SCENEWIDTH * (sceneData->GetMapHeight() - 1) + 0.5), 
			(GLuint)((desLocation->x + SCENEOFFSET) / SCENEWIDTH * (sceneData->GetMapWidth() - 1) + 0.5), (GLuint)((desLocation->z + SCENEOFFSET) / SCENEWIDTH * (sceneData->GetMapHeight() - 1) + 0.5)));
		state = MonMove;
		break;

	case MonMove:
		moved = GL_FALSE;

		if(abs(location->x - (nextLoc.x / sceneData->GetMapWidth() * SCENEWIDTH + ((GLfloat)SCENEWIDTH / sceneData->GetMapWidth() / 2) - SCENEOFFSET)) <= 1 && 
			abs(location->z - (nextLoc.z / sceneData->GetMapHeight() * SCENEWIDTH + ((GLfloat)SCENEWIDTH / sceneData->GetMapHeight() / 2) - SCENEOFFSET)) <= 1 || !step) {
			nextLoc = pathFinder->GetNextLoc();
			oneLength.x = ((nextLoc.x / sceneData->GetMapWidth() * SCENEWIDTH + ((GLfloat)SCENEWIDTH / sceneData->GetMapWidth() / 2) - SCENEOFFSET) - location->x) / 10;
			oneLength.z = ((nextLoc.z / sceneData->GetMapHeight() * SCENEWIDTH + ((GLfloat)SCENEWIDTH / sceneData->GetMapHeight() / 2) - SCENEOFFSET) - location->z) / 10;
			step = GL_TRUE;
		}

		if(oneLength.z < 0)
			locAngle = (acos(Dot(oneLength, vVectorX) / Magnitude(oneLength) / Magnitude(vVectorX)) * 180 / M_PI);
		else if(oneLength.z > 0)
			locAngle = -(acos(Dot(oneLength, vVectorX) / Magnitude(oneLength) / Magnitude(vVectorX)) * 180 / M_PI);
		else if(oneLength.x > 0)
			locAngle = 0.0;
		else if(oneLength.x < 0)
			locAngle = 180.0;

		location->x += oneLength.x;
		location->z += oneLength.z;

		moved = GL_TRUE;
		
		if(abs(location->x - desLocation->x) <= 1 && abs(location->z - desLocation->z) <= 1 && step) {
			state = MonIdle;
			step = GL_FALSE;
			moved = GL_FALSE;
		}
		break;

	case MonAttacked:
		locAngle += 10;
		if(locAngle > 360)
			state = MonIdle;
		break;
		
	case MonDeathing:	//Deathing animation
		locAngleX += 10.0;
		if(locAngleX >= 720) {
			state = MonDeath;
			locAngleX = 0.0;
		}
		break;

	case MonDeath:
		break;
	}
}
