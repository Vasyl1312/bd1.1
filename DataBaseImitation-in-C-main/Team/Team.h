#pragma once
#pragma warning(disable:4996)

#define TEAM_IND "team.ind"
#define TEAM_DATA "team.fl"
#define TEAM_GARBAGE "team_garbage.txt"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Manager.h"
#include "Player.h"


void ShowTeamData(struct Team team)
{
	printf("Team name: %s\n", team.name);
	printf("Team wins: %d\n", team.wins);
}

void FillTeamData(struct Team* team)
{
	int wins;
	char name[16];

	name[0] = '\0';
	printf("Enter Team name: ");
	scanf("%s", name);
	strcpy(team->name, name);

	printf("Enter number of the team wins: ");
	scanf("%d", &wins);
	team->wins = wins;
}

void WriteDeletedTeamData(int id)
{
	FILE* garbageFile = fopen(TEAM_GARBAGE, "rb");	

	int garbageCnt;
	fscanf(garbageFile, "%d", &garbageCnt);

	int* deletedIds = malloc(garbageCnt * sizeof(int));

	for (int i = 0; i < garbageCnt; i++)
	{
		fscanf(garbageFile, "%d", deletedIds + i);				
	}

	fclose(garbageFile);									
	garbageFile = fopen(TEAM_GARBAGE, "wb");				
	fprintf(garbageFile, "%d", garbageCnt + 1);			

	for (int i = 0; i < garbageCnt; i++)
	{
		fprintf(garbageFile, " %d", deletedIds[i]);				
	}

	fprintf(garbageFile, " %d", id);						
	free(deletedIds);											
	fclose(garbageFile);									
}

void RewriteGarbageTeamId(int garbageCnt, FILE* garbageFile, struct Team* note)
{
	int* deletedIds = malloc(garbageCnt * sizeof(int));		

	for (int i = 0; i < garbageCnt; i++)
	{
		fscanf(garbageFile, "%d", deletedIds + i);				
	}

	note->id = deletedIds[0];									

	fclose(garbageFile);								
	fopen(TEAM_GARBAGE, "wb");							
	fprintf(garbageFile, "%d", garbageCnt - 1);			

	for (int i = 1; i < garbageCnt; i++)
	{
		fprintf(garbageFile, " %d", deletedIds[i]);				
	}

	free(deletedIds);											
	fclose(garbageFile);									
}

int InsertTeamData(struct Team note)
{
	struct Index index;
	int garbageCnt;
	FILE* indexTable = fopen(TEAM_IND, "a+b");			
	FILE* database = fopen(TEAM_DATA, "a+b");				
	FILE* garbageFile = fopen(TEAM_GARBAGE, "rb");		

	fscanf(garbageFile, "%d", &garbageCnt);

	if (garbageCnt)									
	{
		RewriteGarbageTeamId(garbageCnt, garbageFile, &note);

		fclose(indexTable);									
		fclose(database);									

		indexTable = fopen(TEAM_IND, "r+b");				
		database = fopen(TEAM_DATA, "r+b");				

		fseek(indexTable, (note.id - 1) * sizeof(struct Index), SEEK_SET);
		fread(&index, sizeof(struct Index), 1, indexTable);
		fseek(database, index.address, SEEK_SET);			
	}
	else                                                 
	{
		long indexerSize = sizeof(struct Index);

		fseek(indexTable, 0, SEEK_END);						

		if (ftell(indexTable))								
		{
			fseek(indexTable, -indexerSize, SEEK_END);		
			fread(&index, sizeof(struct Index), 1, indexTable);	

			note.id = index.id + 1;					
		}
		else                                              
		{
			note.id = 1;									
		}
	}

	note.firstPlayerAddress = -1;
	note.playerNum = 0;

	fwrite(&note, sizeof(struct Team), 1, database);				

	index.id = note.id;									
	index.address = (note.id - 1) * sizeof(struct Team);		
	index.isExist = 1;										

	printf("Your team\'s id is %d\n", note.id);

	fseek(indexTable, (note.id - 1) * sizeof(struct Index), SEEK_SET);
	fwrite(&index, sizeof(struct Index), 1, indexTable);			
	fclose(indexTable);										
	fclose(database);

	return 1;
}

int TryGetTeamData(struct Team* team, int id, char* err)
{
	FILE* indexTable = fopen(TEAM_IND, "rb");				
	FILE* database = fopen(TEAM_DATA, "rb");				
	
	if (!IsExistFile(indexTable, database, err))
	{
		return 0;
	}
	
	struct Index index;

	if (!IsExistIndex(indexTable, id, err))
	{
		return 0;
	}

	fseek(indexTable, (id - 1) * sizeof(struct Index), SEEK_SET);
	fread(&index, sizeof(struct Index), 1, indexTable);			

	if (!IsExistNote(index, err))
	{
		return 0;
	}

	fseek(database, index.address, SEEK_SET);				
	fread(team, sizeof(struct Team), 1, database);		
	fclose(indexTable);										
	fclose(database);

	return 1;
}

int TryUpdateTeamData(struct Team team, char* err)
{
	FILE* indexTable = fopen(TEAM_IND, "r+b");			
	FILE* database = fopen(TEAM_DATA, "r+b");				

	if (!IsExistFile(indexTable, database, err))
	{	
		return 0;
	}

	struct Index index;
	int id = team.id;

	if (!IsExistIndex(indexTable, id, err))
	{
		return 0;
	}

	fseek(indexTable, (id - 1) * sizeof(struct Index), SEEK_SET);	
	fread(&index, sizeof(struct Index), 1, indexTable);			

	if (!IsExistNote(index, err))
	{
		return 0;
	}

	fseek(database, index.address, SEEK_SET);				
	fwrite(&team, sizeof(struct Team), 1, database);				
	fclose(indexTable);										
	fclose(database);

	return 1;
}

int DeleteTeamData(int id, char* err)
{
	FILE* indexTable = fopen(TEAM_IND, "r+b");			
															
	if (indexTable == NULL)
	{
		strcpy(err, "-There are no database files");
		return 0;
	}

	if (!IsExistIndex(indexTable, id, err))
	{
		return 0;
	}

	struct Team team;
	TryGetTeamData(&team, id, err);

	struct Index index;

	fseek(indexTable, (id - 1) * sizeof(struct Index), SEEK_SET);	
	fread(&index, sizeof(struct Index), 1, indexTable);		

	index.isExist = 0;										

	fseek(indexTable, (id - 1) * sizeof(struct Index), SEEK_SET);

	fwrite(&index, sizeof(struct Index), 1, indexTable);			
	fclose(indexTable);										

	WriteDeletedTeamData(id);									

	
	if (team.playerNum)								
	{
		FILE* playerDatabase = fopen(PLAYER_DATA, "r+b");
		struct Player player;

		fseek(playerDatabase, team.firstPlayerAddress, SEEK_SET);

		for (int i = 0; i < team.playerNum; i++)
		{
			fread(&player, sizeof(struct Player), 1, playerDatabase);
			fclose(playerDatabase);
			DeletePlayerData(team, player, player.id, err);
			
			playerDatabase = fopen(PLAYER_DATA, "r+b");
			fseek(playerDatabase, player.nextPlayerAddress, SEEK_SET);
		}

		fclose(playerDatabase);
	}
	return 1;
}