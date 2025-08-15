// Raycasting.cpp 
// By: Sam Schmitz


#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define mapWidth 24
#define mapHeight 24
#define screenWidth 640
#define screenHeight 480

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_RIGHT:
        if (action == GLFW_PRESS) {
            std::cout << "Should rotate right\n";
        }
        break;
    default:
        break;
    }
}

struct Engine {
    GLFWwindow* window;
    float width;
    float height;

    Engine() {
        if (!glfwInit()) {
            std::cout << "Failed to initialize GLFW\n";
            exit(EXIT_FAILURE);
        }
        window = glfwCreateWindow(screenWidth, screenHeight, "Raycasting", NULL, NULL);
        if (!window) {
            std::cout << "Failed to create GLFW window\n";
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwMakeContextCurrent(window);
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cout << "Failed to initialize GLEW\n";
            glfwDestroyWindow(window);
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glViewport(0, 0, screenWidth, screenHeight);
    }

    void beginFrame() {
        // clear the frame each render loop iteration
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

        glMatrixMode(GLPROJECTION);
        glLoadIdentity();
        glOrtho(0, screenWidth, screenHeight, 0, -1, -1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }
};
Engine engine;

struct ColorRGB {
    float r, g, b;

    ColorRGB() : r(0), g(0), b(0) {}
    ColorRGB(float red, float green, float blue) : r(red), g(green), b(blue) {}

    // Divide brightness
    ColorRGB operator/(float value) const {
        return ColorRGB(r / value, g / value, b / value);
    }

    void apply() const {
        glColor3f(r, g, b);
    }
};

const ColorRGB RGB_Red = ColorRGB(1.0f, 0.0f, 0.0f);
const ColorRGB RGB_Green = ColorRGB(0.0f, 1.0f, 0.0f);
const ColorRGB RGB_Blue = ColorRGB(0.0f, 0.0f, 1.0f);
const ColorRGB RGB_White = ColorRGB(1.0f, 1.0f, 1.0f);
const ColorRGB RGB_Yellow = ColorRGB(1.0f, 1.0f, 0.0f);

int worldMap[mapWidth][mapHeight] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
    {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int main(int /*argc*/, char */*argv*/[])
{
    double posX = 22, posY = 12;    //Player's starting location
    double dirX = -1, dirY = 0;     //Player's starting direction
    double planeX = 0, planeY = 0.66;   //Player's camera plane

    double time = 0;    //time of the current frame
    double oldTime = 0;     //time of the previous frame

    while (!glfwWindowShouldClose(engine.window)) {
        engine.beginFrame();

        for (int x = 0; x < w; x++) {
            // Calculate ray position and direction
            double cameraX = 2 * x / double(w) - 1;
            double rayDirX = dirX + planeX * cameraX;
            double rayDirY = dirY + planeY * cameraX;

            // which box of the map we're in 
            int mapX = int(posX);
            int mapY = int(posY);

            //length of ray from current position to next x or y-side
            double sideDistX;
            double sideDistY;

            //length of ray from one x or y-side to next x or y-side
            double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
            double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);
            double perpWallDist;    //used to calc the len of the ray

            //what direction to step in x or y-direction (either +1 or -1)
            int stepX;
            int stepY;

            int hit = 0;    //was there a wall hit?
            int side;   //was a NS or EW wall hit?

            //calculate step and initial sideDist
            if (rayDirX < 0) {
                stepX = -1;
                sideDistX = (posX - mapX) * deltaDistX;
            }
            else {
                stepX = 1;
                sideDistX = (mapX + 1.0 - posX) * deltaDistX;
            }

            if (rayDirY < 0) {
                stepY = -1;
                sideDistY = (posY - mapY) * deltaDistY;
            }
            else {
                stepY = 1;
                sideDistY = (mapY + 1.0 - posY) * deltaDistY;
            }

            //perform DDA
            while (hit == 0) {
                // jump to next map square either in x-dir or y-dir
                if (sideDistX < sideDistY) {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                }
                else {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                }
                // Check if ray has hit a wall
                if (worldMap[mapX][mapY] > 0) hit = 1;
            }

            // Calculate distance projected on camera direction (Euclidean distance would give a fisheye effect)
            if (side == 0) perpWallDist = (sideDistX - deltaDistX);
            else perpWallDist = (sideDistY - deltaDistY);

            //calculate height of line to draw on screen
            int lineHeight = (int)(h / perpWallDist);

            //calculate lowest and highest pixel to fill in current stage
            int drawStart = -lineHeight / 2 + h / 2;
            if (drawStart < 0) drawStart = 0;
            int drawEnd = lineHeight / 2 + h / 2;
            if (drawEnd >= h) drawEnd = h - 1;

            //choose wall color
            ColorRGB color;
            switch (worldMap[mapX][mapY]) {
            case 1: color = RGB_Red; break;
            case 2: color = RGB_Green; break;
            case 3: color = RGB_Blue; break;
            case 4: color = RGB_White; break;
            default: color = RGB_Yellow; break;
            }

            //give x and y sides different brightness
            if (side == 1) { color = color / 2; }

            //draw the pixeld of the stripe as a vertical line
            verLine(x, drawStart, drawEnd, color);
        }

        // timing fro input and FPS counter
        oldTime = time;
        time = getTicks();
        double frameTime = (time - oldTime) / 1000.0;
        std::cout << (1.0 / frameTime) << "\n";
        //replaces redraw()
        glfwSwapBuffers(engine.window);
        glfwPollEvents();
        //replaces cls()
        engine.beginFrame();

        // speed modifiers
        double moveSpeed = frameTime * 5.0;
        double rotSpeed = frameTime * 3.0;

        readKeys();
        //move forward if no wall infront of you
        if (keyDown(SDLK_UP)) {
            if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
            if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
        }
        // move backwards if not wall behind you
        if (keyDown(SDLK_DOWN)) {
            if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
            if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
        }
        //rotate to the right
        if (keyDown(SDLK_RIGHT)) {
            //both camera direction and camera plane must be rotated
            double oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }
        //rotate to the left
        if (keyDown(SDLK_LEFT)) {
            double oldDirX = dirX;
            dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
            dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
            planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
        }     
    }
    return 0;

}

