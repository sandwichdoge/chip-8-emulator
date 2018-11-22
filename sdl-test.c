#include <stdio.h>
#include <SDL2/SDL.h>

//http://lazyfoo.net/tutorials/SDL/01_hello_SDL/index2.php

//Screen dimension constants
const int SCREEN_WIDTH = 640; const int SCREEN_HEIGHT = 480;

struct GUI {
    SDL_Window *window; //the title bar and the border that surrounds it (aka the window frame)
    SDL_Surface *screen_surface; //the area contained within the window frame
};


struct GUI GUICreate()
{
    struct GUI ret;
    ret.window = NULL;
    ret.screen_surface = NULL;

    ret.window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if (ret.window == NULL) {
        printf("Failed to create window.\n");
    }

    ret.screen_surface = SDL_GetWindowSurface(ret.window);
    //SDL_FillRect(ret.screen_surface, NULL, SDL_MapRGB(ret.screen_surface->format, 0x00, 0x00, 0x00));
    //SDL_UpdateWindowSurface(ret.window);
    
    return ret;
}


int main(int argc, char *args[])
{
    struct GUI my_GUI;

    //Initialize SDL 
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) { 
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() ); 
    }

    my_GUI = GUICreate();

    SDL_Surface *hBackground = SDL_LoadBMP("/home/z/Downloads/wp.bmp");
    if (hBackground == NULL) printf("Error loading media.\n");

    //blit the loaded surface onto the GUI surface
    SDL_BlitSurface(hBackground, NULL, my_GUI.screen_surface, NULL);
    SDL_UpdateWindowSurface(my_GUI.window); //then update GUI surface


    SDL_Event e;
    int quit = 0;

    while (quit == 0) {
        while (SDL_PollEvent(&e)) { //poll events until event stack is empty
            switch (e.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;
                case SDL_WINDOWEVENT:
                    switch (e.window.event) {
                        case SDL_WINDOWEVENT_ENTER:
                            printf("Mouse entered window.\n");
                            break;
                        case SDL_WINDOWEVENT_SHOWN:
                            SDL_UpdateWindowSurface(my_GUI.window);
                            break;
                        case SDL_WINDOWEVENT_MINIMIZED:
                            printf("Window is hidden.\n");
                            break;
                    }
                    break;
                case SDL_KEYDOWN:
                    puts("key pressed");
            }
        }
    }

    //clean up
    SDL_FreeSurface(hBackground);
    SDL_DestroyWindow(my_GUI.window);
    SDL_Quit();

    return 0;
}