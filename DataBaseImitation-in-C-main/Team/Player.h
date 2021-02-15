#pragma once
#pragma warning(disable:4996)

#define PLAYER_DATA "player.fl"
#define PLAYER_GARBAGE "player_garbage.txt"

#include <stdio.h>
#include <stdlib.h>

#include "DataStruct.h"
#include "DatabaseUse.h"
#include "Manager.h"
#include "Team.h"


void ShowPlayerData(struct Player player, struct Team team)
{
	printf("Team: %s, %d wins\n", team.name, team.wins);
	printf("Player surname: %s\n", player.surname);
	printf("Player role: %s\n", player.role);
}

void FillPlayerData(struct Player* player)
{
	char str[16];

	str[0] = '\0';
	printf("Enter player surname: ");
	scanf("%s", &str);
	strcpy(player->surname, str);

	str[0] = '\0';
	printf("Enter player role: ");
	scanf("%s", &str);
	strcpy(player->role, str);
}

void WriteDeletedPlayerData(long address)
{
	FILE* garbageFile = fopen(PLAYER_GARBAGE, "rb");

	int garbageCnt;
	fscanf(garbageFile, "%d", &garbageCnt);

	long* delAddresses = malloc(garbageCnt * sizeof(long));

	for (int i = 0; i < garbageCnt; i++)
	{
		fscanf(garbageFile, "%ld", delAddresses + i);
	}

	fclose(garbageFile);
	garbageFile = fopen(PLAYER_GARBAGE, "wb");
	fprintf(garbageFile, "%ld", garbageCnt + 1);

	for (int i = 0; i < garbageCnt; i++)
	{
		fprintf(garbageFile, " %ld", delAddresses[i]);
	}

	fprintf(garbageFile, " %d", address);
	free(delAddresses);
	fclose(garbageFile);
}

void RewriteGarbagePlayerId(int garbageCnt, FILE* garbageFile, struct Player* note)
{
	long* deletedIds = malloc(garbageCnt * sizeof(long));

	for (int i = 0; i < garbageCnt; i++)
	{
		fscanf(garbageFile, "%d", deletedIds + i);
	}

	note->thisAddress = deletedIds[0];
	note->nextPlayerAddress = deletedIds[0];

	fclose(garbageFile);
	fopen(PLAYER_GARBAGE, "wb");
	fprintf(garbageFile, "%d", garbageCnt - 1);

	for (int i = 1; i < garbageCnt; i++)
	{
		fprintf(garbageFile, " %d", deletedIds[i]);
	}

	free(deletedIds);
	fclose(garbageFile);
}

int InsertPlayerData(struct Team team, struct Player player, char* error)
{
	player.isExist = 1;

	FILE* database = fopen(PLAYER_DATA, "a+b");
	FILE* garbageZone = fopen(PLAYER_GARBAGE, "rb");

	int garbageCount;

	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount)
	{
		RewriteGarbagePlayerId(garbageCount, garbageZone, &player);
		OpenDatabase(database);
		fseek(database, player.thisAddress, SEEK_SET);
	}
	else
	{
		fseek(database, 0, SEEK_END);

		int dbSize = ftell(database);

		player.thisAddress = dbSize;
		player.nextPlayerAddress = dbSize;
	}

	fwrite(&player, sizeof(struct Player), 1, database);

	if (!team.playerNum)
	{
		team.firstPlayerAddress = player.thisAddress;
	}
	else
	{
		ConnectData(database, team, player);
	}

	fclose(database);

	team.playerNum++;
	TryUpdateTeamData(team, error);

	return 1;
}

int TryGetPlayerData(struct Team team, struct Player* player, int id, char* error)
{
	if (!team.playerNum)
	{
		strcpy(error, "There are no players in the team");
		return 0;
	}

	FILE* playerDatabase = fopen(PLAYER_DATA, "rb");


	fseek(playerDatabase, team.firstPlayerAddress, SEEK_SET);
	fread(player, sizeof(struct Player), 1, playerDatabase);

	for (int i = 0; i < team.playerNum; i++)
	{
		if (player->id == id)
		{
			fclose(playerDatabase);
			return 1;
		}

		fseek(playerDatabase, player->nextPlayerAddress, SEEK_SET);
		fread(player, sizeof(struct Player), 1, playerDatabase);
	}

	strcpy(error, "There are no players in playerDatabase");
	fclose(playerDatabase);
	return 0;
}


int TryUpdatePlayerData(struct Player player, int id)
{
	FILE* playerDatabase = fopen(PLAYER_DATA, "r+b");

	fseek(playerDatabase, player.thisAddress, SEEK_SET);
	fwrite(&player, sizeof(struct Player), 1, playerDatabase);
	fclose(playerDatabase);

	return 1;
}

int DeletePlayerData(struct Team team, struct Player player, int id, char* error)
{
	FILE* playerDatabase = fopen(PLAYER_DATA, "r+b");
	struct Player previous;

	fseek(playerDatabase, team.firstPlayerAddress, SEEK_SET);

	do
	{
		fread(&previous, sizeof(struct Player), 1, playerDatabase);
		fseek(playerDatabase, previous.nextPlayerAddress, SEEK_SET);
	} while (previous.nextPlayerAddress != player.thisAddress && player.thisAddress != team.firstPlayerAddress);

	ReConnectData(playerDatabase, previous, player, &team);
	WriteDeletedPlayerData(player.thisAddress);

	player.isExist = 0;

	fseek(playerDatabase, player.thisAddress, SEEK_SET);
	fwrite(&player, sizeof(struct Player), 1, playerDatabase);
	fclose(playerDatabase);

	team.playerNum--;
	TryUpdateTeamData(team, error);

}