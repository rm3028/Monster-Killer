#include "stdafx.h"
#include "MonsterKiller.h"


namespace MonsterKiller {

	CMonsterKiller* CMonsterKiller::instance = nullptr;	//Pointer to the Homework4 object
	GLfloat CMonsterKiller::LightPos[] = { -400.0, 700.0, 500.0, 0.0 };	//Position of Light0


	//Construct the game object
	CMonsterKiller::CMonsterKiller(GLvoid)
	{
	}


	//Construct the glut OpenGL environment window
	CMonsterKiller::CMonsterKiller(int argc, char* argv[])
	{
		OnCreate(argc, argv);
	}


	//Deconstruct the game object
	CMonsterKiller::~CMonsterKiller(GLvoid)
	{
		exitWindow = GL_TRUE;

		for(int i = 0; i < MonsterCount; i++)
			monState[i].join();

		delete[] monState;

		delete master;
		delete[] monster;
		monster = nullptr;
		master = nullptr;

		delete camera, sceneMap;
		camera = nullptr;
		sceneMap = nullptr;
	}


	//Create the glut OpenGL environment window
	GLvoid CMonsterKiller::OnCreate(int argc, char* argv[])
	{
		instance = this;
	
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(800, 600);
		glutInitWindowPosition(100, 100);

		glutCreateWindow("Monster Killer");

		sceneMap = new CSceneMap();	//Scene object declaration
		sceneMap->LoadAerialMap();

		aerialMap = GL_TRUE;	//Aerial map on
		lifeBar = GL_TRUE;	//Life bar of monster on
		exitWindow = GL_FALSE;

		master = new CMaster(0, 50, 900);	//Character object declaration
		master->SetSpeed(5.0);
		master->SetScene(sceneMap);
		master->SetAttackPower(LeftArm, 20.0);
		master->SetAttackPower(RightArm, 20.0);

		monster = new CMonster[MonsterCount];	//Monster object array declaration
		monState = new thread[MonsterCount];
		srand(time(NULL));
		for(int i = 0; i < MonsterCount; i++) {
			static float tempLocX, tempLocZ;
			monster[i].SetScene(sceneMap);
			do {
				tempLocX = rand() % SCENEWIDTH - SCENEOFFSET;
				tempLocZ = rand() % SCENEWIDTH - SCENEOFFSET;
			}while(sceneMap->CollisionTest(tempLocX, 40.0, tempLocZ));
			monster[i].SetLocation(tempLocX, 40.0, tempLocZ);	//Locate monster by random
			monster[i].SetAttackPower(1.0);	//Set the power of monster
			monState[i] = thread(SetMonsterTimer, i);	//Run the monster by thread for each monster
		}

		camera = new CCamera();	//Camera object declaration
		camera->SetCamera(master->GetLocation()->x, master->GetLocation()->y, master->GetLocation()->z, 0.0, master->GetLocation()->y, 0.0, 0.0, 1.0, 0.0);
		camera->SetFollow(master->GetLocation());
		
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_DEPTH_TEST);

		glutCloseFunc(SetClose);
		glutTimerFunc(16, SetTimer, 0);
		glutReshapeFunc(SetReshape);
		glutDisplayFunc(SetDisplay);
		glutMouseFunc(SetMouseFunc);
		glutKeyboardFunc(SetKeyboardFunc);
		glutKeyboardUpFunc(SetKeyboardUpFunc);
		glutMainLoop();
	}


	//OnClose event
	GLvoid CMonsterKiller::OnClose(GLvoid)
	{
		this->~CMonsterKiller();
	}


	//OnTimer event
	GLvoid CMonsterKiller::OnTimer(int id)
	{
		switch(id) {
		case 0:
			for(int i = 0; i < MonsterCount; i++) {
				//Recognition of character attack monster
				if(monster[i].GetState() < MonDeathing) {
					if(Distance(*master->GetLocation(), *monster[i].GetLocation()) < 200 && Angle(camera->GetCenter() - camera->GetEye(), *monster[i].GetLocation() - camera->GetEye()) < 90.0) {
						for(int j = 0; j < AttackAnm; j++) {
							switch(j) {
							case LeftArm:	//Attack by left arm
								if(master->GetAttackState(LeftArm))
									monster[i].OnAttacked(master->GetAttackPower(LeftArm));
								break;

							case RightArm:	//Attack by right arm
								if(master->GetAttackState(RightArm))
									monster[i].OnAttacked(master->GetAttackPower(RightArm));
								break;
								break;
							}
						}
					}
				}
			}
			glutPostRedisplay();
			
			static int lastUpdate = 0;	//FPS printer
			static int frames = 0;
			char title[52];

			int currentTime = glutGet(GLUT_ELAPSED_TIME);
			frames++;

			if ((currentTime - lastUpdate) >= 1000 ) {
				sprintf_s(title, "Monster Killer   FPS: %d", frames);
				glutSetWindowTitle(title);
				frames = 0;
				lastUpdate = currentTime;
			}

			glutTimerFunc(16, SetTimer, 0);
			break;
		}
	}


	//OnDisplay event
	GLvoid CMonsterKiller::OnDisplay(GLvoid)
	{
		glMatrixMode(GL_MODELVIEW);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.345, 0.5, 0.745, 0.0);

		glPushMatrix();	//Locate Light0
			glLightfv(GL_LIGHT0, GL_POSITION, LightPos); 
		glPopMatrix();

		sceneMap->ShowMap();	//Render scene

		for(int i = 0; i < MonsterCount; i++) {	//Render monster
			monster[i].OnDisplay();
			if(lifeBar)
				monster[i].OnDisplayLife(master->GetLocAngle());	//Render monster's life bar
		}

		master->OnTimer();	//Character movement

		camera->TurnSet(2.5);	//Camera rotation
		camera->LookAt();
		
		master->SetLocAngle(camera->GetAngleY());	//Rotate character
		master->OnDisplay();	//Render character
		master->OnDisplayLife();	//Render character's life

		for(int i = 0; i < MonsterCount; i++) { //Victory recognition
			if(monster[i].GetState() != MonDeath)
				break;
			if(i == MonsterCount - 1)
				master->OnVictory();
		}

		if(aerialMap) {
			glMatrixMode(GL_PROJECTION);	//Orthographic projection to render the aerial map
			glPushMatrix();
				glLoadIdentity();
				if(glutGet(GLUT_WINDOW_WIDTH) >= glutGet(GLUT_WINDOW_HEIGHT))
					glOrtho((-600 * 5) + 300, 300, -300.0, (600 * 5) * glutGet(GLUT_WINDOW_HEIGHT) / glutGet(GLUT_WINDOW_WIDTH) - 300, -300.0, 300);
				else
					glOrtho(-600 * 5 * glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT) + 300, 300, -300.0, (600 * 5) - 300, -300.0, 300);
		
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
					glLoadIdentity();	//Render the aerial map
					sceneMap->ShowAerialMap();
					for(int i = 0; i < MonsterCount; i++) {
						if(monster[i].GetState() != MonDeath)
							sceneMap->SetMonAerialLoc(*monster[i].GetLocation(), monster[i].GetLocAngle());	//Render monster's location on aerial map
					}
					sceneMap->SetMasAerialLoc(*master->GetLocation(), master->GetLocAngle());	//Render character's location on aerial map
				glPopMatrix();
				glMatrixMode(GL_PROJECTION);
			glPopMatrix();
		}

		glutSwapBuffers();
	}


	//OnReshap event
	GLvoid CMonsterKiller::OnReshape(int width, int height)
	{
		glViewport(0, 0, width, height);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0, (GLfloat) width/(GLfloat) height, 1, 10000.0);
	}


	//OnMouse event
	GLvoid CMonsterKiller::OnMouseFunc(int button, int state, int x, int y) 
	{
		switch(button) {
		case GLUT_LEFT_BUTTON:
			switch(state) {
			case GLUT_DOWN:
				camera->TurnLeft(GL_TRUE);
				break;
			case GLUT_UP:
				camera->TurnLeft(GL_FALSE);
				break;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			switch(state) {
			case GLUT_DOWN:
				camera->TurnRight(GL_TRUE);
				break;
			case GLUT_UP:
				camera->TurnRight(GL_FALSE);
				break;
			}
			break;
		}
	}


	//OnKeyboard event
	GLvoid CMonsterKiller::OnKeyboardFunc(unsigned char key, int x, int y) 
	{
		switch(key) {
		case 'w':
		case 'W':
			master->MoveFront(GL_TRUE);
			break;

		case 's':
		case 'S':
			master->MoveBack(GL_TRUE);
			break;

		case 'a':
		case 'A':
			master->MoveLeft(GL_TRUE);
			break;

		case 'd':
		case 'D':
			master->MoveRight(GL_TRUE);
			break;

		case ' ':
			master->MoveJump(GL_TRUE);
			break;

		case 'l':
		case 'L':	//Switch the spotlight
			master->EnableSpotLight(!master->GetSpotState());
			break;

		case 'n':
		case 'N':	//Rebirth monsters
			for(int i = 0; i < MonsterCount; i++) {
				if(monster[i].GetState() == MonDeath) {
					static float tempLocX, tempLocZ;
					do {
						tempLocX = rand() % SCENEWIDTH - SCENEOFFSET;
						tempLocZ = rand() % SCENEWIDTH - SCENEOFFSET;
					}while(sceneMap->CollisionTest(tempLocX, 40.0, tempLocZ));
					monster[i].SetLocation(tempLocX, 40.0, tempLocZ);	//Locate monster by random
					monster[i].OnRebirth();
				}
			}
			break;

		case 'm':
		case 'M':	//Switch aerial map
			aerialMap = !aerialMap;
			break;

		case 'b':
		case 'B':	//Switch life bar of monsters
			lifeBar = !lifeBar;
			break;

		case '1':	//Attack by left arm
			master->OnAttack(LeftArm);
			break;

		case '2':	//Attack by right arm
			master->OnAttack(RightArm);
			break;

		case '0':	//Rebirth the character
			master->OnRebirth();
			break;
		}
	}


	//OnKeyboardUp event
	GLvoid CMonsterKiller::OnKeyboardUpFunc(unsigned char key, int x, int y) 
	{
		switch(key) {
		case 'w':
		case 'W':
			master->MoveFront(GL_FALSE);
			break;

		case 's':
		case 'S':
			master->MoveBack(GL_FALSE);
			break;

		case 'a':
		case 'A':
			master->MoveLeft(GL_FALSE);
			break;

		case 'd':
		case 'D':
			master->MoveRight(GL_FALSE);
			break;
		}
	}


	//Set close event
	GLvoid CMonsterKiller::SetClose(GLvoid)
	{
		instance->OnClose();
	}


	//Set timer event
	GLvoid CMonsterKiller::SetTimer(int id) 
	{
		instance->OnTimer(id);
	}


	//Set display event
	GLvoid CMonsterKiller::SetDisplay(GLvoid) 
	{
		instance->OnDisplay(); 
	}


	//Set reshape event
	GLvoid CMonsterKiller::SetReshape(int width, int height) 
	{
		instance->OnReshape(width, height);
	}


	//Set mouse event
	GLvoid CMonsterKiller::SetMouseFunc(int button, int state, int x, int y)
	{
		instance->OnMouseFunc(button, state, x, y);
	}


	//Set keyboard event
	GLvoid CMonsterKiller::SetKeyboardFunc(unsigned char key, int x, int y) 
	{
		instance->OnKeyboardFunc(key, x, y);
	}


	//Set keyboardUp event
	GLvoid CMonsterKiller::SetKeyboardUpFunc(unsigned char key, int x, int y) 
	{
		instance->OnKeyboardUpFunc(key, x, y);
	}


	//Thread function of monster
	GLvoid CMonsterKiller::MonsterTimer(GLuint id)
	{
		srand(time(NULL) + id);
		while(monster != nullptr && !exitWindow) { //If death or exit kill thread
			monster[id].StateLoop();	//AI of monster
			if(master != nullptr && monster != nullptr && monster[id].GetState() < MonDeathing) {
				if(Distance(*monster[id].GetLocation(), *master->GetLocation()) < 100)
					master->OnAttacked(monster[id].GetAttackPower());	//Monster attack character event
			}

			for(int i = id + 1; i < id + MonsterCount; i++) {	//Collision detection between monster to monster
				if(monster != nullptr && Distance(*monster[id].GetLocation(), *monster[i % MonsterCount].GetLocation()) < 100.0 && monster[id].GetState() == MonMove && monster[i % MonsterCount].GetState() == MonMove) {
					monster[id].OnCollision();
					monster[i % MonsterCount].OnCollision();
				}
			}
			Sleep(16);
		}
	}


	//Set the thread function of monster
	GLvoid CMonsterKiller::SetMonsterTimer(GLuint id)
	{
		instance->MonsterTimer(id);
	}

}
