#include "common.h"
#include "cmath"
#include "vector"

bool Init();
void CleanUp();
void Run();
void Draw();
void Connect();
void Wall(double x1, double y1, double x2, double y2);
void DrawWalls();
void DrawLine(int x1, int y1, double slope, double slopey, double slopex, double minx, double miny);
vector<double> WallCross(double slope, double slopex, double slopey);
bool SameSign(double num1, double num2);
double ScaleNum(double n, double minN, double maxN, double min, double max);

SDL_Window *window;
SDL_GLContext glContext;
SDL_Surface *gScreenSurface = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Rect pos;

int screenWidth = 500;
int screenHeight = 500;
int viewAng = 80;
int x1 = 20;
int y1 = screenHeight / 2;
int x2 = screenWidth/2;
int y2 = 20;
bool view = false;
const double PI = 3.14159265;

vector<vector<double>> Walls;
vector<double> Distances;

bool Init()
{
    if (SDL_Init(SDL_INIT_NOPARACHUTE & SDL_INIT_EVERYTHING) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s\n", SDL_GetError());
        return false;
    }
    else
    {
        //Specify OpenGL Version (4.2)
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_Log("SDL Initialised");
    }

    //Create Window Instance
    window = SDL_CreateWindow(
        "Game Engine",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screenWidth,
        screenHeight,   
        SDL_WINDOW_OPENGL);

    //Check that the window was succesfully created
    if (window == NULL)
    {
        //Print error, if null
        printf("Could not create window: %s\n", SDL_GetError());
        return false;
    }
    else{
        gScreenSurface = SDL_GetWindowSurface(window);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        SDL_Log("Window Successful Generated");
    }
    //Map OpenGL Context to Window
    glContext = SDL_GL_CreateContext(window);

    return true;
}

int main()
{
    //Error Checking/Initialisation
    if (!Init())
    {
        printf("Failed to Initialize");
        return -1;
    }

    // Clear buffer with black background
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    //Swap Render Buffers
    SDL_GL_SwapWindow(window);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    Run();

    CleanUp();
    return 0;
}

void CleanUp()
{
    //Free up resources
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Run()
{
    bool gameLoop = true;
    srand(time(NULL));
    
    Wall(x1, y1, x2, y2);
    Wall(x2, y2, x1+screenWidth-40, y1);
    Wall(x1, y1, x2, y2+screenHeight-40);
    Wall(x2, y2+screenHeight-40, x1+screenWidth-40, y1);
    while (gameLoop)
    {   
        int ctime = clock();
        Draw();
        if(!view)
            DrawWalls();
        Walls.clear();
        Wall(x1, y1, x2, y2);
        Wall(x2, y2, x1+screenWidth-40, y1);
        Wall(x1, y1, x2, y2+screenHeight-40);
        Wall(x2, y2+screenHeight-40, x1+screenWidth-40, y1);
        SDL_RenderPresent(renderer);
        pos.x = 0;
        pos.y = 0;
        pos.w = screenWidth;
        pos.h = screenHeight;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &pos);

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                gameLoop = false;
            }
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym){
                    case SDLK_ESCAPE:
                        gameLoop = false;
                        break;
                    case SDLK_SPACE:
                        if(view)
                            view = false;
                        else
                            view = true;
                        break;
                    case SDLK_e:
                        viewAng++;
                        if(viewAng > Distances.size() - 1)
                            viewAng = 0;
                        break;
                    case SDLK_q:
                        viewAng--;
                        if(viewAng < 0)
                            viewAng = Distances.size() - 1;
                        break;
                    case SDLK_w:
                        y1-=5;
                        y2-=5;
                        break;
                    case SDLK_a:
                        x1-=5;
                        x2-=5;
                        break;
                    case SDLK_s:
                        y1+=5;
                        y2+=5;
                        break;
                    case SDLK_d:
                        x1+=5;
                        x2+=5;
                        break;
                    case SDLK_UP:
                        y2-=5;
                        break;
                    case SDLK_LEFT:
                        x2-=5;
                        break;
                    case SDLK_DOWN:
                        y2+=5;
                        break;
                    case SDLK_RIGHT:
                        x2+=5;
                        break;
                    default:
                        break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym){
                    default:
                        break;
                }
            }
        }
    }
}

void DrawWalls(){
    int ang = viewAng;
    double wallWidth = (static_cast<double>(screenWidth) / 2) / (static_cast<double>(Distances.size()) / 8);
    double x = screenWidth / 2;
    for(int i = 0; i < Distances.size()/8; i++){
        if(ang > Distances.size() - 1){
            ang = 0;
        }
        int wallHeight;
        if(Distances[ang] == 1000000)
            wallHeight = 0;
        else{
            wallHeight = ScaleNum(Distances[ang], 0, sqrt(pow(screenWidth / 2, 2) + pow(screenHeight / 2, 2)), screenHeight, 10);
        }
        pos.x = floor(x);
        pos.y = (screenHeight / 2) - (wallHeight / 2);
        pos.w = wallWidth;
        pos.h = wallHeight;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &pos);
        ang++;
        x += wallWidth;
    }
    ang = viewAng;
    x = screenWidth / 2;
    for(int i = 0; i < Distances.size()/8; i++){
        x -= wallWidth;
        ang--;
        if(ang < 0){
            ang = Distances.size() - 1;
        }
        int wallHeight;
        if(Distances[ang] == 1000000)
            wallHeight = 0;
        else{
            wallHeight = ScaleNum(Distances[ang], 0, sqrt(pow(screenWidth / 2, 2) + pow(screenHeight / 2, 2)), screenHeight, 10);
        }
        pos.x = ceil(x);
        pos.y = (screenHeight / 2) - (wallHeight / 2);
        pos.w = wallWidth;
        pos.h = wallHeight;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &pos);
    }
}

void Draw(){
    Distances.clear();
    for(double ang = 0; ang < 2*PI; ang += PI/64){
        double slopex = cos(ang);
        double slopey = sin(ang);
        if(slopex < 0.01 && slopex > -.01)
            slopex = 0.0;
        else if(slopey < .01 && slopey > -.01)
            slopey = 0.0;
        double slope = slopey / slopex;
        vector<double> mins = WallCross(slope, slopex, slopey);
        double minx = mins[0];
        double miny = mins[1];
        double minDist = mins[2];
        Distances.push_back(minDist);
        if(view)
            DrawLine(screenWidth / 2, screenHeight / 2, slope, slopey, slopex, minx, miny);
    }
}

void DrawLine(int x1, int y1, double slope, double slopey, double slopex, double minx, double miny){
    int prevx = 0;
    int prevy = 0;
    if(slopex == 0){
        if(slopey >= 0){
            if(miny == 1000000)
                miny = screenHeight;
            for(int y = screenHeight / 2; y < miny; y++){
                pos.x = screenWidth / 2;
                pos.y = y;
                pos.w = 1;
                pos.h = 1;
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &pos);
            }
        }
        else if(slopey < 0){
            if(miny == 1000000)
                miny = 0;
            for(int y = screenHeight / 2; y >= miny; y--){
                pos.x = screenWidth / 2;
                pos.y = y;
                pos.w = 1;
                pos.h = 1;
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &pos);
            }
        }
    }
    else if(slopey == 0){
        if(slopex >= 0){
            if(minx == 1000000)
                minx = screenWidth;
            else if(minx >= screenWidth)
                minx = screenWidth - 1;
            for(int x = screenWidth / 2; x < minx; x++){
                pos.x = x;
                pos.y = screenHeight / 2;
                pos.w = 1;
                pos.h = 1;
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &pos);
            }
        }
        if(slopex < 0){
            if(minx == 1000000)
                minx = 0;
            else if(minx < 0)
                minx = 0;
            for(int x = screenWidth / 2; x >= minx; x--){
                pos.x = x;
                pos.y = screenHeight / 2;
                pos.w = 1;
                pos.h = 1;
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &pos);
            }
        }
    }
    else if(slopex >= 0 && slopey >= 0){
        if(minx == 1000000)
            minx = screenWidth;
        else if(minx >= screenWidth)
            minx = screenWidth - 1;
        for(double x = screenWidth / 2; x < minx; x+=abs(slopex)){
            pos.x = x;
            pos.y = slope * x + slope * (-1 * x1) + y1;
            pos.w = 1;
            pos.h = 1;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &pos);
        }
    }
    else if(slopex < 0 && slopey < 0){
        if(minx == 1000000)
            minx = 0;
        else if(minx < 0)
            minx = 0;
        for(double x = screenWidth/2; x >= minx; x-=abs(slopex)){
            pos.x = x;
            pos.y = slope * x + slope * (-1 * x1) + y1;
            pos.w = 1;
            pos.h = 1;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &pos);
        }
    }
    else if(slopex < 0 && slopey >= 0){
        if(minx == 1000000)
            minx = 0;
        else if(minx < 0)
            minx = 0;
        for(double x = screenWidth / 2; x >= minx; x-=abs(slopex)){
            pos.x = x;
            pos.y = slope * x + slope * (-1 * x1) + y1;
            pos.w = 1;
            pos.h = 1;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &pos);
        }
    }
    else if(slopex >= 0 && slopey < 0){
        if(minx == 1000000)
            minx = screenWidth;
        else if(minx >= screenWidth)
            minx = screenWidth - 1;
        for(double x = screenWidth / 2; x < minx; x+=abs(slopex)){
            pos.x = x;
            pos.y = slope * x + slope * (-1 * x1) + y1;
            pos.w = 1;
            pos.h = 1;
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &pos);
        }
    }
        
}

void Wall(double x1, double y1, double x2, double y2){
    double slope = static_cast<double>((y2 - y1)) / (x2 - x1);
    double slopex = x2 - x1;
    double slopey = y2 - y1;
    if(x2 < x1)
        Walls.push_back({x2, y2, x1, y1, slope, slopex, slopey});
    else
        Walls.push_back({x1, y1, x2, y2, slope, slopex, slopey});
}

vector<double> WallCross(double slope, double slopex, double slopey){
    double minDist = 1000000;
    double minx = 1000000;
    double miny = 1000000;
    double m1, m2, b1, b2, interx, intery, interDistance;

    for(int index = 0; index < Walls.size(); index++){
        if(slopex == 0 && Walls[index][0] == screenWidth/2 && Walls[index][5] == 0){
            int maxwally = Walls[index][1];
            int minwally = Walls[index][3];
            if(Walls[index][1] < Walls[index][3]){
                maxwally = Walls[index][3];
                minwally = Walls[index][1];
            }
            if(slopey == -1){
                interx = screenWidth/2;
                intery = maxwally;
                interDistance = sqrt(pow(interx - screenWidth / 2, 2) + pow(intery - screenHeight / 2, 2));
            }
            else if(slopey == 1){
                interx = screenWidth/2;
                intery = minwally;
                interDistance = sqrt(pow(interx - screenWidth / 2, 2) + pow(intery - screenHeight / 2, 2));
            }
        }
        else if(Walls[index][0] == screenWidth/2 && Walls[index][5] == 0 && slopex == 1){
            interx = 1000000;
            intery = 1000000;
            interDistance = 1000000;
        }
        else if(slopex == 0){
            if(Walls[index][0] <= screenWidth/2 && Walls[index][2] >= screenWidth/2){
                interx = screenWidth/2;
                b1 = Walls[index][4] * (-1 * Walls[index][0]) + Walls[index][1];
                intery = Walls[index][4] * interx + b1;
                interDistance = sqrt(pow(interx - screenWidth / 2, 2) + pow(intery - screenHeight / 2, 2));
            }
        }
        else{
            if(Walls[index][5] == 0){
                if(slopex != 0){
                    interx = Walls[index][0];
                    b1 = slope * (-1 * screenWidth/2) + screenWidth/2;
                    intery = slope * interx + b1;
                    interDistance = sqrt(pow(interx - screenWidth / 2, 2) + pow(intery - screenHeight / 2, 2));
                }
            }
            else{
                m1 = Walls[index][4];
                m2 = slope;
                b1 = m1 * (-1 * Walls[index][0]) + Walls[index][1];
                b2 = m2 * (-1 * screenWidth/2) + screenWidth/2;
                interx = (b1 - b2)/(m2 - m1);
                intery = m1 * interx + b1;
                interDistance = sqrt(pow(interx - screenWidth / 2, 2) + pow(intery - screenHeight / 2, 2));
            }
        }
        if(slopex == 0){
            if(interx >= Walls[index][0] && interx <= Walls[index][2] && SameSign(slopey, intery - screenHeight / 2)){
                if(interDistance < minDist){
                    minDist = interDistance;
                    minx = interx;
                    miny = intery;
                }
            }
        }
        else if(slopey == 0){
            int maxwally = Walls[index][1];
            int minwally = Walls[index][3];
            if(Walls[index][1] < Walls[index][3]){
                maxwally = Walls[index][3];
                minwally = Walls[index][1];
            }
            if(interx >= Walls[index][0] && interx <= Walls[index][2] && intery >= minwally && intery <= maxwally && SameSign(slopex, interx - screenWidth / 2)){
                if(interDistance < minDist){
                    minDist = interDistance;
                    minx = interx;
                    miny = intery;
                }
            }
        }
        else if(interx >= Walls[index][0] && interx <= Walls[index][2] && SameSign(slopex, interx - screenWidth / 2) && SameSign(slopey, intery - screenHeight / 2)){
            int maxwally = Walls[index][1];
            int minwally = Walls[index][3];
            if(Walls[index][1] < Walls[index][3]){
                maxwally = Walls[index][3];
                minwally = Walls[index][1];
            }
            if(interDistance < minDist && intery <= maxwally && intery >= minwally){
                minDist = interDistance;
                minx = interx;
                miny = intery;
            }
        }
    }
    vector<double> mins = {minx, miny, minDist};
    return mins;
}

bool SameSign(double num1, double num2){
    if(num1 >= 0 && num2 >= 0)
        return true;
    else if(num1 < 0 && num2 < 0)
        return true;
    else
        return false;
}

double ScaleNum(double n, double minN, double maxN, double min, double max){
    return (((n - minN) / (maxN - minN)) * (max - min)) + min;
}