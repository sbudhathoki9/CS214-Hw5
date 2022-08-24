#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "csapp.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>



// Dimensions for the drawn grid (should be GRIDSIZE * texture dimensions)
#define GRID_DRAW_WIDTH 640
#define GRID_DRAW_HEIGHT 640

#define WINDOW_WIDTH GRID_DRAW_WIDTH
#define WINDOW_HEIGHT (HEADER_HEIGHT + GRID_DRAW_HEIGHT)

// Header displays current score
#define HEADER_HEIGHT 50

// Number of cells vertically/horizontally in the grid
#define GRIDSIZE 10

typedef struct
{
    int x;
    int y;
} Position;

typedef enum
{
    TILE_GRASS,
    TILE_TOMATO,
    TILE_PLAYER
} TILETYPE;

TILETYPE grid[GRIDSIZE][GRIDSIZE];

typedef struct Client Client;
struct Client {

  char playernumber;
  Position playerPosition;
  Client* next;

};

Client* insert(Client* head,char playernum, Position p) { 

    if (head == NULL) {

        head = malloc(sizeof(Client));
        head->next = NULL;
        head->playernumber = playernum;
        head->playerPosition = p;

    }
    else {

        Client* newJob = malloc(sizeof(Client));
        newJob->next = head;
        newJob->playernumber = playernum;
        newJob->playerPosition = p;
        head = newJob;

    }
    return head;

}
Client* deleteClient(Client* head, char portno){
        
    // Store head node
    Client* temp = head;
    Client* prev = NULL;

    // If head node itself holds
    // the key to be deleted
    if (temp != NULL && temp->playernumber== portno)
    {
       
        head = temp->next; // Changed head
        free(temp);            // free old head
        return head;
    }else{
        
    while (temp != NULL && temp->playernumber== portno)
    {

        prev = temp;
        temp = temp->next;
    }
 
    if (temp == NULL)
        return head;
 
    // Unlink the node from linked list

    prev->next = temp->next;
    //updatejobID();
 
    // Free memory
    free(temp);
    return head;
    }
}
Client* pop(Client* head) {

    if (head == NULL) {

    }
    else {

        if (head->next != NULL) {
            Client* temp = head;
            head = head->next;
            free(temp);
        }
        else {
            Client* temp = head;
            head = NULL;
            free(temp);
        }
    }

    return head;

}
void terminate_all(Client* head) {
    Client* del;

    while (head != NULL) {
        del = head;
        head = head->next;
        free(del);

    }


}


Client* globalClient = NULL;
char Currentplayernumber;
Position CurrentplayerPosition;


int score;
int level;
char playermove[1];
int numTomatoes;
char Grid[108];
bool keypress;

bool shouldExit = false;

TTF_Font* font;

// get a random value in the range [0, 1]
double rand01()
{
    return (double) rand() / (double) RAND_MAX;
}

void initG(char in[]){

    for(int i=0; i<100;i++){

        Grid[i]= 'G';
    }

    for(int j=0; j<8;j++){

        Grid[100+j]= '0';
    }
   //int len = strlen(in);
   

    for(int k=0; k<100;k++){

        Grid[k]= in[k];
    }
    
    for(int z=0; z<8;z++){

        Grid[100+z]= in[100+z];
    }
  

}
 int charToInt(char c){

return c - '0';
}
int getScore(){
    int sum =0;
  


    sum+=charToInt(Grid[100])*10000;
    sum+=charToInt(Grid[101])*1000;
    sum+=charToInt(Grid[102])*100;
    sum+=charToInt(Grid[103])*10;
    sum+=charToInt(Grid[104])*1;
    return sum;

}
int getlevel(){
    int sum =0; 
  
    sum+=charToInt(Grid[105])*100;
    sum+=charToInt(Grid[106])*10;
    sum+=charToInt(Grid[107])*1;
    return sum;

} 
void initGrid(char input[])
{
     int count=0;
    for (int i = 0; i < GRIDSIZE; i++) {
        for (int j = 0; j < GRIDSIZE; j++) {
            
            switch (input[count])
            {
               case 'G':
                grid[j][i]=TILE_GRASS;
                count++;
                break;
                case 'T':
                grid[j][i]=TILE_TOMATO;
                count++;
                break;
               default: 
                grid[j][i]=TILE_PLAYER;
                count++;
                   break;
            }
        }
    }
}

void initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    int rv = IMG_Init(IMG_INIT_PNG);
    if ((rv & IMG_INIT_PNG) != IMG_INIT_PNG) {
        fprintf(stderr, "Error initializing IMG: %s\n", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Error initializing TTF: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
}

void moveTo(int x, int y)
{
    // Prevent falling off the grid
    if (x < 0 || x >= GRIDSIZE || y < 0 || y >= GRIDSIZE)
        return;

    // Sanity check: player can only move to 4 adjacent squares
    if (!(abs(CurrentplayerPosition.x - x) == 1 && abs(CurrentplayerPosition.y - y) == 0) &&
        !(abs(CurrentplayerPosition.x - x) == 0 && abs(CurrentplayerPosition.y - y) == 1)) {
        fprintf(stderr, "Invalid move attempted from (%d, %d) to (%d, %d)\n", CurrentplayerPosition.x, CurrentplayerPosition.y, x, y);
        return;
    }

    CurrentplayerPosition.x = x;
    CurrentplayerPosition.y = y;

    if (grid[x][y] == TILE_TOMATO) {
        grid[x][y] = TILE_GRASS;
        score++;
        numTomatoes--;
        if (numTomatoes == 0) {
            level++;
           // initGrid();
        }
    }
}

void handleKeyDown(SDL_KeyboardEvent* event)
{
    // ignore repeat events if key is held down
    
    if (event->repeat)
        return;

    if (event->keysym.scancode == SDL_SCANCODE_Q || event->keysym.scancode == SDL_SCANCODE_ESCAPE){
        playermove[0] = 'Q';
        keypress = true;
        //shouldExit = true;
    }
    if (event->keysym.scancode == SDL_SCANCODE_UP || event->keysym.scancode == SDL_SCANCODE_W){
       playermove[0]='N'; 
       keypress =true;
       //printf("+++++WWWWWWWWWWWWWWWWWWWW++++\n");
    }

    if (event->keysym.scancode == SDL_SCANCODE_DOWN || event->keysym.scancode == SDL_SCANCODE_S){
       playermove[0]='S'; 
       keypress =true;
       //printf("+++++SSSSSSSSSSSSSSSSSSSSS++++\n");
    }

    if (event->keysym.scancode == SDL_SCANCODE_LEFT || event->keysym.scancode == SDL_SCANCODE_A){
       playermove[0]='E'; 
        keypress =true;
       //printf("+++++AAAAAAAAAAAAAAAAAAAAAAAAAA++++\n");
    }

    if (event->keysym.scancode == SDL_SCANCODE_RIGHT || event->keysym.scancode == SDL_SCANCODE_D){
       playermove[0]='W';  
       keypress =true;
       //printf("+++++DDDDDDDDDDDDDDDDDDDDDDDDDDD++++\n");
    }
}

void processInputs()
{
	SDL_Event event;
   //  printf("+++++INSIDE ProcessInput++++\n");

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				playermove[0]='Q';//shouldExit = true;
                keypress = true;
				break;

            case SDL_KEYDOWN:
                handleKeyDown(&event.key);
				break;

			default:
				break;
		}
	}
}

void drawGrid(SDL_Renderer* renderer, SDL_Texture* grassTexture, SDL_Texture* tomatoTexture, SDL_Texture* playerTexture)
{
  
    SDL_Rect dest;
    SDL_Texture* texture;
    for (int i = 0; i < GRIDSIZE; i++) {
        for (int j = 0; j < GRIDSIZE; j++) {
            dest.x = 64 * i;
            dest.y = 64 * j + HEADER_HEIGHT;

           // SDL_Texture* texture = (grid[i][j] == TILE_GRASS) ? grassTexture : tomatoTexture;
            if(grid[i][j] == TILE_GRASS){
                texture =grassTexture;
                SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
                SDL_RenderCopy(renderer, texture, NULL, &dest);  
            }else if(grid[i][j] == TILE_TOMATO){
                texture =tomatoTexture; 
                SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
                SDL_RenderCopy(renderer, texture, NULL, &dest);
             }else if(grid[i][j] == TILE_PLAYER){
                texture =grassTexture;
                SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);
                SDL_RenderCopy(renderer, texture, NULL, &dest);
                SDL_QueryTexture(playerTexture, NULL, NULL, &dest.w, &dest.h);
                SDL_RenderCopy(renderer, playerTexture, NULL, &dest);    
            }  

           
        }
    }
}

void drawUI(SDL_Renderer* renderer)
{
    char scoreStr[18];
    char levelStr[18];
    sprintf(scoreStr, "Score: %d", score);
    sprintf(levelStr, "Level: %d", level);

    SDL_Color white = {255, 255, 255};
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreStr, white);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);

    SDL_Surface* levelSurface = TTF_RenderText_Solid(font, levelStr, white);
    SDL_Texture* levelTexture = SDL_CreateTextureFromSurface(renderer, levelSurface);

    SDL_Rect scoreDest;
    TTF_SizeText(font, scoreStr, &scoreDest.w, &scoreDest.h);
    scoreDest.x = 0;
    scoreDest.y = 0;

    SDL_Rect levelDest;
    TTF_SizeText(font, levelStr, &levelDest.w, &levelDest.h);
    levelDest.x = GRID_DRAW_WIDTH - levelDest.w;
    levelDest.y = 0;

    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreDest);
    SDL_RenderCopy(renderer, levelTexture, NULL, &levelDest);

    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    SDL_FreeSurface(levelSurface);
    SDL_DestroyTexture(levelTexture);
}

int main(int argc, char* argv[])
{
  
    //exit(0);
    srand(time(NULL));
    keypress=false;
    int clientfd;
    char *host, *port, buf[MAXLINE];
    rio_t rio;
    host = argv[1];
    port = argv[2];
    clientfd = Open_clientfd(host, port);
  
 
    Rio_readinitb(&rio, clientfd);
    Rio_readlineb(&rio, buf, 109);
    
    /*for (int i = 0; i < 108; i++) {
        if(i%10==0){
            printf("\n");
        }
        printf("%c",buf[i]);
    }*/
 
    initG(buf);
    score= getScore();
    level = getlevel();

    /****************** drawing the window ********* */
    initSDL();
    font = TTF_OpenFont("resources/Burbank-Big-Condensed-Bold-Font.otf", HEADER_HEIGHT);
    if (font == NULL) {
        fprintf(stderr, "Error loading font: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }
    
    initGrid(buf);

    SDL_Window* window = SDL_CreateWindow("Client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (window == NULL) {
        fprintf(stderr, "Error creating app window: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL)
	{
		fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
	}

    SDL_Texture *grassTexture = IMG_LoadTexture(renderer, "resources/grass.png");
    SDL_Texture *tomatoTexture = IMG_LoadTexture(renderer, "resources/tomato.png");
    SDL_Texture *playerTexture = IMG_LoadTexture(renderer, "resources/player.png");

    SDL_SetRenderDrawColor(renderer, 0, 105, 6, 255);
    SDL_RenderClear(renderer);
    while(!shouldExit){
        
        SDL_SetRenderDrawColor(renderer, 0, 105, 6, 255);
        SDL_RenderClear(renderer);

        processInputs();
        Rio_writen(clientfd, playermove, 2);
        if(playermove[0] == 'Q'){
            break;
        }
        playermove[0]='K';
        
        Rio_readinitb(&rio, clientfd);
        Rio_readlineb(&rio, buf, 109);
        /*for (int i = 0; i < 108; i++) {
            if(i%10==0){
                printf("\n");
            }
            printf("%c",buf[i]);
        }*/
        initG(buf);
        score = getScore();
        level = getlevel();
        initGrid(buf); 

        drawGrid(renderer, grassTexture, tomatoTexture, playerTexture);
        drawUI(renderer);
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
    Close(clientfd); 
 
    // clean up everything
    SDL_DestroyTexture(grassTexture);
    SDL_DestroyTexture(tomatoTexture);
    SDL_DestroyTexture(playerTexture);

    TTF_CloseFont(font);
    TTF_Quit();

    IMG_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

   

}
