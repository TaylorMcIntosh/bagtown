// NAME :  taylormci
// DATE :  2026-10-25
// PURP :  main.c -> Bagtown Main File

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_WORDS 8

static bool is_playing = true;
static char input[100];

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
#define man    (objects + 4)

#define NUM_OBJ   (objects + 5)

object objects[] = {
	// PLAYER
	{"player", "player", field, NULL, NULL, NULL, NULL, 100, 20},
	// MAP
	{"cave", "A small cavern", NULL, NULL, field, NULL, NULL, 9999, 9999},
	{"field", "An open field", NULL, cave, NULL, NULL, NULL, 9999, 9999},
	// ITEMS
	{"coin", "A golden coin", cave, NULL, NULL, NULL, NULL, 1, 0},
	// PEOPLE
	{"man", "A grumpy old man", field, NULL, NULL, NULL, NULL, 90, 20},
};


bool check_loc(const char *noun, object *obj, object *target_loc) {
	return strcmp(noun, obj->name) == 0 && obj->loc == target_loc;
}

void list_objects(object *loc) {
	object *obj;
	printf("You see:\n");
	for (obj = objects; obj < NUM_OBJ; obj++) {
		if (obj->loc == loc && obj != player)
			printf("%s\n", obj->desc);
	}
}

void look_around(object *loc) {
	printf("You are in %s\n", loc->desc);
	list_objects(loc);
}

void grab_object(object *obj) {
	if(obj->weight > player->capacity) {
		printf("%s is too heavy for you to pick up.\n", obj->desc);
		return;
	}
	obj->loc = player;
	player->capacity -= obj->weight;
	printf("You picked up %s.\n", obj->desc);
}

void drop_object(object *obj) {
	obj->loc = player->loc;
	player->capacity += obj->weight;
	printf("You dropped %s.\n", obj->desc);
}
// }}}

// COMMANDS {{{
void exec_quit(const char *noun, const char *target) { 
	is_playing = false; 
}

void exec_inventory(const char *noun, const char *target) { 
	list_objects(player); 
}

void exec_look(const char *noun, const char *target) {
	if(strcmp(noun, "around") == 0) look_around(player->loc);
	else printf("I do not understand what you wish to see.\n");
}

void exec_move(const char *noun, const char *target) {
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
	exec_look("around", "");
}

void exec_grab(const char *noun, const char *target) {
	object *obj;
	for (obj = objects; obj < NUM_OBJ; obj++)
		if(check_loc(noun, obj, player->loc)) {
			grab_object(obj);
			return;
		}
	printf("There is no %s here.\n", noun);
}

void exec_drop(const char *noun, const char *target) {
	object *obj;
	for(obj = objects; obj < NUM_OBJ; obj++)
		if(check_loc(noun, obj, player)) {
			drop_object(obj);
			return;
		}
	printf("You have no %s to drop.\n", noun);
}

typedef void (*command_func)(const char *noun, const char *target);

typedef struct {
	const char *name;
	command_func func;
} command;

static const command commands[] = {
	{"quit"		, exec_quit},
	{"look"		, exec_look},
	{"move"		, exec_move},
	{"grab"		, exec_grab},
	{"drop"		, exec_drop},
	{"inventory"	, exec_inventory},
};

#define NUM_CMD (sizeof(commands) / sizeof(commands[0]))
// }}}

// INPUT {{{
bool is_filler(const char *token) {
	return  strcmp(token, "i") == 0 ||
		strcmp(token, "the") == 0 ||
	       	strcmp(token, "at") == 0 ||
		strcmp(token, "in") == 0 ||
		strcmp(token, "on") == 0 ||
		strcmp(token, "into") == 0 ||
		strcmp(token, "onto") == 0 ||
		strcmp(token, "a") == 0; 
}

static void handle_input(char *input) {
	
	char *words[MAX_WORDS] = {NULL};
	int word_count = 0;
	
	char *token = strtok(input, " \n");
	while(token != NULL) {
		if (!is_filler(token)) words[word_count++] = token;
		token = strtok(NULL, " \n");
	}

	for(size_t i = 0; i < NUM_CMD; i++) {
		if(strcmp(words[0], commands[i].name) == 0) {
			commands[i].func(words[1], words[2]);
			return;
		}
	}
	printf("I do not understand how to '%s'.\n", words[0]);
}
// }}}

// MAIN && GAME {{{
static void game_loop() {
	printf("\n---> ");
	fgets(input, sizeof input, stdin);
	handle_input(input);
}

int main(int argc, char *argv[]) {
	look_around(player->loc);
	while(is_playing) { game_loop(); }
	return 0;
}
// }}}
