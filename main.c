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
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

//--------------------------------
//GAME CONSTANTS
//--------------------------------
#define ENTITY_LIMIT 200
#define ENTITY_SET 6
#define LEVEL_WIDTH 10
#define LEVEL_HEIGHT 10
#define LEVEL_QUANTITY 5
#define GAME_TITLE "▐               ▗▄▄             \n▐▄▖  ▄▖  ▖▄  ▄▖ ▐  ▌ ▄▖ ▗▗▖  ▄▖ \n▐▘▜ ▝ ▐  ▛ ▘▐▘▐ ▐▄▄▘▐▘▜ ▐▘▐ ▐▘▐ \n▐ ▐ ▗▀▜  ▌  ▐▀▀ ▐  ▌▐ ▐ ▐ ▐ ▐▀▀ \n▐▙▛ ▝▄▜  ▌  ▝▙▞ ▐▄▄▘▝▙▛ ▐ ▐ ▝▙▞ \nUNiX Terminal Game Engine <alpha3>"
#define GAME_INSTRUCTIONS "<,> - Walk left, <.> - Walk right, <;> - Jumps"
#define GAME_H_BORDER "=-="
#define GAME_V_BORDER "|x|"
#define GAME_UPDATE_TIME 200000
#define GAME_LEVEL_START 0

/*
--------------------------------
AVAILABLE FUNCTIONS:
--------------------------------
entity_make(code, x, y)
entity_remove(id)
entity_set(id, x, y, code)
entity_match(id, int pointer)
level_load(code)
level_reset()

--------------------------------
AVAILABLE VARIABLES:
--------------------------------
x (entity struct)
y (entity struct)
code (entity struct)
id (entity struct)
game_frame
game_input
*/

struct entity {
    int x;
    int y;
    int code;
    int id;
};

int game_running = 1;
int game_frame = 0;
char game_input = '\0';
int game_render[LEVEL_WIDTH*LEVEL_HEIGHT];
struct entity entities[ENTITY_LIMIT];
char entity_char[ENTITY_SET][4];
int entity_count = 0;
int levels[LEVEL_QUANTITY][LEVEL_WIDTH*LEVEL_HEIGHT];



int player_score = 0;
int player_level = 1;
int player_jump = 0;



//--------------------------------
//ENTITY/LEVEL HANDLING
//--------------------------------

//ADD ENTITY TO GAME
struct entity* entity_make(int code, int x, int y) {
    struct entity newEntity = {x, y, code, entity_count};
    entities[entity_count] = newEntity;    entity_count++;
    return &entities[entity_count-1];
};

//DELETE ENTITY FROM GAME
int entity_remove(int id) {
    struct entity blankSpace = {0, 0, 0, 0};
    entities[id] = blankSpace;
    return 0;
};

//SET ENTITY TO PARAMETERS
int entity_set(int id, int x, int y, int code) {
    struct entity newEntity = {x, y, code, id};
    entities[id] = newEntity;
    return 0;
};

//GET ARRAY OF ENTITIES WITH CODE
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
    if(code>=LEVEL_QUANTITY || code<0) {
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

//DEFINE GAME RESET HERE
int level_reset() {
    level_load(GAME_LEVEL_START);
    player_score = 0;
    player_level = 1;
    player_jump = 0;
};


//--------------------------------
//ENTITY/LEVEL SETTINGS
//--------------------------------

//SET ENTITY BEHAVIOUR HERE
int entity_update(int id) {
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

        if(game_input == ',') {
            if(collide_x1 == 0) {
                entities[id].x -= 1;
            }
        }
        if(game_input == '.') {
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

        if(game_input == ';') {
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

        if(entities[id].x<0 || entities[id].x>LEVEL_WIDTH-1 || entities[id].y>LEVEL_HEIGHT-1){
            player_jump = 0;
            level_load(player_level-1);
        }
        
        for(int i=0; i<spike_count; i++) {
            if (spikes[i].x == entities[id].x && spikes[i].y == entities[id].y) {
                player_jump = 0;
                level_load(player_level-1);
            }
        }

        for(int i=0; i<coin_count; i++) {
            if (coins[i].x == entities[id].x && coins[i].y == entities[id].y) {
                player_score += 10;
                entity_remove(coins[i].id);
            }
        }

        for(int i=0; i<flag_count; i++) {
            if (flags[i].x == entities[id].x && flags[i].y == entities[id].y) {
                player_level += 1;
                player_jump = 0;
                level_load(player_level-1);
            }
        }

        free(blocks);
        free(spikes);
        free(coins);
        free(flags);
        break;
    }
    default: break;
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
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 4, 0, 5,
        0, 0, 0, 0, 0, 4, 0, 0, 0, 2,
        0, 0, 0, 4, 0, 0, 0, 2, 0, 0,
        0, 1, 0, 0, 0, 2, 0, 0, 0, 0,
        2, 2, 2, 2, 0, 0, 0, 0, 0, 0},

    {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 4, 0, 0, 0, 4, 0, 0, 0,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 2, 2, 0, 2, 2, 2, 0, 0, 0,
        0, 0, 0, 3, 0, 0, 0, 3, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
        0, 0, 0, 0, 0, 4, 0, 0, 0, 0,
        5, 0, 4, 0, 0, 0, 0, 4, 0, 2,
        2, 0, 0, 0, 0, 2, 0, 0, 2, 0,
        0, 0, 2, 0, 0, 0, 0, 2, 0, 0},

    {   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        5, 0, 0, 4, 0, 0, 0, 0, 0, 0,
        2, 2, 2, 2, 0, 0, 4, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 2, 2, 0, 4,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 3, 0, 0, 4, 0, 2,
        1, 0, 0, 4, 3, 0, 0, 0, 0, 0,
        2, 0, 3, 4, 0, 0, 0, 2, 0, 0,
        2, 0, 3, 0, 0, 0, 0, 0, 0, 0,
        2, 0, 0, 2, 2, 2, 0, 0, 0, 0},

    {   0, 0, 0, 0, 0, 0, 4, 0, 0, 0,
        0, 0, 0, 4, 0, 0, 2, 0, 0, 0,
        5, 0, 0, 2, 0, 0, 3, 0, 4, 0,
        2, 0, 0, 3, 0, 0, 0, 0, 2, 3,
        3, 0, 0, 0, 0, 0, 0, 4, 0, 0,
        0, 0, 0, 0, 0, 0, 3, 2, 0, 0,
        0, 0, 0, 0, 4, 0, 0, 0, 0, 4,
        0, 0, 0, 4, 0, 4, 0, 0, 0, 2,
        0, 1, 0, 0, 3, 0, 0, 0, 0, 3,
        0, 2, 2, 2, 2, 2, 2, 2, 0, 0},

    {   2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        0, 0, 4, 0, 4, 0, 4, 0, 2, 4,
        5, 0, 3, 0, 3, 0, 3, 0, 0, 0,
        2, 2, 2, 2, 2, 2, 2, 2, 4, 2,
        3, 0, 0, 0, 0, 0, 0, 0, 0, 3,
        2, 4, 2, 2, 2, 2, 2, 2, 2, 2,
        0, 0, 0, 4, 0, 3, 0, 4, 0, 0,
        0, 2, 2, 2, 0, 2, 0, 2, 0, 0,
        0, 0, 4, 3, 0, 4, 0, 3, 0, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
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

//CHANGE TERMINAL TO RAWMODE
int input_set() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    return 0;
}

//CHANGE TERMINAL TO DEFAULT
int input_restore() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
    fcntl(STDIN_FILENO, F_SETFL, 0);
    return 0;
}
    
//GET INPUT AND UPDATE ENTITIES
int update() {
    if (read(STDIN_FILENO, &game_input, 1) > 0) {
        if (game_input == 'q') {
            game_running = 0;
        };
        if (game_input == 'r') {
            level_reset();
        };
    }
    
    for(int i=0; i<entity_count; i++) {
        entity_update(i);
    }

    game_frame++;
    game_input = '\0';
    return 0;
}

//ORDER RENDER LIST BASED ON ENTITIES
int render_order() {
    memset(game_render, 0, sizeof(game_render));

    for(int i=0; i<entity_count; i++) {
        if(entities[i].code != 0) {
            if(entities[i].x > -1 && entities[i].x < LEVEL_WIDTH) {
                if(entities[i].y > -1 && entities[i].y < LEVEL_HEIGHT) {
                    if(game_render[(entities[i].y*LEVEL_WIDTH)+entities[i].x] == 0) {
                        game_render[(entities[i].y*LEVEL_WIDTH)+entities[i].x] = entities[i].code;
                    }
                }
            }
        }
    }
};

//PRINT RENDER LIST WITH ENTITY_CHAR
int render_print() {
    for(int i=0; i<LEVEL_WIDTH+2; i++) {
        printf("%s", GAME_H_BORDER);
    }
    printf("\n");
    
    for(int y=0; y<LEVEL_HEIGHT; y++) {
        printf("%s", GAME_V_BORDER);
        for(int x=0; x<LEVEL_WIDTH; x++) {
            printf("%s", entity_char[game_render[(y*LEVEL_WIDTH)+x]]);
        }
        printf("%s\n", GAME_V_BORDER);
    }
    
    for(int i=0; i<LEVEL_WIDTH+2; i++) {
        printf("%s", GAME_H_BORDER);
    }
    printf("\n");
};

//RENDER TITLE, ENTITIES, INSTRUCTIONS AND MORE
int render() {
    system("clear");
    printf("%s\n", GAME_TITLE);

    render_order();
    render_print();

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
    input_set();
    level_load(GAME_LEVEL_START);
    
    while(game_running) {
        update();
        render();
        usleep(GAME_UPDATE_TIME);
    }
    
    input_restore();
    return 0;
}
