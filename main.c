// NAME :  taylormci
// DATE :  2026-10-25
// PURP :  main.c -> Bagtown Main File

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_WORDS 8

static bool is_playing = true;
static char input[100];

// TODO : contents [ preface list_objects ], fill [ fills a container with given object ],
//	  empty [ empties a container into location ], talk [ talk to sentient object (health>0 || animated) ],
//	  give [ gives object to sentient object ]

// OBJECTS {{{
typedef struct object {
	const char *name;
	const char *desc;
	const char *content;
	
	struct object *loc;
	struct object *dest;	

	struct object *equip;

	int weight;
	int capacity;
} object;

#define player    (objects + 0)
#define cave      (objects + 1)
#define field     (objects + 2)
#define vial      (objects + 3)
#define coin      (objects + 4)
#define man       (objects + 5)
#define cave_entr (objects + 6)
#define cave_exit (objects + 7)

#define NUM_OBJ   (objects + 8)

object objects[] = {
	// PLAYER
	{"player"   , "player"          , "You have ", field, NULL, NULL, 100, 20},
	// MAP
	{"cave"     , "a small cavern"  , "You see ", NULL, NULL, NULL, 9999, 9999},
	{"field"    , "an open field"   , "You see", NULL, NULL, NULL, 9999, 9999},
	// ITEMS
	{"vial"     , "a glass vial"    , "It's filled with ", cave, NULL, NULL, 1, 1},
	{"coin"     , "a golden coin"   , "", man, NULL, NULL, 1, 0},
	// PEOPLE
	{"man"      , "a grumpy old man", "They have ", field, NULL, NULL, 90, 20},
	// PASSAGES
	{"cave_entr", "a cave entrance" , "", field, cave, NULL, 9999, 9999},
	{"cave_exit", "a cave exit"     , "", cave, field, NULL, 9999, 9999},
};

object *get_object(const char *noun) {
	object *obj;
	for (obj = objects; obj < NUM_OBJ; obj++)
		if(strcmp(noun, obj->name) == 0) return obj;
	printf("I do not know what a '%s' is.\n", noun);
	return NULL;
}

void list_objects(object *loc) {
	object *obj;
	printf("%s\n", loc->content);
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

object *get_passage(object *from, object *to) {
	if(from != NULL && to != NULL) {
		object *obj;
		for(obj = objects; obj < NUM_OBJ; obj++) {
			if(obj->loc == from && obj->dest == to)
				return obj;
		}
	}
	return NULL;
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
	object *obj = get_object(noun);
	if(obj != NULL && get_passage(player->loc, obj) != NULL)
		player->loc = obj;
	exec_look("around", "");
}

void exec_equip(const char *noun, const char *target) {
	object *obj = get_object(noun);
	if(obj != NULL && obj->loc == player) {
		player->equip = obj;
		printf("You equipped the %s.\n", player->equip->name);
		return;
	}
	printf("You have no %s to equip.\n", noun);
}

void exec_unequip(const char *noun, const char *target) {
	object *obj = get_object(noun);
	if(obj != NULL && obj == player->equip) {
		player->equip = NULL;
		printf("You unequipped the %s.\n", obj->desc);
		return;
	}
	printf("You are not holding %s.\n", noun);
}

void exec_grab(const char *noun, const char *target) {
	object *obj = get_object(noun);
	if(obj != NULL && obj->loc == player->loc) {
		grab_object(obj);
		return;
	}
	printf("There is no %s here.\n", noun);
}

void exec_drop(const char *noun, const char *target) {
	object *obj = get_object(noun);
	if(obj != NULL && obj->loc == player && obj != player->equip) {
		drop_object(obj);
		return;
	}
	else if(obj != NULL && obj == player->equip) {
		exec_unequip(noun, target);
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
	{"equip"	, exec_equip},
	{"unequip"	, exec_unequip},
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
		strcmp(token, "with") == 0 ||
		strcmp(token, "out") == 0 ||
		strcmp(token, "of") == 0 ||
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
