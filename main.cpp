#include <iostream>
#include <vector>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <cmath>
#include "circle.h"

constexpr int WINDOW_WIDTH{800};
constexpr int WINDOW_HEIGHT{800};

constexpr float GLOBAL_ANIMATION_SPEED{150.0f};

SDL_Window *window{nullptr};
SDL_Renderer *renderer{nullptr};
SDL_Event currentEvent;
SDL_Color backgroundColor{255, 255, 255, 255};

bool quit{false};

float mouseX{-1.0f}, mouseY{-1.0f};

float displayScale{1.0f};

SDL_FRect myRectangle{100.0f, 100.0f, 200.0f, 100.0f};

Uint64 startTime, endTime, elapsedTime;

struct Rectangle {
    glm::vec2 pos;
    float width;
    float height;
    SDL_Color color;
    glm::vec2 dir;
    float speed;

    void draw(SDL_Renderer* renderer) {
        SDL_FRect tmpRect;
        tmpRect.x = pos.x - width / 2.0f;
        tmpRect.y = pos.y - height / 2.0f;
        tmpRect.w = width;
        tmpRect.h = height;
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &tmpRect);
        SDL_SetRenderDrawColor(renderer, color.r / 2, color.g / 2, color.b / 2, color.a);
        SDL_RenderRect(renderer, &tmpRect);
    }
};

float getRandomAngle() {
    float angle = (rand() % 161 -80);
    return angle * M_PI / 180.0f;
}

bool gameOver = false;
bool gameWon = false;
bool gameStarted = false;
Rectangle r, paddle;
Circle c;

std::vector<Circle> circles;
std::vector<Rectangle> rectangles;

void drawBar(SDL_Renderer* r, float x, float y, float w, float h) {
    SDL_FRect rect = {x, y, w, h};
    SDL_RenderFillRect(r, &rect);
}

void drawVictory(SDL_Renderer* renderer) {
    float pulse = (std::sin(SDL_GetTicks() / 200.0f) + 1.0f) / 2.0f;
    Uint8 red = 255;
    Uint8 green = static_cast<Uint8>(215 + (40 * pulse));
    Uint8 blue = 0;

    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);

    float startX = 120.0f;
    float startY = 350.0f;
    float w = 60.0f;
    float h = 100.0f;
    float th = 10.0f;
    float space = 80.0f;

    float currentX = startX;
    drawBar(renderer, currentX, startY, th, h/2);
    drawBar(renderer, currentX + th, startY + h/2, th, h/2);
    drawBar(renderer, currentX + w - th, startY, th, h/2);
    drawBar(renderer, currentX + w - 2*th, startY + h/2, th, h/2);
    drawBar(renderer, currentX + w/2 - th, startY + h - th, th*2, th);

    currentX += space;
    drawBar(renderer, currentX + w/2 - th/2, startY, th, h);
    drawBar(renderer, currentX + w/4, startY, w/2, th);
    drawBar(renderer, currentX + w/4, startY + h - th, w/2, th);

    currentX += space;
    drawBar(renderer, currentX, startY, th, h);
    drawBar(renderer, currentX, startY, w, th);
    drawBar(renderer, currentX, startY + h - th, w, th);

    currentX += space;
    drawBar(renderer, currentX, startY, w, th);
    drawBar(renderer, currentX + w/2 - th/2, startY, th, h);

    currentX += space;
    drawBar(renderer, currentX, startY, th, h);
    drawBar(renderer, currentX + w - th, startY, th, h);
    drawBar(renderer, currentX, startY, w, th);
    drawBar(renderer, currentX, startY + h - th, w, th);

    currentX += space;
    drawBar(renderer, currentX, startY, th, h);
    drawBar(renderer, currentX, startY, w, th);
    drawBar(renderer, currentX, startY + h/2, w, th);
    drawBar(renderer, currentX + w - th, startY, th, h/2);
    drawBar(renderer, currentX + w/2, startY + h/2, th, h/2);
    drawBar(renderer, currentX + w - th, startY + 3*h/4, th, h/4);

    currentX += space;
    drawBar(renderer, currentX, startY, th, h/2);
    drawBar(renderer, currentX + w - th, startY, th, h/2);
    drawBar(renderer, currentX, startY + h/2, w, th);
    drawBar(renderer, currentX + w/2 - th/2, startY + h/2, th, h/2);
}

void resetGame() {
    rectangles.clear();
    int numBricks = 16;
    int totalWidth = numBricks * r.width;
    int startX = (WINDOW_WIDTH - totalWidth) / 2.0f;

    for (int i = 0; i < numBricks; i++)
    {
        Rectangle tmpRect;
        tmpRect.pos = glm::vec2(startX + i * r.width + r.width / 2.0f, 100.0f);
        tmpRect.width = r.width;
        tmpRect.height = r.height;
        tmpRect.color = r.color;
        rectangles.push_back(tmpRect);
    }

    paddle.color = SDL_Color{0, 0, 255, 255};
    paddle.pos= glm::vec2(WINDOW_WIDTH/2.0, WINDOW_HEIGHT-50.0f);
    c.pos = glm::vec2(paddle.pos.x, paddle.pos.y-paddle.height/2.0f-c.radius);
    c.color = SDL_Color{255, 165, 0, 255};
    c.dir=glm::vec2(0.0f, -1.0f);
    gameOver=false;
    gameWon=false;
    gameStarted=false;
    std::cout << "Game Reset!" << std::endl;
}

void initRectangle() {
    srand(time(NULL));
    r.width = 50.0f;
    r.height = 20.0f;
    r.color = SDL_Color{0, 255, 0, 255};
    r.dir = glm::vec2(0.0f, 0.0f);
    r.speed = 1.0f;

    int numBricks = 16;
    int totalWidth = numBricks * r.width;
    int startX = (WINDOW_WIDTH - totalWidth) / 2.0f;

    for (int i = 0; i < numBricks; i++)
    {
        Rectangle tmpRect;
        tmpRect.pos = glm::vec2(startX + i * r.width + r.width / 2.0f, 100.0f);
        tmpRect.width = r.width;
        tmpRect.height = r.height;
        tmpRect.color = r.color;
        rectangles.push_back(tmpRect);
    }

    paddle.pos= glm::vec2(WINDOW_WIDTH/2.0, WINDOW_HEIGHT-50.0f);
    paddle.width=100.0f;
    paddle.height=20.0f;
    paddle.dir=glm::vec2(0.0f, 0.0f);
    paddle.speed=0.3f;
    paddle.color = SDL_Color{0, 0, 255, 255};

    c.radius = 10.0f;
    c.pos = glm::vec2(paddle.pos.x, paddle.pos.y-paddle.height/2.0f-c.radius);
    c.color = SDL_Color{255, 165, 0, 255};
    c.speed = 2.0f;
    c.dir = glm::vec2(0.0f, -1.0f);
}

bool checkCollision(const Circle& circle, const Rectangle& rect) {
    float closestX=std::max(rect.pos.x-rect.width/2.0f, std::min(circle.pos.x, rect.pos.x+rect.width/2.0f));
    float closestY=std::max(rect.pos.y-rect.height/2.0f, std::min(circle.pos.y, rect.pos.y+rect.height/2.0f));
    float distX=circle.pos.x-closestX;
    float distY=circle.pos.y-closestY;
    float distancesquared = distX*distX+distY*distY;
    return distancesquared<(circle.radius*circle.radius);
}

bool initWindow() {
    bool success{true};

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL initialization failed: %s\n", SDL_GetError());
        success = false;
    } else {
        displayScale = SDL_GetDisplayContentScale(1);

        if (!SDL_CreateWindowAndRenderer(
            "SDL Hello World Example",
            static_cast<int>(displayScale * WINDOW_WIDTH),
            static_cast<int>(displayScale * WINDOW_HEIGHT),
            0,
            &window, &renderer)) {
            SDL_Log("Failed to create window and renderer: %s\n", SDL_GetError());
            success = false;
        } else {
            SDL_SetRenderScale(renderer, displayScale, displayScale);
            SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b,
                                   backgroundColor.a);
            SDL_RenderClear(renderer);
        }
    }

    return success;
}

void processEvents() {
    while (SDL_PollEvent(&currentEvent)) {
        if (currentEvent.type == SDL_EVENT_QUIT) {
            quit = true;
        }
        if (currentEvent.type == SDL_EVENT_KEY_DOWN) {
            switch (currentEvent.key.key) {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_R:
                    if (gameOver || gameWon) resetGame();
                    break;
                case SDLK_K:
                    rectangles.clear();
                    break;
                case SDLK_SPACE:
                    if (!gameStarted && !gameOver && !gameWon) {
                        gameStarted = true;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

void processMovement() {
    const bool* keys = SDL_GetKeyboardState(nullptr);

    if (keys[SDL_SCANCODE_LEFT]) {
        paddle.pos.x -= 0.1f;
        if (paddle.pos.x -paddle.width/2.0f<0) {
            paddle.pos.x=paddle.width/2.0f;
        }
    }

    if (keys[SDL_SCANCODE_RIGHT]) {
        paddle.pos.x += 0.1f;
        if (paddle.pos.x+paddle.width/2.0f>WINDOW_WIDTH) {
            paddle.pos.x=WINDOW_WIDTH-paddle.width/2.0f;
        }
    }

    if (!gameStarted) {
        c.pos.x = paddle.pos.x;
    }
}

void drawFrame() {
    if (gameWon) {
        SDL_SetRenderDrawColor(renderer, 230, 255, 230, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    }
    SDL_RenderClear(renderer);

    if (gameWon) {
        drawVictory(renderer);
    } else {
        c.draw(renderer);
        paddle.draw(renderer);
        for (int i=0; i<rectangles.size(); i++) {
            rectangles[i].draw(renderer);
        }
    }

    SDL_RenderPresent(renderer);
}

void animate() {
    if (gameOver) return;
    if (gameWon) return;
    if (!gameStarted) return;

    c.pos += c.speed * (elapsedTime / 1000.0f) * GLOBAL_ANIMATION_SPEED * c.dir;

    if (checkCollision(c, paddle)) {
        c.pos.y = paddle.pos.y - paddle.height/2.0f - c.radius;
        c.dir.y = -c.dir.y;
    }

    if (c.pos.y - c.radius/2.0f > WINDOW_HEIGHT) {
        c.pos = glm::vec2(paddle.pos.x, paddle.pos.y - paddle.height/2.0f - c.radius);
        c.dir = glm::vec2(0.0f, -1.0f);
        paddle.color = SDL_Color{255, 0, 0, 255};
        gameOver = true;
    }

    if (c.pos.x - c.radius < 0) {
        c.pos.x = c.radius;
        c.dir.x = -c.dir.x;
    }
    if (c.pos.x + c.radius > WINDOW_WIDTH) {
        c.pos.x = WINDOW_WIDTH - c.radius;
        c.dir.x = -c.dir.x;
    }
    if (c.pos.y - c.radius < 0) {
        c.pos.y = c.radius;
        c.dir.y = -c.dir.y;
    }

    for (int i = 0; i < rectangles.size(); i++) {
        if (checkCollision(c, rectangles[i])) {
            rectangles.erase(rectangles.begin() + i);
            float angle = getRandomAngle();
            c.dir.x = glm::sin(angle);
            c.dir.y = glm::cos(angle);
            c.dir = glm::normalize(c.dir);
            break;
        }
    }

    if (rectangles.empty() && !gameWon) {
        gameWon = true;
        paddle.color = SDL_Color{0, 255, 0, 255};
        std::cout << "YOU WON!" << std::endl;
    }
}

void cleanup() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

int main() {
    if (!initWindow()) {
        std::cout << "Failed to initialize" << std::endl;
        return -1;
    }

    initRectangle();

    SDL_zero(currentEvent);

    while (!quit) {
        startTime = SDL_GetTicks();
        processEvents();
        processMovement();
        animate();
        drawFrame();
        endTime = SDL_GetTicks();
        elapsedTime = endTime - startTime;
    }

    cleanup();
    return 0;
}