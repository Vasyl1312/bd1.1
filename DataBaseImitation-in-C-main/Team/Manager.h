#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "Player.h"

int IsExistFile(FILE* indexTable, FILE* database, char* err)
{
	if (indexTable == NULL ||
		database == NULL)
	{
		strcpy(err, "There are no notes in the database");
		return 0;
	}

	return 1;
}

int IsExistIndex(FILE* indexTable, int id, char* err)
{
	fseek(indexTable, 0, SEEK_END);

	long indexTableSize = ftell(indexTable);

	if (indexTableSize == 0 ||
		id * sizeof(struct Index) > indexTableSize)
	{
		strcpy(err, "There is no table index");
		return 0;
	}

	return 1;
}

int IsExistNote(struct Index index, char* err)
{
	if (index.isExist)
		return 1;

	strcpy(err, "There is no note you are looking for");
	return 0;
}

int IsExistUniqueKeyPair(struct Team team, int id)
{
	FILE* PlayerDatebase = fopen(PLAYER_DATA, "r+b");
	struct Player player;

	fseek(PlayerDatebase, team.firstPlayerAddress, SEEK_SET);

	for (int i = 0; i < team.playerNum; i++)
	{
		fread(&player, sizeof(struct Player), 1, PlayerDatebase);
		fclose(PlayerDatebase);

		if (player.id == id)
		{
			return 0;
		}

		PlayerDatebase = fopen(PLAYER_DATA, "r+b");
		fseek(PlayerDatebase, player.nextPlayerAddress, SEEK_SET);
	}

	fclose(PlayerDatebase);

	return 1;
}

void ShowInfo()
{
	FILE* indexTable = fopen("team.ind", "rb");

	if (indexTable == NULL)
	{
		printf("Error: there are no created files in the database\n");
		return;
	}

	int teamCnt = 0;
	int playerCnt = 0;

	fseek(indexTable, 0, SEEK_END);
	int indAmount = ftell(indexTable) / sizeof(struct Index);

	struct Team team;

	char err[51];

	for (int i = 1; i <= indAmount; i++)
	{
		if (TryGetTeamData(&team, i, err))
		{
			teamCnt++;
			playerCnt += team.playerNum;

			printf("Team %d, consists of %d players\n", i, team.playerNum);
		}
	}

	fclose(indexTable);

	printf("Amount of teams: %d\n", teamCnt);
	printf("Amount of players: %d\n", playerCnt);
}