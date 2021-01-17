/*
 * main.cpp : Defines the entry point for the console application.
 * Author: Wei-Yun
 * Email: rm3028@hotmail.com.tw
 * Copyright © 2021 Wei-Yun.
 * All rights reserved.
 */

#include "stdafx.h"
#include "MonsterKiller.h"
using namespace MonsterKiller;


int main(int argc, char* argv[])
{
	CMonsterKiller monsterKiller;
	monsterKiller.OnCreate(argc, argv);

	return 0;
}

