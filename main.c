// NAME :  taylormci
// DATE :  2026-10-25
// PURP :  main.c -> Bagtown Main File

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// OBJECTS {{{
typedef struct obj {
	const char *name;
	const char *desc;

	struct obj *loc;
	struct obj *north;
	struct obj *south;
	struct obj *east;
	struct obj *west;
} obj;

#define player (objects + 0)
#define cave   (objects + 1)
#define field  (objects + 2)
#define coin   (objects + 3)

obj objects[] = {
	{"player", "the player", field, NULL, NULL, NULL, NULL},
	
	{"cave", "a small cavern", NULL, field, NULL, NULL, NULL},
	{"field", "an empty field", NULL, NULL, cave, NULL, NULL},
	{"coin", "a golden coin", cave, NULL, NULL, NULL, NULL},
	
};
// }}}

static bool is_playing = true;
static char input[100];

// IO {{
static void handle_input(char *input) {
	char *verb = strtok(input, " \n");
	char *noun = strtok(NULL, " \n");

	if (verb == NULL) { return; }		 // return if null
	else if (strcmp(verb, "quit") == 0)	 // quit -> quits game
		is_playing = false; 
}
// }}}

// MAIN && GAME {{{
static void game_loop() {
	printf("\n--->");
	fgets(input, sizeof input, stdin);
	handle_input(input);
}

int main(int argc, char *argv[]) {
	while(is_playing) { game_loop(); }
	return 0;
}
// }}}
