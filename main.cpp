#include <SDL2/SDL.h>
#include <vector>
#include <X11/Xlib.h>
#include <iostream>
#include <vector>

#define PI 3.14159265

Display * display;
Window root_window;
SDL_Window * window;
SDL_Renderer * renderer;
uint8_t screen_num = 0;
int * sin_values;
int screen_size[2] = {1600, 900};
int framerate_ = 250;
unsigned int radius = 350;
uint8_t r,g,b = 0;
std::vector<SDL_Point> circle;
SDL_Point origin;
bool runAsWallpaper = true;
bool run = true;


void init_video(){
    SDL_Init(SDL_INIT_VIDEO);
    if(runAsWallpaper) {
        display = XOpenDisplay(nullptr);
        screen_size[0] = DisplayWidth(display, screen_num);
        screen_size[1] = DisplayHeight(display, screen_num);
        root_window = RootWindow(display, DefaultScreen(display));
        window = SDL_CreateWindowFrom((void*) root_window);
    } else window = SDL_CreateWindow("Trigonometry", -1, -1, 1600, 900, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void init_math(const int& framerate, const int& radius){
    sin_values = (int*)malloc((2*framerate)*sizeof(int)); //2pi/(pi/framerate) = 2*framerate -> all different points for a given framerate
    std::cout << "Loading a period of sin(x)";
    for(int i = 0; i < 2*framerate; i++){
        *(sin_values + i) = (int)(radius*sin(i*PI/framerate));
        std::cout << ".";
    }
    std::cout << "\n";
    std::cout << "Loading the circle";
    origin = {radius + 32, screen_size[1]-radius - 32};
    double r_squared = pow(radius, 2);
    double r_squared_minus_1 = pow(radius-1, 2);
    double dst;
    for(int y = origin.y - (int)radius; y < origin.y; y++) for(int x = origin.x - (int)radius; x < origin.x; x++){
            dst = pow(x-origin.x, 2) + pow(y-origin.y, 2);

            if(r_squared_minus_1 < dst && dst < r_squared){
                circle.push_back({x,y});
                circle.push_back({-1*(x - origin.x) + origin.x, -1*(y - origin.y) + origin.y});
                circle.push_back({-1*(x - origin.x) + origin.x, y});
                circle.push_back({x, -1*(y - origin.y) + origin.y});
            }
        }
}

void destroy(){
    free(sin_values);
    if(runAsWallpaper) XCloseDisplay(display);
    SDL_Quit();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
}



int8_t parse_args(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            if (i + 1 == argc) {
                std::cout << "Too few arguments, quitting.\n";
                return -3;
            }
            screen_num = (uint8_t) strtol(argv[i + 1], nullptr, 10);
            if (screen_num == 0 && argv[i + 1][0] != '0') {
                std::cout << "Screen number isn't a number, quitting.\n";
                return -3;
            }
            if (screen_num >= ScreenCount(display)) {
                std::cout << "Screen number is superior the the ScreenCount(), quitting.\n";
                return -3;
            }
            i++;
        } else if (strcmp(argv[i], "-R") == 0) {
            if (i + 1 == argc) {
                std::cout << "Too few arguments, quitting.\n";
                return -3;
            }
            radius = (unsigned int) strtol(argv[i + 1], nullptr, 10);
            if (radius == 0 && argv[i + 1][0] != '0') {
                std::cout << "Circle radius isn't a number, quitting.\n";
                return -3;
            }
            i++;
        } else if (strcmp(argv[i], "-r") == 0) {
            if (i + 1 == argc) {
                std::cout << "Too few arguments, quitting.\n";
                return -3;
            }
            r = (uint8_t) strtol(argv[i + 1], nullptr, 10);
            if (r == 0 && argv[i + 1][0] != '0') {
                std::cout << "The amount of red isn't a number, quitting.\n";
                return -3;
            }
            i++;
        } else if (strcmp(argv[i], "-g") == 0) {
            if (i + 1 == argc) {
                std::cout << "Too few arguments, quitting.\n";
                return -3;
            }
            g = (uint8_t) strtol(argv[i + 1], nullptr, 10);
            if (g == 0 && argv[i + 1][0] != '0') {
                std::cout << "The amount of green isn't a number, quitting.\n";
                return -3;
            }
            i++;
        } else if (strcmp(argv[i], "-b") == 0) {
            if (i + 1 == argc) {
                std::cout << "Too few arguments, quitting.\n";
                return -3;
            }
            b = (uint8_t) strtol(argv[i + 1], nullptr, 10);
            if (b == 0 && argv[i + 1][0] != '0') {
                std::cout << "The amount of blue isn't a number, quitting.\n";
                return -3;
            }
            i++;
        } else if (strcmp(argv[i], "-w") == 0) {
            runAsWallpaper = false;
        } else if (strcmp(argv[i], "-?") == 0 | strcmp(argv[i], "--help") == 0) {
            std::cout << "MathPaper help\n"
                         "  -s NUM : select the number of the screen whose wallpaper should be changed\n"
                         "  -w     : run as a window and not a wallpaper"
                         "  -R NUM : choose the radius of the circle\n"
                         "  -r NUM : choose the amount of red color\n"
                         "  -g NUM : choose the number of green color\n"
                         "  -b NUM : choose the number of blue color\n"
                         "  --help or -? : display this message\n";
        } else {
            std::cout << "Option not recognized. Please use --help\n";
        }
    }
    if (r + g + b == 0) {
        std::cout << "You must specify at least a color !\n";
        return -3;
    }
    return 0;
}

void drawcircle(){
    for(SDL_Point p : circle){
        SDL_RenderDrawPoint(renderer, p.x, p.y);
    }
}

void drawdottedline(int x0, int y0, int x1, int y1){
    int y_old = 0;
    bool color = false;
    if(x0  == x1){ // special case where the slope isn't defined
        int adder = (y1 > y0 ? 1 : -1);
        int y = adder*20;
        while((y1-y0) * adder > y * adder){
            if(color) SDL_RenderDrawLine(renderer, x0, y_old+y0, x0, y + y0);
            y_old = y;
            y += adder*20;
            color = !color;
        }
        if(color) SDL_RenderDrawLine(renderer, x0, y_old+y0, x0, y1);

    } else{
        int slope = (y0-y1)/(x0-x1);
        int adder = (x1 > x0 ? 1 : -1);
        int x = adder*20;
        int x_old = 0;
        while(adder * (x1-x0) > adder * x){
            if(color) SDL_RenderDrawLine(renderer, x_old+x0, y_old+y0, x+x0, x*slope + y0);
            x_old = x;
            y_old = x*slope;
            x += adder*20;
            color = !color;
        }
        if(color) SDL_RenderDrawLine(renderer, x_old+x0, y_old+y0, x1, y1);
    }
}

int main(int argc, char** argv)
{
    int8_t return_val = parse_args(argc, argv);
    if(return_val) return return_val;
    init_video();
    init_math(framerate_, radius);
    for(int it_cos = 0, it_sin = framerate_/2; run ; it_cos = (it_cos + 1) % (2 * framerate_), it_sin = (it_sin + 1) % (2 * framerate_)){
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        drawcircle();
        SDL_RenderDrawLine(renderer, origin.x - radius, origin.y, screen_size[0], origin.y);
        SDL_RenderDrawLine(renderer, origin.x, 0, origin.x, origin.y + radius);
        SDL_RenderDrawLine(renderer, origin.x + radius, origin.y + radius, origin.x + radius, 0);
        SDL_RenderDrawLine(renderer, origin.x - radius, origin.y, origin.x - radius, 0);
        SDL_RenderDrawLine(renderer, origin.x, origin.y, *(sin_values + it_cos) + origin.x, *(sin_values + it_sin) + origin.y);//image.x, image.y);
        SDL_RenderDrawLine(renderer, origin.x, origin.y + radius, screen_size[0], origin.y + radius);
        SDL_RenderDrawLine(renderer, origin.x-radius, origin.y - radius, screen_size[0], origin.y - radius);
        for(int i = 0; i < screen_size[0] - origin.x - radius; i++){
            SDL_RenderDrawLine(renderer, origin.x + radius + 3*i, *(sin_values + (it_sin - i + 1500) % (2 * framerate_)) + origin.y, origin.x + radius + 3 * i + 3, *(sin_values + (it_sin - i - 1 + 1500) % (2 * framerate_)) + origin.y);
        }
        for(int i = 0; i < origin.y - radius - 1; i++){
            SDL_RenderDrawLine(renderer, *(sin_values + (it_cos - i + 1500) % (2 * framerate_)) + origin.x, origin.y - radius - 3*i, *(sin_values + (it_cos - i - 1 + 1500) % (2 * framerate_)) + origin.x, origin.y - radius - 3*i - 3);
        }
        drawdottedline(*(sin_values + it_cos) + origin.x, *(sin_values + it_sin) + origin.y, origin.x + radius, *(sin_values + it_sin) + origin.y);
        drawdottedline(*(sin_values + it_cos) + origin.x, *(sin_values + it_sin) + origin.y, *(sin_values + it_cos) + origin.x, origin.y - radius);
        SDL_RenderPresent(renderer);
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT)
                run = false;
        }
    }
    destroy();
}
