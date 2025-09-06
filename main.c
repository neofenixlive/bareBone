/*
▐               ▗▄▄
▐▄▖  ▄▖  ▖▄  ▄▖ ▐  ▌ ▄▖ ▗▗▖  ▄▖
▐▘▜ ▝ ▐  ▛ ▘▐▘▐ ▐▄▄▘▐▘▜ ▐▘▐ ▐▘▐
▐ ▐ ▗▀▜  ▌  ▐▀▀ ▐  ▌▐ ▐ ▐ ▐ ▐▀▀
▐▙▛ ▝▄▜  ▌  ▝▙▞ ▐▄▄▘▝▙▛ ▐ ▐ ▝▙▞
UNiX Terminal Game Engine
*/

//--------------------------------
//C LIBRARIES
//--------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//--------------------------------
//GAME CONSTANTS
//--------------------------------
#define ENTITY_LIMIT 200
#define ENTITY_SET 6
#define LEVEL_WIDTH 10
#define LEVEL_HEIGHT 10
#define LEVEL_QUANTITY 3
#define GAME_TITLE "▐               ▗▄▄             \n▐▄▖  ▄▖  ▖▄  ▄▖ ▐  ▌ ▄▖ ▗▗▖  ▄▖ \n▐▘▜ ▝ ▐  ▛ ▘▐▘▐ ▐▄▄▘▐▘▜ ▐▘▐ ▐▘▐ \n▐ ▐ ▗▀▜  ▌  ▐▀▀ ▐  ▌▐ ▐ ▐ ▐ ▐▀▀ \n▐▙▛ ▝▄▜  ▌  ▝▙▞ ▐▄▄▘▝▙▛ ▐ ▐ ▝▙▞ \nUNiX Terminal Game Engine <alpha1>"
#define GAME_INSTRUCTIONS "<,> - Walk left, <.> - Walk right, <;> - Jumps"
#define GAME_H_BORDER "=-="
#define GAME_V_BORDER "|x|"

/*
--------------------------------
AVAILABLE FUNCTIONS:
--------------------------------

entity_make(code, x, y)
entity_remove(id)
entity_set(id, x, y, code)
entity_match(id, int pointer)
level_load(code)

--------------------------------
AVAILABLE PARAMETERS:
--------------------------------

x (entity)
y (entity)
code (entity)
id (entity)
*/

struct entity {
    int x;
    int y;
    int code;
    int id;
};

struct entity entities[ENTITY_LIMIT];
int entity_count = 0;
char entity_char[ENTITY_SET][4];
int render_list[LEVEL_WIDTH*LEVEL_HEIGHT];
int levels[LEVEL_QUANTITY][LEVEL_WIDTH*LEVEL_HEIGHT];

int player_jump = 0;
int player_score = 0;
int player_level = 1;




//--------------------------------
//ENTITY/LEVEL HANDLING
//--------------------------------

//ADDS ENTITY TO GAME
struct entity* entity_make(int code, int x, int y) {
    struct entity newEntity = {x, y, code, entity_count};
    entities[entity_count] = newEntity;    entity_count++;
    retur &entities[entity_count-1];
};

//DELETES ENTITY FROM GAME
int entity_remove(int id) {
    struct entity blankSpace = {0, 0, 0, 0};
    entities[id] = blankSpace;
    return 0;
};

//SETS ENTITY TO PARAMETERS
int entity_set(int id, int x, int y, int code) {
    struct entity newEntity = {x, y, code, id};
    entities[id] = newEntity;
    return 0;
};

//GETS ARRAY OF ENTITIES WITH CODE
struct entity* entity_match(int code, int* out_count) {
    struct entity* matches = malloc(sizeof(struct entity)*ENTITY_LIMIT);
    int count = 0;

    for(int i=0; i<entity_count; i++) {
        if(entities[i].code == code) {
            matches[count] = entities[i];
            count++;
        }
    }

    *out_count = count;
    return matches;
    //REMEMBER TO free()
};

//LEVEL LOADING FROM LIST
int level_load(int code) {
    if(code>=LEVEL_QUANTITY) {
        exit(0);
    }
    memset(entities, 0, sizeof(entities));
    entity_count = 0;
    for(int y=0; y<LEVEL_HEIGHT; y++) {
        for(int x=0; x<LEVEL_WIDTH; x++) {
            entity_make(levels[code][(y*LEVEL_WIDTH)+x], x, y);
        }
    }

    return 0;
};



//--------------------------------
//ENTITY/LEVEL SETTINGS
//--------------------------------

//SET ENTITY BEHAVIOUR HERE
int entity_update(int id, char input[]) {
    switch(entities[id].code) {
    case 1: {
        int block_count = 0;
        int spike_count = 0;
        int coin_count = 0;
        int flag_count = 0;
        struct entity* blocks = entity_match(2, &block_count);
        struct entity* spikes = entity_match(3, &spike_count);
        struct entity* coins = entity_match(4, &coin_count);
        struct entity* flags = entity_match(5, &flag_count);

        int collide_x1 = 0;
        int collide_x2 = 0;
        for(int i=0; i<block_count; i++) {
            if (blocks[i].x == entities[id].x-1 && blocks[i].y == entities[id].y) {
                collide_x1 = 1;
            }
            if (blocks[i].x == entities[id].x+1 && blocks[i].y == entities[id].y) {
                collide_x2 = 1;
            }
        }

        if(strcmp(input,",") == 0) {
            if(collide_x1 == 0) {
                entities[id].x -= 1;
            }
        }
        if(strcmp(input,".") == 0) {
            if(collide_x2 == 0) {
                entities[id].x += 1;
            }
        }

        int collide_y1 = 0;
        int collide_y2 = 0;
        for(int i=0; i<block_count; i++) {
            if (blocks[i].x == entities[id].x && blocks[i].y == entities[id].y-1) {
                collide_y1 = 1;
            }
            if (blocks[i].x == entities[id].x && blocks[i].y == entities[id].y+1) {
                collide_y2 = 1;
            }
        }

        if(strcmp(input,";") == 0) {
            if(collide_y1 == 0 && collide_y2 == 1) {
                player_jump = 2;
            }
        }
        if(collide_y1 == 0 && player_jump > 0) {
            entities[id].y -= 1;
        }
        if(collide_y2 == 0 && player_jump < 1) {
            entities[id].y += 1;
        }
        player_jump -= 1;

        for(int i=0; i<spike_count; i++) {
            if (spikes[i].x == entities[id].x && spikes[i].y == entities[id].y) {
                level_load(player_level-1);
                player_jump = 0;
            }
        }

        for(int i=0; i<coin_count; i++) {
            if (coins[i].x == entities[id].x && coins[i].y == entities[id].y) {
                entity_remove(coins[i].id);
                player_score += 10;
            }
        }

        for(int i=0; i<flag_count; i++) {
            if (flags[i].x == entities[id].x && flags[i].y == entities[id].y) {
                level_load(player_level);
                player_level += 1;
                player_jump = 0;
            }
        }

        free(blocks);
        free(spikes);
        free(coins);
        free(flags);
        break;
    }
    default: {
        break;
    }
    };

    return 0;
};

//SET ENTITY APPEARANCE HERE
char entity_char[ENTITY_SET][4] = {
    "   ",
    " @ ",
    "[ ]",
    "< >",
    " * ",
    " # "
};

//SET LEVELS HERE
int levels[LEVEL_QUANTITY][LEVEL_WIDTH*LEVEL_HEIGHT] = {
    {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 5,
        0, 0, 0, 0, 0, 0, 0, 4, 0, 0,
        0, 0, 0, 0, 0, 4, 0, 0, 0, 2,
        0, 0, 0, 4, 0, 0, 0, 2, 0, 2,
        0, 1, 0, 0, 0, 2, 0, 2, 0, 2,
        2, 2, 2, 2, 3, 2, 3, 2, 3, 2
    },

    {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 4, 0, 0, 0, 4, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 2, 2, 0, 2, 2, 2, 0, 0, 0,
        0, 0, 0, 3, 0, 0, 0, 3, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 4, 0, 4, 0,
        5, 0, 0, 4, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 2, 2, 2, 2,
        2, 0, 0, 2, 0, 0, 0, 0, 0, 0
    },

    {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        5, 0, 0, 4, 0, 0, 0, 0, 0, 0,
        2, 2, 2, 2, 0, 0, 4, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 2, 2, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
        0, 0, 4, 0, 3, 0, 0, 0, 0, 2,
        1, 0, 0, 0, 3, 0, 0, 4, 0, 0,
        2, 0, 3, 0, 0, 0, 0, 2, 0, 0,
        2, 0, 3, 0, 0, 4, 0, 0, 0, 0,
        2, 0, 0, 2, 2, 2, 0, 0, 0, 0
    }
};



//--------------------------------
//GAME UPDATE/RENDER
//--------------------------------

//SET OUTPUT VARIABLE NAMES
const char *output_print[] = {
    "Score: ",
    "Level: "
};

//SET OUTPUT VARIABLE VALUES
const int *output_value[] = {
    &player_score,
    &player_level
};

//GET INPUT AND UPDATE ENTITIES
int update() {
    char input[255];
    printf("Enter input: ");
    scanf("%254s", input);

    for(int i=0; i<entity_count; i++) {
        entity_update(i, input);
    }

    return 0;
}

//ORDERS RENDER LIST BASED ON ENTITIES
int render_order() {
    memset(render_list, 0, sizeof(render_list));

    for(int i=0; i<entity_count; i++) {
        if(entities[i].code != 0) {
            if(entities[i].x > -1 && entities[i].x < LEVEL_WIDTH) {
                if(entities[i].y > -1 && entities[i].y < LEVEL_HEIGHT) {
                    if(render_list[(entities[i].y*LEVEL_WIDTH)+entities[i].x] == 0) {
                        render_list[(entities[i].y*LEVEL_WIDTH)+entities[i].x] = entities[i].code;
                    }
                }
            }
        }
    }
};

//PRINTS RENDER LIST WITH ENTITY_CHAR
int render_print() {
    for(int y=0; y<LEVEL_HEIGHT; y++) {
        printf("%s", GAME_V_BORDER);
        for(int x=0; x<LEVEL_WIDTH; x++) {
            printf("%s", entity_char[render_list[(y*LEVEL_WIDTH)+x]]);
        }
        printf("%s\n", GAME_V_BORDER);
    }
};

//RENDERS TITLE, ENTITIES, INSTRUCTIONS AND MORE
int render() {
    render_order();

    system("clear");
    printf("%s\n", GAME_TITLE);

    for(int i=0; i<LEVEL_WIDTH+2; i++) {
        printf("%s", GAME_H_BORDER);
    }
    printf("\n");

    render_print();

    for(int i=0; i<LEVEL_WIDTH+2; i++) {
        printf("%s", GAME_H_BORDER);
    }
    printf("\n");

    for(int i=0; i<sizeof(output_value)/sizeof(output_value[0]); i++) {
        printf("%s", output_print[i]);
        printf("%d\n", *output_value[i]);
    }
    printf("\n");

    printf("%s\n", GAME_INSTRUCTIONS);
    return 0;
};



//--------------------------------
//MAIN FUNCTION
//--------------------------------

int main() {
    level_load(0);
    while(1) {
        update();
        render();
    }
    return 0;
}
