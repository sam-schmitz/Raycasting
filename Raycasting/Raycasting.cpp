// Raycasting.cpp 
// By: Sam Schmitz


#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdint>

#define mapWidth 24
#define mapHeight 24
#define screenWidth 640
#define screenHeight 480
#define texWidth 64
#define texHeight 64

using Uint32 = uint32_t;

int rotateDir = 0;
int moveDir = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
    case GLFW_KEY_RIGHT:
        if (action == GLFW_PRESS) {            
            rotateDir -= 1;
        }
        else if (action == GLFW_RELEASE) {
            rotateDir += 1;
        }
        break;
    case GLFW_KEY_LEFT:
        if (action == GLFW_PRESS) {            
            rotateDir += 1;
        }
        else if (action == GLFW_RELEASE) { 
            rotateDir -= 1;
        }
        break;
    case GLFW_KEY_UP: 
        if (action == GLFW_PRESS) {            
            moveDir += 1;
        }
        else if (action == GLFW_RELEASE) {
            moveDir -= 1;
        }
        break;
    case GLFW_KEY_DOWN:
        if (action == GLFW_PRESS) {
            moveDir -= 1;
        }
        else if (action == GLFW_RELEASE){
            moveDir += 1;
        }
        break;
    default:
        break;
    }
}

struct Engine {
    GLFWwindow* window;

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
        glfwSetKeyCallback(window, key_callback);
    }

    void beginFrame() {
        // clear the frame each render loop iteration
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, screenWidth, screenHeight, 0, -1, 1);

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

void verLine(int x, int y1, int y2, const ColorRGB& color) {
    color.apply();

    glBegin(GL_LINES);
    glVertex2i(x, y1);
    glVertex2i(x, y2);
    glEnd();
}

void drawBuffer(const Uint32* pixels) {
    glDrawPixels(screenWidth, screenHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

int worldMap[mapWidth][mapHeight] =
{
  {4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,7,7,7,7,7,7,7,7},
  {4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
  {4,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7},
  {4,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,0,0,0,7},
  {4,0,4,0,0,0,0,5,5,5,5,5,5,5,5,5,7,7,0,7,7,7,7,7},
  {4,0,5,0,0,0,0,5,0,5,0,5,0,5,0,5,7,0,0,0,7,7,7,1},
  {4,0,6,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
  {4,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,7,7,1},
  {4,0,8,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,0,0,0,8},
  {4,0,0,0,0,0,0,5,0,0,0,0,0,0,0,5,7,0,0,0,7,7,7,1},
  {4,0,0,0,0,0,0,5,5,5,5,0,5,5,5,5,7,7,7,7,7,7,7,1},
  {6,6,6,6,6,6,6,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
  {8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
  {6,6,6,6,6,6,0,6,6,6,6,0,6,6,6,6,6,6,6,6,6,6,6,6},
  {4,4,4,4,4,4,0,4,4,4,6,0,6,2,2,2,2,2,2,2,3,3,3,3},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,0,0,0,6,2,0,0,5,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
  {4,0,6,0,6,0,0,0,0,4,6,0,0,0,0,0,5,0,0,0,0,0,0,2},
  {4,0,0,5,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,2,0,2,2},
  {4,0,6,0,6,0,0,0,0,4,6,0,6,2,0,0,5,0,0,2,0,0,0,2},
  {4,0,0,0,0,0,0,0,0,4,6,0,6,2,0,0,0,0,0,2,0,0,0,2},
  {4,4,4,4,4,4,4,4,4,4,1,1,1,2,2,2,2,2,2,3,3,3,3,3}
};

inline Uint32 makeRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return (Uint32(a) << 24) | (Uint32(r) << 16) | (Uint32(g) << 8) | Uint32(b);
}

int main(int /*argc*/, char */*argv*/[])
{
    double posX = 22.0, posY = 12.0;    //Player's starting location
    double dirX = -1.0, dirY = 0.0;     //Player's starting direction
    double planeX = 0.0, planeY = 0.66;   //Player's camera plane

    double time = 0;    //time of the current frame
    double oldTime = 0;     //time of the previous frame

    //Uint32 buffer[screenHeight][screenWidth];   // y-coordinate 1st becuase it works per scanline
    std::vector<Uint32> buffer(screenWidth * screenHeight);

    std::vector<Uint32> texture[8];
    for (int i = 0; i < 8; i++) texture[i].resize(texWidth * texHeight);

    for (int x = 0; x < texWidth; x++) {
        for (int y = 0; y < texHeight; y++) {
            int xorcolor = (x * 256 / texWidth) ^ (y * 256 / texHeight);
            int ycolor = y * 256 / texHeight;
            int xycolor = y * 128 / texHeight + x * 128 / texWidth;            
            //Updated way of making textures
            texture[0][texWidth * y + x] = makeRGBA(254 * (x != y && x != texWidth - y), 0, 0);
            texture[1][texWidth * y + x] = makeRGBA(xycolor, xycolor, xycolor);
            texture[2][texWidth * y + x] = makeRGBA(xycolor, xycolor, 0);
            texture[3][texWidth * y + x] = makeRGBA(xorcolor, xorcolor, xorcolor);
            texture[4][texWidth * y + x] = makeRGBA(0, xorcolor, 0);
            texture[5][texWidth * y + x] = makeRGBA(192 * (x % 16 && y % 16), 0, 0);
            texture[6][texWidth * y + x] = makeRGBA(ycolor, 0, 0);
            texture[7][texWidth * y + x] = makeRGBA(128, 128, 128);
        }
    }        

    while (!glfwWindowShouldClose(engine.window)) {
        engine.beginFrame();

        for (int x = 0; x < screenWidth; x++) {
            // Calculate ray position and direction
            double cameraX = 2 * x / double(screenWidth) - 1;
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
            int lineHeight = (int)(screenHeight / perpWallDist);

            //calculate lowest and highest pixel to fill in current stage
            int drawStart = -lineHeight / 2 + screenHeight / 2;
            if (drawStart < 0) drawStart = 0;
            int drawEnd = lineHeight / 2 + screenHeight / 2;
            if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;

            /*
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
            */

            //texturing calculations
            int texNum = worldMap[mapX][mapY] - 1;

            //calculate value of wallX
            double wallX;   //where exactly the wall was hit
            if (side == 0) {
                wallX = posY + perpWallDist * rayDirY;
            }
            else {
                wallX = posX + perpWallDist * rayDirX;
            }
            wallX -= floor((wallX));

            //x coordinate on the texture
            int texX = int(wallX * double(texWidth));
            if (side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
            if (side == 1 && rayDirY < 0) texX = texWidth - texX - 1;

            double step = 1.0 * texHeight / lineHeight;
            double texPos = (drawStart - screenHeight / 2 + lineHeight / 2) * step;
            for (int y = drawStart; y < drawEnd; y++) {
                int texY = (int)texPos & (texHeight - 1);
                texPos += step;
                Uint32 color = texture[texNum][texHeight * texY + texX];
                if (side == 1) color = (color >> 1) & 8355711;
                buffer[y * screenWidth + x] = color;
            }
        }
        drawBuffer(buffer.data());
        //clear the buffer instead of cls()
        std::fill(buffer.begin(), buffer.end(), 0);
        //for (int y = 0; y < screenHeight; y++) for (int x = 0; x < screenWidth; x++) buffer[y][x] = 0;  
        // timing from input and FPS counter
        oldTime = time;        
        time = glfwGetTime() * 1000.0;
        double frameTime = (time - oldTime) / 1000.0;
        std::cout << (1.0 / frameTime) << "\n";        
        glfwSwapBuffers(engine.window);
        glfwPollEvents();        
        engine.beginFrame();

        // speed modifiers
        double moveSpeed = frameTime * 5.0;
        double rotSpeed = frameTime * 3.0;
              
        //move forward if no wall infront of you
        if (moveDir > 0) {
            if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
            if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
        }
        // move backwards if not wall behind you
        if (moveDir < 0) {
            if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
            if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
        }
        if (rotateDir < 0) {
            //both camera direction and camera plane must be rotated
            double oldDirX = dirX;
            dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
            dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
            double oldPlaneX = planeX;
            planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
            planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
        }
        if (rotateDir > 0) {
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

