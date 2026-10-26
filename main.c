// NAME :  taylormci
// DATE :  2026-10-25
// PURP :  main.c -> Bagtown Main File

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// OBJECTS {{{
typedef struct object {
	const char *name;
	const char *desc;

	struct object *loc;
	struct object *north;
	struct object *south;
	struct object *east;
	struct object *west;
} object;

#define player (objects + 0)
#define cave   (objects + 1)
#define field  (objects + 2)
#define coin   (objects + 3)

#define obj_end   (objects + 4)

object objects[] = {
	// PLAYER
	{"player", "the player", field, NULL, NULL, NULL, NULL},
	// MAP
	{"cave", "a small cavern", NULL, NULL, field, NULL, NULL},
	{"field", "an empty field", NULL, cave, NULL, NULL, NULL},
	// ITEMS
	{"coin", "a golden coin", cave, NULL, NULL, NULL, NULL},
};

void list_objects(object *loc) {
	object *obj;
	printf("You see:");
	for (obj = objects; obj < obj_end; obj++) {
		if (obj->loc == loc && obj != player)
			printf("\n%s", obj->desc);
	}
}

void grab_object(object *obj) {
	printf("You picked up ");
	printf("%s", obj->desc);
	obj->loc = player;
}

void drop_object(object *obj) {
	printf("You dropped ");
	printf("%s", obj->desc);
	obj->loc = player->loc;
}
// }}}

static bool is_playing = true;
static char input[100];

// ACTIONS {{{
void exec_look(const char *noun) {
	if(strcmp(noun, "around") == 0)
		printf("You are in %s\n", player->loc->desc);
	else { printf("I do not understand what you wish to see."); }
	list_objects(player->loc);
}

void exec_move(const char *noun) {
	if (strcmp(noun, "north") == 0 && player->loc->north != NULL)
		player->loc = player->loc->north;
	else if (strcmp(noun, "south") == 0 && player->loc->south != NULL)
		player->loc = player->loc->south;
	else if (strcmp(noun, "east") == 0 && player->loc->east != NULL)
		player->loc = player->loc->east;
	else if (strcmp(noun, "west") == 0 && player->loc->west != NULL)
		player->loc = player->loc->west;
	else {
		printf("There is nothing of note %s of here.\n", noun);
		return;
	}
	exec_look("around");
}

// TODO : Will need weight check
void exec_grab(const char *noun) {
	object *obj;
	for (obj = objects; obj < obj_end; obj++)
		if (strcmp(noun, obj->name) == 0 && player->loc == obj->loc) {
			grab_object(obj);	
			return;
		}
	printf("There is no %s here.\n", noun);
}

void exec_drop(const char *noun) {
	object *obj;
	for(obj = objects; obj < obj_end; obj++)
		if(strcmp(noun, obj->name) == 0 && obj->loc == player) {
			drop_object(obj);
			return;
		}
	printf("You have no %s to drop.\n", obj->name);
}
// }}}

// IO {{
static void handle_input(char *input) {
	char *verb = strtok(input, " \n");
	char *noun = strtok(NULL, " \n");

	if (verb == NULL) { return; }		 // return if null
	else if (strcmp(verb, "quit") == 0)	 // quit -> quits game
		is_playing = false; 
	else if (strcmp(verb, "look") == 0)	 // look -> looks at noun
		exec_look(noun);
	else if (strcmp(verb, "move") == 0)	 // move -> moves to noun
		exec_move(noun);
	else if (strcmp(verb, "grab") == 0)	 // grab -> pick up noun
		exec_grab(noun);
	else if (strcmp(verb, "drop") == 0)	 // drop -> drop noun
		exec_drop(noun);
	else if (strcmp(verb, "inventory") == 0) // inventory -> list items located at player
		list_objects(player);
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
