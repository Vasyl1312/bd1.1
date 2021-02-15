#pragma once

void OpenDatabase(FILE* database)
{
	fclose(database);
	database = fopen(PLAYER_DATA, "r+b");
}

void ConnectData(FILE* database, struct Team team, struct Player player)
{
	OpenDatabase(database);

	struct Player previous;

	fseek(database, team.firstPlayerAddress, SEEK_SET);

	for (int i = 0; i < team.playerNum; i++)
	{
		fread(&previous, sizeof(struct Player), 1, database);
		fseek(database, previous.nextPlayerAddress, SEEK_SET);
	}

	previous.nextPlayerAddress = player.thisAddress;
	fwrite(&previous, sizeof(struct Player), 1, database);
}

void ReConnectData(FILE* database, struct Player previous, struct Player player, struct Team* team)
{
	if (player.thisAddress == team->firstPlayerAddress)
	{
		if (player.thisAddress == player.nextPlayerAddress)
		{
			team->firstPlayerAddress = -1;
		}
		else
		{
			team->firstPlayerAddress = player.nextPlayerAddress;
		}
	}
	else
	{
		if (player.thisAddress == player.nextPlayerAddress)
		{
			previous.nextPlayerAddress = previous.thisAddress;
		}
		else
		{
			previous.nextPlayerAddress = player.nextPlayerAddress;
		}

		fseek(database, previous.thisAddress, SEEK_SET);
		fwrite(&previous, sizeof(struct Player), 1, database);
	}
}