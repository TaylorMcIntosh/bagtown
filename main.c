// NAME :  taylormci
// DATE :  2026-10-25
// PURP :  main.c -> Bagtown Main File

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "color.h"

#define MAX_WORDS 8

static bool is_playing = true;
static char input[100];

// TODO : 
//    talk [ talk to sentient object (health > 0 || animated) ],
//    attack [ attacks a sentient object (health > 0 || animated) ]
//    Order of adjectives -> Rework how descriptions work

// ADJECTIVES {{{
// OPINION, SIZE, PHYS QUALITY, SHAPE, AGE, COLOR, ORIGIN, MATERIAL, TYPE, PURPOSE

typedef struct opinion {
    const int id;
    const char *value;
    const char *color;
} opinion;

const opinion opinions[] = {
    {0,  "null", RED},
};
// }}}

// OBJECTS {{{
typedef struct object {
    const char *name;
    const char *(*desc)(struct object *obj);
    const char *content;
    const char *details;
    const char *text_go;
    const char *dialogue;

    struct opinion opinion;

    struct object *loc;
    struct object *dest;
    struct object *prop;
    struct object *equip;
    struct object *want;

    int health;
    int weight;
    int capacity;

    bool is_liquid;
    bool holds_liquid;
    bool (*exists)(void);
} object;

#define player     (objects + 0)
#define cave       (objects + 1)
#define field      (objects + 2)
#define vial       (objects + 3)
#define coin       (objects + 4)
#define crate      (objects + 5)
#define water      (objects + 6)
#define man        (objects + 7)
#define cave_entr  (objects + 8)
#define cave_blkd  (objects + 9)
#define cave_exit  (objects + 10)

#define NUM_OBJ    (objects + 11)

const char *read_desc(struct object *obj);

bool always_true(void);
bool cave_check_entr(void);
bool cave_check_blkd(void);

object objects[] = {
    // PLAYER
    {"player"   
    , read_desc
    ,"You have "
    ,""
    ,""
    ,""
    , opinions[0]
    , field, NULL, NULL, NULL, NULL
    , 20, 100, 20
    , false, false, always_true
    },
    // MAP
    {"cave"     
    , read_desc      
    ,"You see "   
    ,""
    ,""
    ,""
    , opinions[0]
    , NULL, NULL, NULL, NULL, NULL
    , 0, 9999, 9999
    , false, true, always_true
    },

    {"field"    
    , read_desc
    ,"You see "   
    ,""
    ,""
    ,""
    , opinions[0]
    , NULL, NULL, NULL, NULL, NULL
    , 0, 9999, 9999
    , false, true, always_true  
    },
    // ITEMS
    {"vial"     
    , read_desc
    ,"It's filled with " 
    ,""
    ,""
    ,""
    , opinions[0]
    , crate, NULL, NULL, NULL, NULL
    , 0, 1, 1
    , false, true, always_true 
    },
    
    {"coin"     
    , read_desc
    ,""       
    ,""
    ,""
    ,""
    , opinions[0]
    , field, NULL, NULL, NULL, NULL
    , 0, 1, 0
    , false, false, always_true
    },
    
    {"crate"    
    , read_desc
    ,"It's filled with " 
    ,""
    ,""
    ,""
    , opinions[0]
    , cave, NULL, NULL, NULL, NULL
    , 0, 25, 20
    , false, false, always_true
    },
    
    {"water"    
    , read_desc
    ,""       
    ,""
    ,""
    ,""
    , opinions[0]
    , cave, NULL, NULL, NULL, NULL
    , 0, 1, 0
    , true, false, always_true
    },
    // PEOPLE
    {"man"      
    , read_desc
    ,"They have "     
    ,""
    ,""
    ,"'This is a conversation' asks the old man"
    , opinions[0]
    , field, NULL, NULL, NULL, coin
    , 20, 90, 20
    , false, false, always_true
    },
    // PASSAGES
    {"cave_entr"
    , read_desc
    ,""       
    ,""
    ,"You walk into the cave.\n"
    ,""
    , opinions[0]
    , field, cave, NULL, NULL, NULL
    , 0, 9999, 9999
    , false, false, cave_check_entr
    },
    
    {"cave_blkd"
    , read_desc
    ,""       
    ,""
    ,"The cave is blocked by the old man.\n"
    ,""
    , opinions[0]
    , field, NULL, cave, NULL, NULL
    , 0, 9999, 9999
    , false, false, cave_check_blkd
    },

    {"cave_exit"
    , read_desc
    ,""       
    ,""
    ,"You exit the cave.\n"
    ,""
    , opinions[0]
    , cave, field, NULL, NULL, NULL
    , 0, 9999, 9999
    , false, false, always_true
    },
};

const char *read_desc(object *obj) {
    static char str[128] = "";
    str[0] = '\0';
    
    strcat(str, "the");

    // OPINION
    if(obj->opinion.id != 0) {
        strcat(str, " ");
        strcat(str, obj->opinion.color);
        strncat(str, obj->opinion.value, strlen(obj->opinion.value));
        strcat(str, RESET);
    }

    // OBJECT NAME
    strcat(str, " ");
    char *ptr = strchr(obj->name, '_');
    if(ptr != NULL) {
        int len = ptr - obj->name;
        strncat(str, obj->name, len);
    } else strncat(str, obj->name, sizeof(obj->name));
    return str;
}

bool always_true(void) { return true; }
bool cave_check_entr(void) { return man->want->loc == man; }
bool cave_check_blkd(void) { return man->want->loc != man; }
// }}}

// MISC {{{
object *get_object(const char *noun) {
    if(noun == NULL) return NULL;
    object *obj;
    for (obj = objects; obj < NUM_OBJ; obj++)
        if(strcmp(noun, obj->name) == 0) return obj;
    printf("I do not know what a '%s' is.\n", noun);
    return NULL;
}

void list_objects(object *loc) {
    object *obj;
    printf("%s:\n", loc->content);
    for (obj = objects; obj < NUM_OBJ; obj++) {
        if (obj->exists() && obj->loc == loc && obj != player)
            printf("%s\n", obj->desc(obj));
    }
}

void look_around(object *loc) {
    printf("You are in %s\n", loc->desc(loc));
    list_objects(loc);
}

void grab_object(object *obj) {
    if(obj->weight > player->capacity) {
        printf("%s is too heavy for you to pick up.\n", obj->desc(obj));
        return;
    }
    else if(obj->is_liquid) {
        printf("You need something to hold %s.\n", obj->desc(obj));
        return;
    }
    printf("You grabbed %s ", obj->desc(obj));
    printf("from %s.\n", obj->loc->desc(obj->loc));
    obj->loc = player;
    player->capacity -= obj->weight;
}

void drop_object(object *obj) {
    obj->loc = player->loc;
    player->capacity += obj->weight;
    printf("You dropped %s.\n", obj->desc(obj));
}

void fill_object(object *obj, object *tar) {
    tar->loc = obj;
    obj->capacity -= tar->weight;
    printf("You filled %s", obj->desc(obj));
    printf(" with %s.\n", tar->desc(tar));
}

void give_object(object *obj, object *tar) {
    obj->loc = tar;
    tar->capacity -= obj->weight;
    printf("You gave %s", tar->desc(tar));
    printf(" %s.\n", obj->desc(obj));
}

object *get_passage(object *from, object *to) {
    if(from == NULL && to == NULL) return NULL;
    object *obj;
    for(obj = objects; obj < NUM_OBJ; obj++) {
        if(obj->exists() && obj->loc == from && obj->dest == to)
            return obj;
    }
    return NULL;
}

object *get_prop(object *from, object *to) {
    if(from == NULL && to == NULL) return NULL;
    object *obj;
    for(obj = objects; obj < NUM_OBJ; obj++) {
        if(obj->exists() && obj->loc == from && obj->prop == to)
            return obj;
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

void exec_fill(const char *noun, const char *target) {
    object *obj = get_object(noun);
    object *tar = get_object(target);
    if(obj == NULL || tar == NULL) return;
    else if(obj->holds_liquid && tar->is_liquid && obj->capacity >= tar->weight) fill_object(obj, tar);
    else if(!obj->holds_liquid && !tar->is_liquid && obj->capacity >= tar->weight) fill_object(obj, tar);
    else {
        printf("%s cannot hold", obj->desc(obj));
        printf(" %s.\n", tar->desc(tar));
    }
}

void exec_empty(const char *noun, const char *target) {
    object *obj = get_object(noun);
    object *obj2;
    if(obj != NULL && obj->loc == player) {
        printf("You have emptied: \n");
        for(obj2 = objects; obj2 < NUM_OBJ; obj2++) {
            if(obj == obj2->loc) {
                obj->capacity += obj2->weight;
                obj2->loc = player->loc;
                printf("%s\n", obj2->desc(obj2));
            }
        }
        printf("From %s onto the floor of %s.\n", obj->desc(obj), player->loc->desc(player->loc));
    }
}

void exec_look(const char *noun, const char *target) {
    object *obj = get_object(noun);
    if(strcmp(noun, "around") == 0 || obj == player->loc) {
        look_around(player->loc);
        return;
    }
    else if(obj->loc == player->loc || obj->loc == player) {
        if(obj == NULL) return;
        printf("%s\n", obj->details);
        list_objects(obj);
        return;
    }
    printf("There is no %s here.\n", noun);
}

void exec_move(const char *noun, const char *target) {
    object *obj = get_object(noun);
    if(obj != NULL && get_passage(player->loc, obj) != NULL) {
        printf("%s\n", get_passage(player->loc, obj)->text_go);
        player->loc = obj;
    }
    if(obj != NULL && get_prop(player->loc, obj) != NULL)
        printf("%s\n", get_prop(player->loc, obj)->text_go);
    exec_look(player->loc->name, "");
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
        printf("You unequipped the %s.\n", obj->desc(obj));
        return;
    }
    printf("You are not holding %s.\n", noun);
}

void exec_grab(const char *noun, const char *target) {
    object *obj = get_object(noun);
    object *tar = get_object(target);
    if(obj == NULL) return;
    else if(obj->loc == player->loc && tar == NULL) {
        grab_object(obj);
        return;
    }
    else if(obj->loc == tar && tar->health == 0) {
        grab_object(obj);
        return;
    }
    else if(obj->loc == tar && tar->health > 0) {
        printf("no.\n");
        return;
    }
    else if(obj->loc == player->loc && tar->loc == player) {
        exec_fill(target, noun);
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

void exec_give(const char *noun, const char *target) {
    object *obj = get_object(noun);
    object *tar = get_object(target);
    if (obj == NULL || tar == NULL) return;
    else if(obj->loc == player && tar->health > 0 && tar->want == obj) 
        give_object(obj, tar);
    else if(tar->loc == player && obj->health > 0 && obj->want == tar) 
        give_object(tar, obj);
    else if(obj->loc == player && tar->health > 0 && tar->want != obj) 
        printf("'I have no use for that.' says %s.\n", tar->desc(tar)); 
    else if(tar->loc == player && obj->health > 0 && obj->want != tar)
        printf("'I have no use for that.' says %s.\n", obj->desc(obj)); 
}

void exec_talk(const char *noun, const char *target) {
    object *obj = get_object(noun);
    if(obj == NULL) return;
    else if(obj->health > 0 && obj->want->loc != obj) printf("%s.\n", obj->dialogue);
}

typedef void (*command_func)(const char *noun, const char *target);

typedef struct {
    const char *name;
    command_func func;
} command;

static const command commands[] = {
    {"quit"     , exec_quit},
    {"talk"     , exec_talk},
    {"look"     , exec_look},
    {"examine"  , exec_look},
    {"move"     , exec_move},
    {"travel"   , exec_move},
    {"go"       , exec_move},
    {"grab"     , exec_grab},
    {"get"      , exec_grab},
    {"give"     , exec_give},
    {"fill"     , exec_fill},
    {"drop"     , exec_drop},
    {"empty"    , exec_empty},
    {"equip"    , exec_equip},
    {"unequip"  , exec_unequip},
    {"inventory"    , exec_inventory},
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
        strcmp(token, "to") == 0 ||
        strcmp(token, "into") == 0 ||
        strcmp(token, "onto") == 0 ||
        strcmp(token, "from") == 0 ||
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
