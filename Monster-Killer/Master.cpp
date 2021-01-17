#include "stdafx.h"
#include <atlimage.h>
#include <glm/glm.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include "Master.h"


GLfloat CMaster::spotDif[] = { 1.0, 1.0, 1.0, 1.0 };	//Diffuse of spotlight
GLfloat CMaster::spotSpe[] = { 1.0, 1.0, 1.0, 1.0 };	//Specular of spotlight
GLfloat CMaster::spotDir[] = { 1.0, 0.0, 0.0 };	//Direction of spotlight


//Construct the character
CMaster::CMaster(GLvoid) : katanaLocZ(0.0), katanaSpeedZ(1.5), locAngle(90.0), m_front(GL_FALSE), m_back(GL_FALSE), m_left(GL_FALSE), m_right(GL_FALSE), m_jump(GL_FALSE), spotLight(GL_FALSE), sceneData(nullptr), health(MAXHEALTH), state(MasIdle)
{
	//Initialize spotlight
	glLightfv(GL_LIGHT1, GL_DIFFUSE, spotDif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spotSpe);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 15.0);
	//Initialize location
	location = new CVector();

	//Initialize the animation data
	for(int i = 0; i < AttackAnm; i++) {
		attackData[i].AttackAnimation = GL_FALSE;
		attackData[i].AttackAngle = 0.0;
		attackData[i].AttackSpeed = 7.5;
		attackData[i].AttackPower = 0.0;
		attackData[i].AttackTiming = GL_FALSE;
	}
	//Read model and texture to memory
	LoadObj(&m_katana, 1.0, GLM_MATERIAL, ".\\res\\Character\\katana.obj");
	LoadTexture(&lifeTex, ".\\res\\Character\\ico_health.png");
	LoadTexture(&deathTex, ".\\res\\Character\\gameover.png");
	LoadTexture(&winTex, ".\\res\\Character\\Victory_Games.png");
}


//Construct the character with location coordinate
CMaster::CMaster(GLfloat locationX, GLfloat locationY, GLfloat locationZ) : katanaLocZ(0.0), katanaSpeedZ(1.5), locAngle(90.0), m_front(GL_FALSE), m_back(GL_FALSE), m_left(GL_FALSE), m_right(GL_FALSE), m_jump(GL_FALSE), spotLight(GL_FALSE), sceneData(nullptr), health(MAXHEALTH), state(MasIdle)
{
	//Initialize spotlight
	glLightfv(GL_LIGHT1, GL_DIFFUSE, spotDif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spotSpe);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 30.0);
	//Initialize location
	location = new CVector(locationX, locationY, locationZ);

	//Initialize the animation data
	for(int i = 0; i < AttackAnm; i++) {
		attackData[i].AttackAnimation = GL_FALSE;
		attackData[i].AttackAngle = 0.0;
		attackData[i].AttackSpeed = 7.5;
		attackData[i].AttackPower = 30.0;
		attackData[i].AttackTiming = GL_FALSE;
	}
	//Read model and texture to memory
	LoadObj(&m_katana, 1.0, GLM_MATERIAL, ".\\res\\Character\\katana.obj");
	LoadTexture(&lifeTex, ".\\res\\Character\\ico_health.png");
	LoadTexture(&deathTex, ".\\res\\Character\\gameover.png");
	LoadTexture(&winTex, ".\\res\\Character\\Victory_Games.png");
}


//Deconstruct the character
CMaster::~CMaster(GLvoid)
{
	delete location;
}


GLboolean CMaster::LoadObj(GLuint *id, GLfloat scale, GLuint mode, char *path)
{
	GLMmodel *glm_model;

    glm_model = glmReadOBJ(path);
    glmScale(glm_model, scale);
    glmFacetNormals(glm_model);
    glmVertexNormals(glm_model, 90);
	*id = glmList(glm_model, mode | GLM_SMOOTH);

	glmDelete( glm_model );

	return GL_TRUE;
}


GLboolean CMaster::LoadTexture(GLuint *id, char *path)
{
	USES_CONVERSION;

	GLuint width, height;
	CImage tex;

	if(tex.Load(A2W(path)))
		return GL_FALSE;

	width = tex.GetWidth();
	height = tex.GetHeight();

	GLubyte *texImg = new BYTE[height * width * 4];

	if(tex.GetBPP() == 32)
	{
		for (int y = 0; y < height; y++) 
		{
			for (int x = 0; x < width; x++)
			{
				GLubyte *img = (GLubyte*)tex.GetPixelAddress(x, height - 1 - y);
				texImg[width * y * 4 + 4 * x + 0] = *(img + 2);
				texImg[width * y * 4 + 4 * x + 1] = *(img + 1);
				texImg[width * y * 4 + 4 * x + 2] = *(img + 0);
				texImg[width * y * 4 + 4 * x + 3] = *(img + 3);
			}
		}
	}
	else if(tex.GetBPP()==24)
	{
		for (int y = 0; y < height; y++) 
		{
			for (int x = 0; x < width; x++)
			{
				GLubyte *img = (GLubyte*)tex.GetPixelAddress(x, height - 1 - y);
				texImg[width * 4 * y + 4 * x + 0] = *(img + 2);
				texImg[width * 4 * y + 4 * x + 1] = *(img + 1);
				texImg[width * 4 * y + 4 * x + 2] = *(img + 0);
				texImg[width * 4 * y + 4 * x + 3] = 255;
			}
		}
	}

	glGenTextures(1, id);
    glBindTexture(GL_TEXTURE_2D, *id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texImg);

	delete[] texImg;

	return GL_TRUE;
}


GLvoid CMaster::OnRebirth(GLvoid)
{
	health = MAXHEALTH;
	state = MasIdle;
	//Initialize the animation data
	for(int i = 0; i < AttackAnm; i++) {
		attackData[i].AttackAnimation = GL_FALSE;
		attackData[i].AttackAngle = 0.0;
		attackData[i].AttackTiming = GL_FALSE;
	}
}


//Timer event
GLvoid CMaster::OnTimer(GLvoid)
{
	if(state != MasDeath) {
		Movement(m_speed);
		AttackAnimation();
	}
}


//Rendering the character by OpenGL function
GLvoid CMaster::OnDisplay(GLvoid)
{
	if(state != MasDeath) { //Show character model
		if(health < MAXHEALTH) {
			health += 0.01;
			if(health > MAXHEALTH)
				health = MAXHEALTH;
		}

		if(m_front || m_back || m_left || m_right) {
			katanaLocZ += katanaSpeedZ;
			if(katanaLocZ >= 12.0 || katanaLocZ <= -12.0)
				katanaSpeedZ = -katanaSpeedZ;
		}
		else
			katanaLocZ = 0.0;

		GLfloat spotPos[] = { location->x, location->y, location->z, 1.0 };
		glPushMatrix();
			glPushMatrix();	//Rotate spotlight
				glRotatef(locAngle, 0.0, 1.0, 0.0);
				glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);
			glPopMatrix();
			glPushMatrix();	//Move spotlight
				glLightfv(GL_LIGHT1, GL_POSITION, spotPos);
			glPopMatrix();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(location->x, location->y, location->z);
			glRotatef(locAngle + 90.0, 0.0, 1.0, 0.0);

			glPushMatrix();	//Left arm
				glTranslatef(0.0, 0.0, katanaLocZ);
				glTranslatef(30, -30.0, 0.0);

				glTranslatef(-100, 0.0, 0.0);
				glRotatef(attackData[LeftArm].AttackAngle, 0.0, 1.0, 0.0);
				glTranslatef(100, 0.0, 0.0);

				glCallList(m_katana);
			glPopMatrix();

			glPushMatrix();	//Right arm
				glTranslatef(0.0, 0.0, -katanaLocZ);
				glTranslatef(-30, -30.0, 0.0);

				glTranslatef(100, 0.0, 0.0);
				glRotatef(attackData[RightArm].AttackAngle, 0.0, 1.0, 0.0);
				glTranslatef(-100, 0.0, 0.0);

				glCallList(m_katana);
			glPopMatrix();
		glPopMatrix();
	}
	else { //If death show game over
		glMatrixMode(GL_MODELVIEW);
		glPushAttrib(GL_ENABLE_BIT);
	
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND); 

		glMatrixMode(GL_PROJECTION);
			glPushMatrix();
				glLoadIdentity();
					glOrtho(-300, 300, -300.0, 300, -1, 1);
		
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
					glLoadIdentity();

					glPushMatrix();
						glBindTexture(GL_TEXTURE_2D, deathTex);
						glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor3f(1.0, 1.0, 1.0);
						glBegin(GL_QUADS);
							glTexCoord2f(0.0, 0.0); glVertex3f(-200.0, -100.0, 0.0);
							glTexCoord2f(1.0, 0.0); glVertex3f(200.0, -100.0, 0.0);
							glTexCoord2f(1.0, 1.0); glVertex3f(200.0, 100.0, 0.0);
							glTexCoord2f(0.0, 1.0); glVertex3f(-200.0, 100.0, 0.0);
						glEnd();
					glPopMatrix();
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
			glPopMatrix();

		glPopAttrib();
	}

	return;
}


//Rendering the character's life by OpenGL function
GLvoid CMaster::OnDisplayLife(GLvoid)
{
	glMatrixMode(GL_MODELVIEW);
	glPushAttrib(GL_ENABLE_BIT);
	
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND); 

	glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glLoadIdentity();
				glOrtho(0, 120, 0.0, 120, -1, 1);
		
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
				glLoadIdentity();

				glPushMatrix();
					glBindTexture(GL_TEXTURE_2D, lifeTex);
					glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					glColor3f(1.0, 1.0, 1.0);
					glBegin(GL_QUADS);
						glTexCoord2f(0.0, 0.0); glVertex3f(4.0, 113.0, 0.0);
						glTexCoord2f(1.0, 0.0); glVertex3f(11.0, 113.0, 0.0);
						glTexCoord2f(1.0, 1.0); glVertex3f(11.0, 120.0, 0.0);
						glTexCoord2f(0.0, 1.0); glVertex3f(4.0, 120.0, 0.0);
					glEnd();
				glPopMatrix();

				glPushMatrix();
				glDisable(GL_TEXTURE_2D);
					glColor3f(0.0, 1.0, 0.0);
					glBegin(GL_QUADS);
						glVertex3f(15.0, 115.0, 0.0);
						glVertex3f(health + 15.0, 115.0, 0.0);
						glVertex3f(health + 15.0, 118.0, 0.0);
						glVertex3f(15.0, 118.0, 0.0);
					glEnd();
				glPopMatrix();
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
		glPopMatrix();

	glPopAttrib();
}


//Attacked event
GLvoid CMaster::OnAttack(GLuint id)
{
	if(state != MasDeath) {
		switch(id) {
		case LeftArm:
			attackData[LeftArm].AttackAnimation = GL_TRUE;
			break;

		case RightArm:
			attackData[RightArm].AttackAnimation = GL_TRUE;
			break;
		}
	}
}


//Attacked event
GLvoid CMaster::OnAttacked(GLfloat damage)
{
	health -= damage;
	if(health < 0.0) { //No life
		health = 0.0;
		for(int i = 0; i < AttackAnm; i++)
			attackData[i].AttackTiming = GL_FALSE;
		ResetMove();
		state = MasDeath;
	}
}


//Attack animation function
GLvoid CMaster::AttackAnimation(GLvoid)
{
	if(state != MasDeath) {
		if(attackData[LeftArm].AttackAnimation) {
			attackData[LeftArm].AttackAngle -= attackData[LeftArm].AttackSpeed;
			if(attackData[LeftArm].AttackAngle <= -180.0) {
				attackData[LeftArm].AttackSpeed = -attackData[LeftArm].AttackSpeed;
				attackData[LeftArm].AttackTiming = GL_TRUE;
			}
			else
				attackData[LeftArm].AttackTiming = GL_FALSE;
			if(attackData[LeftArm].AttackAngle >= 0.0) {
				attackData[LeftArm].AttackAngle = 0.0;
				attackData[LeftArm].AttackSpeed = -attackData[LeftArm].AttackSpeed;
				attackData[LeftArm].AttackAnimation = GL_FALSE;
			}
		}
		if(attackData[RightArm].AttackAnimation) {
			attackData[RightArm].AttackAngle += attackData[RightArm].AttackSpeed;
			if(attackData[RightArm].AttackAngle >= 180.0) {
				attackData[RightArm].AttackSpeed = -attackData[RightArm].AttackSpeed;
				attackData[RightArm].AttackTiming = GL_TRUE;
			}
			else
				attackData[RightArm].AttackTiming = GL_FALSE;
			if(attackData[RightArm].AttackAngle <= 0.0) {
				attackData[RightArm].AttackAngle = 0.0;
				attackData[RightArm].AttackSpeed = -attackData[RightArm].AttackSpeed;
				attackData[RightArm].AttackAnimation = GL_FALSE;
			}
		}
	}
}


//Rendering the victory texture by OpenGL function
GLvoid CMaster::OnVictory(GLvoid)
{
	glMatrixMode(GL_MODELVIEW);
		glPushAttrib(GL_ENABLE_BIT);
	
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND); 

		glMatrixMode(GL_PROJECTION);
			glPushMatrix();
				glLoadIdentity();
					glOrtho(-300, 300, -300.0, 300, -1, 1);
		
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
					glLoadIdentity();

					glPushMatrix();
						glBindTexture(GL_TEXTURE_2D, winTex);
						glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
						glColor3f(1.0, 1.0, 1.0);
						glBegin(GL_QUADS);
							glTexCoord2f(0.0, 0.0); glVertex3f(-200.0, -100.0, 0.0);
							glTexCoord2f(1.0, 0.0); glVertex3f(200.0, -100.0, 0.0);
							glTexCoord2f(1.0, 1.0); glVertex3f(200.0, 100.0, 0.0);
							glTexCoord2f(0.0, 1.0); glVertex3f(-200.0, 100.0, 0.0);
						glEnd();
					glPopMatrix();
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
			glPopMatrix();

		glPopAttrib();
}


//Handling movement event
GLvoid CMaster::Movement(GLfloat speed)
{
	if(state != MasDeath) {
		if(m_front) {//Move front event
			location->SetVector(location->x + (float)cos(M_PI * locAngle / 180.0) * speed, 
								location->y, 
								location->z - (float)sin(M_PI * locAngle / 180.0) * speed);
			if(sceneData != nullptr)	//Collision detection
			if(sceneData->CollisionTest(location->x, location->y, location->z)) {
				location->SetVector(location->x - (float)cos(M_PI * locAngle / 180.0) * speed, 
								location->y, 
								location->z + (float)sin(M_PI * locAngle / 180.0) * speed); 
			}
		}
		if(m_back) {//Move back event
			location->SetVector(location->x - (float)cos(M_PI * locAngle / 180.0) * speed, 
								location->y, 
								location->z + (float)sin(M_PI * locAngle / 180.0) * speed);
			if(sceneData != nullptr)	//Collision detection
			if(sceneData->CollisionTest(location->x, location->y, location->z)) {
				location->SetVector(location->x + (float)cos(M_PI * locAngle / 180.0) * speed, 
								location->y, 
								location->z - (float)sin(M_PI * locAngle / 180.0) * speed);
			}
		}
		if(m_left) {//Move left event
			location->SetVector(location->x - (float)sin(M_PI * locAngle / 180.0) * speed, 
								location->y, 
								location->z - (float)cos(M_PI * locAngle / 180.0) * speed);
			if(sceneData != nullptr)	//Collision detection
			if(sceneData->CollisionTest(location->x, location->y, location->z)) {
				location->SetVector(location->x + (float)sin(M_PI * locAngle / 180.0) * speed, 
								location->y, 
								location->z + (float)cos(M_PI * locAngle / 180.0) * speed);
			}
		}
		if(m_right) {//Move right event
			location->SetVector(location->x + (float)sin(M_PI * locAngle / 180.0) * speed, 
								location->y, 
								location->z + (float)cos(M_PI * locAngle / 180.0) * speed);
			if(sceneData != nullptr)	//Collision detection
			if(sceneData->CollisionTest(location->x, location->y, location->z)) {
				location->SetVector(location->x - (float)sin(M_PI * locAngle / 180.0) * speed, 
								location->y, 
								location->z - (float)cos(M_PI * locAngle / 180.0) * speed);
			}
		}
		if(m_jump) {//Jump event
			static GLuint jump = 0;
			static GLboolean jumpUp = GL_TRUE;
		
			if(jump == 0 && jumpUp == GL_FALSE) {
				location->y = location->y - 15 + jump;
				m_jump = GL_FALSE;
				jumpUp = GL_TRUE;
				jump = 0;
				return;
			}

			switch(jumpUp) {
			case GL_TRUE:
				location->y = location->y + 15 - jump;
				jump++;
				break;
			
			case GL_FALSE:
				location->y = location->y - 15 + jump;
				jump--;
				break;
			}
		
			if(jump == 15) {
				location->y = location->y + 15 - jump;
				jumpUp = GL_FALSE;
			}
		}
	}
	return;
}


//Switching the move front event
GLvoid CMaster::MoveFront(GLboolean m_enable)
{
	if(state != MasDeath) {
		switch(m_enable) {
		case GL_TRUE:
			m_front = GL_TRUE;
			break;
		case GL_FALSE:
			m_front = GL_FALSE;
			break;
		}
	}
	return;
}


//Switching the move back event
GLvoid CMaster::MoveBack(GLboolean m_enable)
{
	if(state != MasDeath) {
		switch(m_enable) {
		case GL_TRUE:
			m_back = GL_TRUE;
			break;
		case GL_FALSE:
			m_back = GL_FALSE;
			break;
		}
	}
	return;
}


//Switching the move left event
GLvoid CMaster::MoveLeft(GLboolean m_enable)
{
	if(state != MasDeath) {
		switch(m_enable) {
		case GL_TRUE:
			m_left = GL_TRUE;
			break;
		case GL_FALSE:
			m_left = GL_FALSE;
			break;
		}
	}
	return;
}


//Switching the move right event
GLvoid CMaster::MoveRight(GLboolean m_enable)
{
	if(state != MasDeath) {
		switch(m_enable) {
		case GL_TRUE:
			m_right = GL_TRUE;
			break;
		case GL_FALSE:
			m_right = GL_FALSE;
			break;
		}
	}
	return;
}


//Switching the jump event
GLvoid CMaster::MoveJump(GLboolean m_enable)
{
	if(state != MasDeath) {
		switch(m_enable) {
		case GL_TRUE:
			m_jump = GL_TRUE;
			break;
		case GL_FALSE:
			m_jump = GL_FALSE;
			break;
		}
	}
	return;
}


//Turn on or off the spotlight
GLvoid CMaster::EnableSpotLight(GLboolean m_enable)
{
	spotLight = m_enable;
	switch(spotLight) {
	case GL_TRUE:
		glEnable(GL_LIGHT1);
		break;

	case GL_FALSE:
		glDisable(GL_LIGHT1);
		break;
	}
}
