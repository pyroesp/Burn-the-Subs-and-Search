#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "bts.h"

int main(int argc, char **argv){
    int i;
    int exit;
    char *query = NULL;

    int ret_val;
    char *err_msg = NULL;
    sqlite3 *db = NULL;

    SQL_table sub;

    SDL_Window *w = NULL; // SDL window
    SDL_Surface *ws = NULL; // SDL window surface
    SDL_Surface *bts = NULL; // bts board
    SDL_Rect bts_blit; // position and size of bts to show on screen
    int bts_scaling_factor = 2; // bts board scaling
    SDL_Surface *wood_bg; // wood bg image
    SDL_Surface *logo = NULL; // twitch logo
    SDL_Rect logo_pos; // logo position
    SDL_Surface *text = NULL; // names
    SDL_Rect text_pos; // text position
    TTF_Font *font_big = NULL; // big font
    TTF_Font *font_small = NULL; // small font
    SDL_Event e; // used for keyboard, mouse,... events
    int mouse_x; // mouse x
    int mouse_y; // mouse y

    char name[128] = {0};
    char prev_name[128] = {0};
    int search_new_name = 0; // searching new name state machine
    int save_to_bmp = 0;

    /** Initializing SQL_table structure **/
    sub.data_size = 0;
    sub.name = NULL;
    sub.id = NULL;
    sub.pos_x = NULL;
    sub.pos_y = NULL;
    sub.font_size = NULL;
    sub.entry_time = NULL;

    printf("Starting BTSearch:\n");
    printf("==================\n\n");

    /** Initializing SDL **/
    if (SDL_Init(SDL_INIT_VIDEO)){
        printf("SDL could not initialize... SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    if (TTF_Init()){
        printf("TTF could not initialize... TTF_Error: %s\n", TTF_GetError());
        return -1;
    }
    w = SDL_CreateWindow("BTS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                         BTS_WIN_W, BTS_WIN_H, SDL_WINDOW_SHOWN);
    ws = SDL_GetWindowSurface(w);

    /** Creating BTS Surface **/
    bts = SDL_CreateRGBSurface(0, BTS_BOARD_W_PX, BTS_BOARD_H_PX, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    SDL_FillRect(bts, NULL, 0xFFFFFFFF);

    /** Loading wood background **/
    wood_bg = SDL_LoadBMP(BTS_WOOD_BG);
    SDL_BlitScaled(wood_bg, NULL, bts, NULL);

    /** Loading logo **/
    logo = SDL_LoadBMP(BTS_CENTERFANCY);
    SDL_SetColorKey(logo, SDL_TRUE, 0xFFFFFFFF);
    logo_pos.x = (BTS_BOARD_W_PX - logo->w)/2;
    logo_pos.y = 0;
    logo_pos.w = logo->w;
    logo_pos.h = logo->h;
    SDL_BlitSurface(logo, NULL, bts, &logo_pos);


    /** Read query for all usernames **/
    query = SQL_queryRead(SQL_GETALLNAMES);
    if (query == NULL)
        return -1;

    /** Connect to DB and execute query **/
    sqlite3_open(SQL_DATABASE, &db);
    ret_val = sqlite3_exec(db, query, &SQL_callbackReadData, &sub, &err_msg);
    if (ret_val != SQLITE_OK){
        printf("Error %d: %s\n", ret_val, err_msg);
        free(err_msg);
    }
    sqlite3_close(db);

    /** Write names to BTS board/surface **/
    SDL_Color c1 = {0, 0, 0, 0xFF}; // BLACK
    SDL_Color c2 = {0xFF, 0, 0, 0xFF}; //RED
    SDL_Color c = c1;
    for (i = 0; i < sub.data_size; i++){
        // Get text position from query
        text_pos.x = sub.pos_x[i];
        text_pos.y = sub.pos_y[i];

        // Check if position is not 0
        if(text_pos.x != 0 && text_pos.y != 0){
            // Render name to SDL Surface
            if (sub.font_size[i] > BTS_FONT_THRESHOLD){
                font_big = TTF_OpenFont(BTS_FONT_BIG, sub.font_size[i]);
                text = TTF_RenderText_Solid(font_big, sub.name[i],  c);
            } else {
                font_small = TTF_OpenFont(BTS_FONT_SMALL, sub.font_size[i]);
                text = TTF_RenderText_Solid(font_small, sub.name[i],  c);
            }

            // Draw text to bts surface
            SDL_BlitSurface(text, NULL, bts, &text_pos);
            SDL_FreeSurface(text);

            if (font_big){
                TTF_CloseFont(font_big);
                font_big = NULL;
            } else if (font_small){
                TTF_CloseFont(font_small);
                font_small = NULL;
            }
        }
    }

    /** Main loop **/
    exit = 0;
    const uint8_t *keys = SDL_GetKeyboardState(NULL); // get pointer to sdl internal keyboard state
    while(!exit){
        /** Checking events **/
        while(SDL_PollEvent(&e)){
            switch(e.type){
                case SDL_QUIT:
                    exit = 1;
                    break;
                case SDL_KEYDOWN:
                    // CTRL+F enables search name feature on key release
                    if (search_new_name == 0 && (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]) && keys[SDL_SCANCODE_F])
                        search_new_name = 1;
                    // CTRL+S saves image to BMP on key release
                    if (save_to_bmp == 0 && (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL]) && keys[SDL_SCANCODE_S])
                        save_to_bmp = 1;
                    switch(e.key.keysym.sym){
                        case SDLK_ESCAPE:
                            exit = 1;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    // CTRL+F enables search name feature on key release
                    if (search_new_name == 1 && (!keys[SDL_SCANCODE_LCTRL] || !keys[SDL_SCANCODE_RCTRL]) && !keys[SDL_SCANCODE_F]){
                        search_new_name = 2;
                        SDL_StartTextInput(); // Start text input
                        memset((void*)name, 0, sizeof(name)); // Clear name array
                        font_big = TTF_OpenFont(BTS_FONT_BIG, 76); // Open ttf font for search text
                    }
                    // CTRL+S saves image to BMP on key release
                    if (save_to_bmp == 1 && (!keys[SDL_SCANCODE_LCTRL] || !keys[SDL_SCANCODE_RCTRL]) && !keys[SDL_SCANCODE_S]){
                        save_to_bmp = 0;
                        SDL_SaveBMP(bts, "bts.bmp");
                        printf("Image saved\n");
                    }

                    switch(e.key.keysym.sym){
                        // Zoom in/out with +/-
                        case SDLK_KP_PLUS:
                        case SDLK_PLUS:
                            if (bts_scaling_factor > 1)
                                bts_scaling_factor--;
                            break;
                        case SDLK_KP_MINUS:
                        case SDLK_MINUS:
                            if (BTS_WIN_W * bts_scaling_factor < bts->w && BTS_WIN_H * bts_scaling_factor < bts->h)
                                bts_scaling_factor++;
                            break;
                        // Backspace used in search feature
                        case SDLK_BACKSPACE:
                            // As long as name string isn't empty, you can use backspace to clear the name
                            if(search_new_name == 2){
                                if (strlen(name)){
                                    name[strlen(name) - 1] = 0;
                                }
                            }
                            break;
                        // Return key to end search feature
                        case SDLK_RETURN:
                            if (search_new_name == 2){
                                SDL_StopTextInput(); // Stop text input
                                TTF_CloseFont(font_big); // Close the font
                                font_big = NULL;

                                search_new_name = 0; // Reset search state machine

                                for (i = 0; i < sub.data_size; i++){
                                    // Check if name to be written is the one we searched
                                    // Or if it's the previous searched name
                                    if (!strcmp(sub.name[i], name) || !strcmp(sub.name[i], prev_name)){
                                        text_pos.x = sub.pos_x[i];
                                        text_pos.y = sub.pos_y[i];

                                        // If it's the new name, change the color to red
                                        if (!strcmp(sub.name[i], name))
                                            c = c2;
                                        // Else set the color to black
                                        else
                                            c = c1;

                                        // Change font depending on size for better clarity
                                        if (sub.font_size[i] > BTS_FONT_THRESHOLD){
                                            font_big = TTF_OpenFont(BTS_FONT_BIG, sub.font_size[i]);
                                            text = TTF_RenderText_Solid(font_big, sub.name[i], c);
                                        } else {
                                            font_small = TTF_OpenFont(BTS_FONT_SMALL, sub.font_size[i]);
                                            text = TTF_RenderText_Solid(font_small, sub.name[i], c);
                                        }

                                        // Overwrite previous and new searched names in their respective colors
                                        SDL_BlitSurface(text, NULL, bts, &text_pos);
                                        SDL_FreeSurface(text);

                                        // Close appropriate font
                                        if (font_big){
                                            TTF_CloseFont(font_big);
                                            font_big = NULL;
                                        } else if (font_small){
                                            TTF_CloseFont(font_small);
                                            font_small = NULL;
                                        }
                                    }
                                }
                                // Copy new name to previous name
                                strcpy(prev_name, name);
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                // Zoom in/out with mouse scroll wheel
                case SDL_MOUSEWHEEL:
                    if (e.wheel.y > 0){
                        if (bts_scaling_factor > 1)
                            bts_scaling_factor--;
                    }
                    else if (e.wheel.y < 0){
                        if (BTS_WIN_W * bts_scaling_factor < bts->w && BTS_WIN_H * bts_scaling_factor < bts->h)
                            bts_scaling_factor++;
                    }
                    break;
                // SDL text input
                case SDL_TEXTINPUT:
                    if (search_new_name == 2){
                        strcat(name, e.text.text); // Concatenate SDL text to name string
                    }
                    break;
                default:
                    break;
            }
        }

        // Get cursor position
        SDL_GetMouseState(&mouse_x, &mouse_y);

        /** Blit BTS board to window **/
        bts_blit.w = BTS_WIN_W * bts_scaling_factor;
        bts_blit.h = BTS_WIN_H * bts_scaling_factor;
        bts_blit.x = (BTS_BOARD_W_PX - bts_blit.w) * mouse_x / BTS_WIN_W;
        bts_blit.y = (BTS_BOARD_H_PX - bts_blit.h) * mouse_y / BTS_WIN_H;
        SDL_BlitScaled(bts, &bts_blit, ws, NULL);

        /** Show search text on screen **/
        if (search_new_name == 2){
            char temp[128] = {0};
            snprintf(temp, 128, "[%s]", name);
            // Render search text with big font
            text = TTF_RenderText_Solid(font_big, temp, c2);
            // Set text position
            text_pos.x = 0;
            text_pos.y = BTS_WIN_H - text->h;
            text_pos.w = text->w;
            text_pos.h = text->h;
            SDL_FillRect(ws, &text_pos, 0xFF000000);
            SDL_BlitSurface(text, NULL, ws, &text_pos);
            SDL_FreeSurface(text);
        }

        /** Update window **/
        SDL_UpdateWindowSurface(w);
        SDL_FillRect(ws, NULL, 0xFFFFFFFF);
    }

    /** SDL quit **/
    SDL_Quit();
    TTF_Quit();
    SDL_FreeSurface(ws);
    SDL_FreeSurface(bts);
    SDL_FreeSurface(wood_bg);
    SDL_FreeSurface(logo);
    SDL_free(ws);

    /** Free all data used/read **/
    free(sub.id);
    free(sub.pos_x);
    free(sub.pos_y);
    free(sub.font_size);
    for (i = 0; i < sub.data_size; i++){
        free(sub.name[i]);
        free(sub.entry_time[i]);
    }
    free(sub.name);
    free(sub.entry_time);
    free(query);

    printf("=======END========\n\n");
    return 0;
}
