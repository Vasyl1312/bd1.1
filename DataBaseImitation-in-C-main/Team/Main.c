#include <stdio.h>

#include "Team.h"
#include "Player.h"
#include "DataStruct.h"

#define SEPARATOR "--------------------------------------\n"

int main()
{
	struct Team team;
	struct Player player;

	printf(SEPARATOR);
	printf("Database simulation | Hryts Vasyl K-25\n ");

	while (1)
	{
		int id;
		int command;
		char errMessage[60];

		printf(SEPARATOR);
		printf("commands:\n1. Show Information\n2. Insert Team\n3. Insert Player\n4. Delete Team\n5. Delete Player\n6. Update Team Data\n7. Update Player Data\n8. Get Team Info\n9. Get Player Info\n10.Exit\n");
		scanf("%d", &command);
		printf(SEPARATOR);

		switch (command)
		{
		case 1:
			ShowInfo();
			break;

		case 2:
			FillTeamData(&team);
			InsertTeamData(team);
			break;

		case 3:
			printf("Enter Team Id: ");
			scanf("%d", &id);

			if (TryGetTeamData(&team, id, errMessage))
			{
				player.teamId = id;
				printf("Enter new Player Id: ");
				scanf("%d", &id);

				player.id = id;
				FillPlayerData(&player);
				InsertPlayerData(team, player, errMessage);
				printf("Inserted is done\n");
			}
			else
				printf("Error: %s\n", errMessage);

			break;

		case 4:
			printf("Enter Team Id: ");
			scanf("%d", &id);
			if (DeleteTeamData(id, errMessage))
				printf("Deleted is done\n");
			else
				printf("Error: %s\n", errMessage);

			break;

		case 5:
			printf("Enter Team Id: ");
			scanf("%d", &id);

			if (TryGetTeamData(&team, id, errMessage))
			{
				printf("Enter Player Id: ");
				scanf("%d", &id);

				if (TryGetPlayerData(team, &player, id, errMessage))
				{
					DeletePlayerData(team, player, id, errMessage);
					printf("Deleted is done\n");
				}
				else
					printf("Error: %s\n", errMessage);
			}
			else
				printf("Error: %s\n", errMessage);

			break;

		case 6:
			printf("Enter Team Id: ");
			scanf("%d", &id);

			team.id = id;

			FillTeamData(&team);
			if (TryUpdateTeamData(team, errMessage))
				printf("Updated is done\n");
			else
				printf("Error: %s\n", errMessage);

			break;

		case 7:
			printf("Enter Team Id: ");
			scanf("%d", &id);

			if (TryGetTeamData(&team, id, errMessage))
			{
				printf("Enter Player Id: ");
				scanf("%d", &id);

				if (TryGetPlayerData(team, &player, id, errMessage))
				{
					FillPlayerData(&player);
					TryUpdatePlayerData(player, id, errMessage);
					printf("Updated is done\n");
				}
				else
					printf("Error: %s\n", errMessage);
			}
			else
				printf("Error: %s\n", errMessage);

			break;

		case 8:
			printf("Enter Team Id: ");
			scanf("%d", &id);
			if (TryGetTeamData(&team, id, errMessage))
				ShowTeamData(team);
			else
				printf("Error: %s\n", errMessage);

			break;

		case 9:
			printf("Enter Team Id: ");
			scanf("%d", &id);

			if (TryGetTeamData(&team, id, errMessage))
			{
				printf("Enter Player Id: ");
				scanf("%d", &id);
				TryGetPlayerData(team, &player, id, errMessage) ? ShowPlayerData(player, team) : printf("Error: %s\n", errMessage);
			}
			else
				printf("Error: %s\n", errMessage);

			break;

		case 10:
			return 0;

		default:
			printf("Error Command, try input again\n");
		}
	}

	return 0;
}