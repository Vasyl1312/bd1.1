#pragma once

struct Index
{
	int id;
	int address;
	int isExist;
};

struct Team
{
	int id;
	char name[16];
	int wins;
	long firstPlayerAddress;
	int playerNum;
};

struct Player
{
	int teamId;
	int id;
	char surname[16];
	char role[16];
	int isExist;
	long thisAddress;
	long nextPlayerAddress;
};