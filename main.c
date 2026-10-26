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

	int weight;
	int capacity;
} object;

#define player (objects + 0)
#define cave   (objects + 1)
#define field  (objects + 2)
#define coin   (objects + 3)

#define obj_end   (objects + 4)

object objects[] = {
	// PLAYER
	{"player", "the player", field, NULL, NULL, NULL, NULL, 100, 20},
	// MAP
	{"cave", "a small cavern", NULL, NULL, field, NULL, NULL, 9999, 9999},
	{"field", "an open field", NULL, cave, NULL, NULL, NULL, 9999, 9999},
	// ITEMS
	{"coin", "a golden coin", cave, NULL, NULL, NULL, NULL, 1, 0},
};

bool check_loc(const char *noun, object *obj, object *target_loc) {
	return strcmp(noun, obj->name) == 0 && obj->loc == target_loc;
}

void list_objects(object *loc) {
	object *obj;
	printf("You see:\n");
	for (obj = objects; obj < obj_end; obj++) {
		if (obj->loc == loc && obj != player)
			printf("%s\n", obj->desc);
	}
}

void look_around(object *loc) {
	printf("You are in %s\n", loc->desc);
	list_objects(loc);
}

void grab_object(object *obj) {
	printf("You picked up %s\n", obj->desc);
	obj->loc = player;
}

void drop_object(object *obj) {
	printf("You dropped %s\n", obj->desc);
	obj->loc = player->loc;
}
// }}}

static bool is_playing = true;
static char input[100];

// ACTIONS {{{
void exec_look(const char *noun) {
	if(strcmp(noun, "around") == 0)
		look_around(player->loc);
	else { printf("I do not understand what you wish to see.\n"); }
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
// TODO : Move item check to new function
void exec_grab(const char *noun) {
	object *obj;
	for (obj = objects; obj < obj_end; obj++)
		if(check_loc(noun, obj, player->loc)) {
			grab_object(obj);
			return;
		}
	printf("There is no %s here.\n", noun);
}

void exec_drop(const char *noun) {
	object *obj;
	for(obj = objects; obj < obj_end; obj++)
		if(check_loc(noun, obj, player)) {
			drop_object(obj);
			return;
		}
	printf("You have no %s to drop.\n", obj->name);
}
// }}}

// INPUT {{
static void handle_input(char *input) {
	char *verb = strtok(input, " \n");
	char *noun = strtok(NULL, "\n");

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
	look_around(player->loc);
	while(is_playing) { game_loop(); }
	return 0;
}
// }}}
