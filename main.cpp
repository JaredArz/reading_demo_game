#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#define PI 3.14159265358979323846264
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define P_WIDTH 80
#define P_HEIGHT 90
#define B_WIDTH 150
#define B_HEIGHT 300
#define BULLET_HEIGHT 150
#define BULLET_WIDTH 150
#define LASER_HEIGHT 450
#define LASER_WIDTH 50
#define MAX_SPRITES 8
#define LASER_ANIMATION_LEN 2

int sdl_init(void);
SDL_Texture *load_texture_from_bmp(const char *filename);
void check_game_over();
int collision(SDL_Rect a, SDL_Rect b);

void init_player(SDL_Texture *txtr_in);
void init_boss(SDL_Texture *txtr_in);
void init_bullet(SDL_Texture *txtr_in);
void init_laser(SDL_Texture *txtr_in);

void draw_player(void);
void draw_boss(void);
void draw_bullet(void);
void draw_laser(void);

void move_player(void);
void move_boss(void);
void move_bullet(int speed);
void move_laser(void);

int player_lives = 3;

enum direction_t{
    L,
    R,
    STATIONARY

};

enum state_t{
    MENU,
    GAME,
    GAME_OVER
};

//Key press surfaces constants
enum KeyPressSurfaces{
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL
};


struct player_t {
    SDL_Rect hitbox;
    bool live;
    SDL_Texture *txtr;
    enum direction_t direction;
};

struct boss_t {
    SDL_Rect hitbox;
    bool live;
    unsigned int health;
    SDL_Texture *txtr;

    int speed;
    int state;
    enum direction_t direction;
};

struct bullet_t {
    SDL_Rect hitbox;
    bool live;
    SDL_Texture *txtr;
};

struct laser_t {
    SDL_Rect hitbox;
    bool live;
    SDL_Texture *txtr[LASER_ANIMATION_LEN];
};

// global variable entities
struct bullet_t bullet;
struct player_t player;
struct boss_t boss;
struct laser_t laser;

// global game state
enum state_t state;

// keep track of all textures created for easy cleanup
SDL_Texture *entity_txtrs[MAX_SPRITES];

// global graphics context
SDL_Window   *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font     *menu_font = NULL;

void check_game_over(void){
    if (player.live == false || boss.health < 1){
        state = GAME_OVER;
    }
}


void init_player(SDL_Texture *txtr_in){
    player.hitbox.x = (SCREEN_WIDTH / 2) - (P_WIDTH / 2);
    player.hitbox.y = SCREEN_HEIGHT - (P_HEIGHT + 10);
    player.hitbox.w = P_WIDTH;
    player.hitbox.h = P_HEIGHT;
    player.txtr = txtr_in;
    player.live = true;
}

void init_boss(SDL_Texture *txtr_in){
    boss.hitbox.x = (SCREEN_WIDTH / 2) - (B_WIDTH / 2);
    boss.hitbox.y = (SCREEN_HEIGHT / 4) - (B_HEIGHT + 10);
    boss.hitbox.w = B_WIDTH;
    boss.hitbox.h = B_HEIGHT;
    boss.txtr = txtr_in;
    boss.health = 20;
    boss.live   = true;
}

void init_bullet(SDL_Texture *txtr_in){
    bullet.hitbox.x = 0;
    bullet.hitbox.y = SCREEN_HEIGHT;
    bullet.hitbox.w = BULLET_WIDTH;
    bullet.hitbox.h = BULLET_HEIGHT;
    bullet.txtr = txtr_in;
    bullet.live = false;
}

void init_laser(SDL_Texture *txtr_in[]){
    laser.txtr[0] = txtr_in[0];
    laser.txtr[1] = txtr_in[1];
    laser.hitbox.x = 0;
    laser.hitbox.y = SCREEN_HEIGHT/3 - 50;
    laser.hitbox.w = LASER_WIDTH;
    laser.hitbox.h = LASER_HEIGHT;
    laser.live = false;
}

void draw_player(void){
    if ( player.live == true )
        SDL_RenderCopy(renderer, player.txtr, NULL, &player.hitbox);
}

void draw_boss(void){
    if ( boss.live == true )
        SDL_RenderCopy(renderer, boss.txtr, NULL, &boss.hitbox);
}

void draw_bullet(void){
    if ( bullet.live == true )
        SDL_RenderCopy(renderer, bullet.txtr, NULL, &bullet.hitbox);
}

//int animation_count = 0;
void draw_laser(void){
    if ( laser.live == true ){
        SDL_RenderCopy(renderer, laser.txtr[0], NULL, &laser.hitbox);
        //if (animation_count < 1000 || (animation_count > 2000 && animation_count < 3000) )
        //else
        //    SDL_RenderCopy(renderer, laser.txtr[1], NULL, &laser.hitbox);
        //animation_count++;
    }
}

void move_player(void){
    if( player.direction == R && (player.hitbox.x + player.hitbox.w < SCREEN_WIDTH))
        player.hitbox.x += 8;
    else if( player.direction == L && player.hitbox.x > 0)
        player.hitbox.x -= 8;
}

bool reverse;
void move_boss(int time){
    if (time % 2027 == 0)
        reverse = !reverse;
    if(reverse && boss.hitbox.x + boss.hitbox.w < SCREEN_WIDTH)
        boss.hitbox.x += 10;
    else if(player.hitbox.x > 0)
        boss.hitbox.x -= 10;
}

bool bset = true;
void move_bullet(int xpos,int speed){
    if (bullet.live == true){
        if( bset ){ 
            bullet.hitbox.x = xpos;
            bullet.hitbox.y = player.hitbox.y - player.hitbox.h;
            bset = false;
        }
        bullet.hitbox.y -= speed;
        if (bullet.hitbox.y <= 0){
            bullet.live = false;
            bset = true;
        }
    }
    else{
        bullet.hitbox.y = player.hitbox.y - player.hitbox.h;
        bset = true;
    }
}

int lticks = 0;
void move_laser(int xpos){
    if (laser.live == true){
        laser.hitbox.x = xpos;
        if (lticks > 4000){
            lticks = 0;
            laser.live = false;
            animation_count = 0;
        }
        lticks++;
    }
}

int collision(SDL_Rect a, SDL_Rect b){
    if (a.y + a.h < b.y)
        return 0;
    if (a.y > b.y + b.h)
        return 0;
    if (a.x > b.x + b.w)
        return 0;
    if (a.x + a.w < b.x)
        return 0;
    return 1;
}

int collision_laser(SDL_Rect p, SDL_Rect l){
    if (p.x < (l.x + l.w) && p.x > (l.x - l.w) )
        return 1;
    return 0;
}

int sdl_init(void){

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ){
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        return 1;
    }

    if (TTF_Init() == -1 ){
        printf("ttf failed init%s\n",TTF_GetError());
        return 1;
    }


    //Set texture filtering to linear
    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear" ) )
        printf( "Warning: Linear texture filtering not enabled!" );

    //FIXME dont know what this does
    SDL_RenderSetLogicalSize(renderer,SCREEN_WIDTH,SCREEN_HEIGHT);

    //Create window
    window = SDL_CreateWindow( "reading", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( window == NULL ){
        printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
        return 1;
    }
    //Create renderer for window
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if( renderer == NULL ){
        printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
        return 1;
    }
    //Initialize renderer color

    SDL_SetRenderDrawColor(renderer,255,0,0,255);

    menu_font = TTF_OpenFont( "./assets/Lazy.ttf",28);
    if (menu_font == NULL){
        printf("Failed to  get font %s\n",TTF_GetError());
        return 1;
    }


	return 0;
}


void sdl_cleanup(void){
    for ( unsigned int i = 0; i < sizeof(entity_txtrs[i]); i++ ){
        SDL_DestroyTexture( entity_txtrs[i] );
        entity_txtrs[i] = NULL;
    }

	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	window   = NULL;
	renderer = NULL;


    TTF_CloseFont(menu_font);
	menu_font = NULL;

	//Quit SDL and its subsystems
	SDL_Quit();
	TTF_Quit();
}



SDL_Texture *load_texture_from_bmp(const char *filename){
    static short num_txtrs = 0;
    SDL_Texture *new_texture = NULL;
    SDL_Surface *temp_surface = NULL;
    temp_surface = SDL_LoadBMP(filename);
    if (temp_surface == NULL){
        printf("UNABLE TO LOAD %s\n",filename);
        return NULL;
    }

    new_texture = SDL_CreateTextureFromSurface(renderer,temp_surface);
    if (new_texture == NULL){
        printf("UNABLE TO CREATE TEXTURE FROM %s\n",filename);
        return NULL;
    }
    SDL_FreeSurface( temp_surface );

    entity_txtrs[num_txtrs] = new_texture;
    num_txtrs++;

    return new_texture;
}



int main( int argc, char* args[] ){
	if( sdl_init() == 1 ){
		printf( "Failed to initialize!\n" );
        return 1;
	}



    //Main loop flag
    bool quit = false;
    //Event handler
    SDL_Event e;

    // begin in menu until mouse press
    state = MENU;

    // load assets
    SDL_Texture *player_txtr    = NULL;  
    SDL_Texture *boss_txtr      = NULL; 
    SDL_Texture *bullet_txtr    = NULL;
    SDL_Texture *menu_txtr      = NULL;   
    SDL_Texture *game_over_txtr = NULL;
    SDL_Texture *laser_txtrs[LASER_ANIMATION_LEN];
    laser_txtrs[0] = NULL;
    laser_txtrs[1] = NULL;

    menu_txtr   = load_texture_from_bmp("./assets/background.bmp");
    player_txtr = load_texture_from_bmp("./assets/eye.bmp");
    boss_txtr   = load_texture_from_bmp("./assets/boss.bmp");
    bullet_txtr = load_texture_from_bmp("./assets/proj.bmp");
    game_over_txtr = load_texture_from_bmp("./assets/game_over.bmp");
    laser_txtrs[0] = load_texture_from_bmp("./assets/laser_one.bmp");
    laser_txtrs[1] = load_texture_from_bmp("./assets/laser_two.bmp"); 

    if( menu_txtr      == NULL ){ printf("ERROR\n"); return 1; }
    if( player_txtr    == NULL ){ printf("ERROR\n"); return 1; }
    if( boss_txtr      == NULL ){ printf("ERROR\n"); return 1; }
    if( bullet_txtr    == NULL ){ printf("ERROR\n"); return 1; }
    if( menu_txtr      == NULL ){ printf("ERROR\n"); return 1; }
    if( game_over_txtr == NULL ){ printf("ERROR\n"); return 1; }
    if( laser_txtrs[0]  == NULL ){ printf("ERROR\n"); return 1; }
    if( laser_txtrs[1]  == NULL ){ printf("ERROR\n"); return 1; }

    
    SDL_Rect dest;
    dest.x = SCREEN_WIDTH  / 2;
    dest.y = SCREEN_HEIGHT / 2;
    dest.w = 86;
    dest.h = 28;
    SDL_Color color_text = {0,0,0};
    SDL_Surface *textS = TTF_RenderText_Solid( menu_font, "click to start", color_text);
    SDL_Texture *text = NULL;
    text = SDL_CreateTextureFromSurface(renderer, textS);
    SDL_FreeSurface( textS );


    init_player(player_txtr);
    init_boss(boss_txtr);
    init_bullet(bullet_txtr);
    init_laser(laser_txtrs);


    SDL_RenderClear( renderer );
    SDL_RenderCopy( renderer, menu_txtr, NULL, NULL );
    SDL_RenderCopy( renderer, text, NULL, &dest );
    SDL_RenderPresent( renderer );


    int start_time = SDL_GetTicks();
    srand(start_time);
    int time;

    bool click = false;
    //While application is running
    while( !quit ){
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 ){
            //User requests quit, exit on next loop around
            if( e.type == SDL_QUIT ){
                quit = true;
            }
            // handle kb press
            if( e.type == SDL_KEYDOWN ){
                switch( e.key.keysym.sym ){
                    case SDLK_d:
                        player.direction = R;
                    break;

                    case SDLK_a:
                        player.direction = L;
                    break;

                    default:
                        ;
                    break;
                }
            }
            // handle mouse press
            if( e.type == SDL_MOUSEBUTTONDOWN )
                click = true;
        }

        // handle game logic depending on state
        if( state == MENU ){
            if ( click ){
                state = GAME;
                click = false;
            }
        }
        else if ( state == GAME ){
            if ( click ){
                bullet.live = true;
                click = false;
            }
            // game logic
            time = SDL_GetTicks();
            move_player();
            if(!laser.live)
                move_boss(time);
            move_bullet(player.hitbox.x - player.hitbox.h/2, 1);
            if ( (time % 2023 == 0) && (time - start_time) > 2000 && !laser.live )
                laser.live = true;
            move_laser(boss.hitbox.x + 25);
            player.direction = STATIONARY;
            if (bullet.live){
                if( collision(boss.hitbox, bullet.hitbox) ){
                    boss.health -= 1;
                    bullet.live = 0;
                }
            }
            if (laser.live){
                if( collision_laser(player.hitbox, laser.hitbox )){
                    player.lives -= 1;
                    SDL_RenderCopy( renderer, menu_txtr, NULL, NULL);
                    draw_player();
                    draw_boss();
                    draw_bullet();
                    draw_laser();
                    SDL_Delay(1000);
                    SDL_RenderCopy( renderer, game_over_txtr, NULL, NULL);
                    SDL_RenderPresent( renderer );
                    SDL_Delay(1000);
                }
            }
            if (player.lives == 0)
                player.live = false;

        }
        else if ( state == GAME_OVER ){
            if ( click ){
                state = MENU;
                click = false;
                init_player(player_txtr);
                init_boss(boss_txtr);
                init_bullet(bullet_txtr);
                init_laser(laser_txtrs);
            }
            else{
                ;
            }
        }



        SDL_RenderClear( renderer );

        // handle rendering depending on state
        if( state == MENU ){
            SDL_RenderCopy( renderer, menu_txtr, NULL, NULL);
            SDL_RenderCopy( renderer, text, NULL, &dest );
        }
        else if ( state == GAME ){
            SDL_RenderCopy( renderer, menu_txtr, NULL, NULL);
            draw_player();
            draw_boss();
            draw_bullet();
            draw_laser();
        }
        else if ( state == GAME_OVER ){
            SDL_RenderCopy( renderer, game_over_txtr, NULL, NULL);
        }
        SDL_RenderPresent( renderer );
        check_game_over();
    }

    sdl_cleanup();
    
	return 0;
}
