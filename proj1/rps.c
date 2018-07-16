/*******************************
 * Author: Michael Adams
 * Last Edit: 2/11/18
 ******************************/
#include <stdio.h>

typedef struct Player {
	int choice;
	int score;
}Player;

/*
 * compare choices and handle winner
 */
void compare(Player *user, Player *comp) {
	if ((*user).choice == 0 && (*comp).choice == 1) {
		printf("Computer chose paper. You lose this game.\n");
		(*comp).score++;
	}
	else if ((*user).choice == 0 && (*comp).choice == 2) {
		printf("Computer chose scissors. You win this game.\n");
		(*user).score++;
	}
	else if ((*user).choice == 1 && (*comp).choice == 0) {
		printf("Computer chose rock. You win this game.\n");
		(*user).score++;
	}
	else if ((*user).choice == 1 && (*comp).choice == 2) {
		printf("Computer chose scissors. You lose this game.\n");
		(*comp).score++;
	}
	else if ((*user).choice == 2 && (*comp).choice == 0) {
		printf("Computer chose rock. You lose this game.\n");
		(*comp).score++;
	}
	if ((*user).choice == 2 && (*comp).choice == 1) {
		printf("Computer chose paper. You win this game.\n");
		(*user).score++;
	}
}

int main()
{
	// seed the rng
	srand((unsigned int)time(NULL));

	// initialize vars
	int round_num = 1;
	char in[100];
	char input[5];
	
	// create players
	Player user;
	user.choice = 0;
	user.score = 0;
	Player comp;
	comp.choice = 0;
	comp.score = 0;

	printf("Welcome to rock, paper, scissors!\n\n");
	
	while(1) { // run until we return a value from main and end the program
		do {
			printf("Round: %d\nChoose rock, paper, or scissors: ", round_num);
			scanf("%s", in);

			// retry until user makes valid choice
			while(strcmp(in, "rock") && strcmp(in, "paper") && strcmp(in, "scissors")) { //ret 0 if equal, so anything else will !=0 (true)
				printf("Error: invalid choice\n");
				printf("Round: %d\nChoose this rock, paper, or scissors: ", round_num);
				scanf("%s", in);
			}

			// set user choice
			if (!strcmp(in, "rock")) {
				user.choice = 0;
			}
			else if (!strcmp(in, "paper")) {
				user.choice = 1;
			}
			else if (!strcmp(in, "scissors")) {
				user.choice = 2;
			}

			// get comp choice
			comp.choice = rand() % 3;

			// print message on draw
			if (user.choice == comp.choice)	{
				if (user.choice == 0) {
					printf("DRAW: both chose rock, replay round\n");
				}
				else if (user.choice == 1) {
					printf("DRAW: both chose paper, replay round\n");
				}
				else if (user.choice == 2) {
					printf("DRAW: both chose scissors, replay round\n");
				}
			}
		}while(user.choice == comp.choice);
		
		compare(&user, &comp);
		printf("Current Scores:\nuser: %d\ncomputer: %d\n\n", user.score, comp.score);
		
		
		if (user.score == 3) { //user won
			char input[5];
			printf("You win the match!\nWould you like to play again? (y/n): ");
			scanf("%s", input);
			if (!strcmp(input, "y")) {
				// reset round number
				round_num = 1;
				
				// reset players
				user.choice = 0;
				user.score = 0;
				comp.choice = 0;
				comp.score = 0;
			}
			else {
				return 0;
			}
		}
		else if (comp.score == 3) { //computer won
			char input[5];
			printf("You lose the match!\nWould you like to play again? (y/n): ");
			scanf("%s", input);
			if (!strcmp(input, "y")) {
				// reset round number
				round_num = 1;
				// reset players
				user.choice = 0;
				user.score = 0;
				comp.choice = 0;
				comp.score = 0;
			}
			else {
				return 0;
			}
		}
		else { //no winner yet
			round_num++;
		}	
	}
}