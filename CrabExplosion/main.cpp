#include "GD_BaseEngine.h"
#include "GD_GameResource.h"
#include "GD_GameObject.h"
#include "ACO_Background.h"
#include "GD_TextObject.h"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

#define TILE_SIZE 50.0f
#define PLAYER_IDX 3
#define BONUS1_IDX 4
#define BONUS2_IDX 5
#define GAME_GRID_SIZE 10
#define MENU_HEIGHT 2

class Object2D : public GD_GameObject {
private:
    sf::Sprite* sprite;
    int x, y;
public:
    Object2D(std::string id, std::string spriteType) :
        GD_GameObject(id) {
        GD_GameResource* res = GD_GameResource::createInstance();
        sf::IntRect rect = (*res->getAtlas())[spriteType];
        sprite = new sf::Sprite(*res->getTexture(), rect);
        sprite->setScale({ TILE_SIZE / rect.size.x, TILE_SIZE / rect.size.y });
        x = 0; y = 0;
    }
    ~Object2D() { delete sprite; }
    void init() { }
    void update(sf::Time deltaTime) { }
    void draw(sf::RenderWindow* window) {
        window->draw(*sprite);
    }
    void setPosition(int xV, int yV) {
        x = xV; y = yV;
        sprite->setPosition({ TILE_SIZE * x, TILE_SIZE * y });
    }
    void move(int xV, int yV) {
        x += xV; y += yV;
        sprite->setPosition({ TILE_SIZE * x, TILE_SIZE * y });
        std::cout << "\nCurrent position is: " << TILE_SIZE * x << ", " << TILE_SIZE * y;
    }
    int getX() { return x; }
    int getY() { return y; }
};

class Game : public GD_BaseEngine {
private:
    std::vector<GD_GameObject*> gameObjects;
    int score;
public:
    void init() {
        std::srand(std::time(nullptr));
        score = 0;

        gameObjects.push_back(new ACO_Background("bg1", "grass",
            TILE_SIZE, GAME_GRID_SIZE, GAME_GRID_SIZE, 0, 0));
        gameObjects.push_back(new ACO_Background("bg2", "tile",
            TILE_SIZE, MENU_HEIGHT, GAME_GRID_SIZE, 0, 500));
        gameObjects.push_back(new GD_TextObject("score",
            "SCORE: 0", 20, 5 + TILE_SIZE * GAME_GRID_SIZE));
        gameObjects.push_back(new Object2D("player", "crab"));

        gameObjects.push_back(new Object2D("bonus1", "bonus1"));
        gameObjects.push_back(new Object2D("bonus2", "bonus2"));

        setRandomPosition((Object2D*)gameObjects[BONUS1_IDX]);
        setRandomPosition((Object2D*)gameObjects[BONUS2_IDX]);

        for (int i = 0; i < gameObjects.size(); i++)
            gameObjects[i]->init();
    }

    void setRandomPosition(Object2D* obj) {
        int x = std::rand() % GAME_GRID_SIZE;
        int y = std::rand() % GAME_GRID_SIZE;
        obj->setPosition(x, y);
    }

    void update(sf::Time deltaTime) {
        Object2D* player = (Object2D*)gameObjects[PLAYER_IDX];
        Object2D* bonus1 = (Object2D*)gameObjects[BONUS1_IDX];
        Object2D* bonus2 = (Object2D*)gameObjects[BONUS2_IDX];

        if (player->getX() == bonus1->getX() && player->getY() == bonus1->getY()) {
            score += 1;
            setRandomPosition(bonus1);
            ((GD_TextObject*)gameObjects[2])->setText("SCORE: " + std::to_string(score));
        }

        if (player->getX() == bonus2->getX() && player->getY() == bonus2->getY()) {
            score += 1;
            setRandomPosition(bonus2);
            ((GD_TextObject*)gameObjects[2])->setText("SCORE: " + std::to_string(score));
        }
    }

    void draw(sf::RenderWindow* window) {
        for (int i = 0; i < gameObjects.size(); i++)
            gameObjects[i]->draw(window);
    }

    void keyPressTrigger(sf::Keyboard::Scan keyCode) {
        Object2D* player = (Object2D*)gameObjects[PLAYER_IDX];

        if (keyCode == sf::Keyboard::Scan::Up)
            player->move(0, -1);
        else if (keyCode == sf::Keyboard::Scan::Down)
            player->move(0, 1);
        else if (keyCode == sf::Keyboard::Scan::Left)
            player->move(-1, 0);
        else if (keyCode == sf::Keyboard::Scan::Right)
            player->move(1, 0);
    }

    void keyReleaseTrigger(sf::Keyboard::Scan keyCode) { }

    ~Game() {
        GD_GameResource* resources = GD_GameResource::createInstance();
        delete resources;
        for (int i = 0; i < gameObjects.size(); i++)
            delete gameObjects[i];
    }
};

int main() {
    Game game;
    game.run("Hands-on 3: Collectable",
        TILE_SIZE * GAME_GRID_SIZE,
        TILE_SIZE * (GAME_GRID_SIZE + MENU_HEIGHT));
    return 0;
}