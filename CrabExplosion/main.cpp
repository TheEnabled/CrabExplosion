#include "GD_BaseEngine.h"
#include "GD_GameResource.h"
#include <iostream>
#include <vector>

#define TILE_SIZE 50.0f
#define PLAYER_IDX 0
#define BOOM_IDX 1
#define BOOM_GS 8
#define GAME_GRID_SIZE 10
#define MENU_HEIGHT 2

class Game : public GD_BaseEngine {
private:
    std::vector<sf::Sprite> gameObjects;
    std::vector<sf::Sprite> gameBackground;
    std::vector<sf::Text> menuText;
    GD_GameResource* resources;

    // Explosion animation
    sf::Sprite explosionSprite;
    std::vector<sf::IntRect> boomFrames;
    int currentFrame;
    sf::Time frameTime;
    const sf::Time frameDuration = sf::seconds(0.1f); // Adjust frame duration

public:
    Game() : explosionSprite(*resources->getTexture()) {
        // Initialize resources
        resources = GD_GameResource::createInstance();
        currentFrame = 0;
        frameTime = sf::Time::Zero;
    }

    void scaleSprite(sf::Sprite* sprite, sf::IntRect rect) {
        sprite->setScale({ TILE_SIZE / rect.size.x * 0.99f, TILE_SIZE / rect.size.y * 0.99f });
    }

    void init() override {
        // Initialize game objects (e.g., player, crab)
        gameObjects.push_back(
            sf::Sprite(*resources->getTexture(),
                (*resources->getAtlas())["crab"]));
        gameObjects.back().setPosition({ 0.f, 0.f });
        scaleSprite(&gameObjects.back(),
            (*resources->getAtlas())["crab"]);

        // Initialize menu text
        menuText.push_back(sf::Text(*resources->getFont()));
        menuText.back().setString("ENEMIES: 3");
        menuText.back().setFillColor(sf::Color::White);
        menuText.back().setPosition({ 20,5 + TILE_SIZE * GAME_GRID_SIZE });

        // Initialize game background tiles
        for (int i = 0; i < GAME_GRID_SIZE; i++) {
            for (int j = 0; j < GAME_GRID_SIZE; j++) {
                gameBackground.push_back(
                    sf::Sprite(*resources->getTexture(),
                        (*resources->getAtlas())["grass"]));
                gameBackground.back().setPosition({ TILE_SIZE * i, TILE_SIZE * j });
                scaleSprite(&gameBackground.back(),
                    (*resources->getAtlas())["grass"]);
            }
        }

        // Debug: Check if "boom" exists in the atlas
        auto atlas = (*resources->getAtlas());
        std::cout << "Atlas contains the following keys:" << std::endl;
        for (const auto& entry : atlas) {
            std::cout << entry.first << std::endl; // Print out all keys in the atlas
        }

        // Initialize explosion animation frames from the "boom" section in the atlas
        if (atlas.find("boom") != atlas.end()) {
            sf::IntRect boomRect = atlas["boom"];
            int frameWidth = boomRect.size.x / BOOM_GS; // Number of frames in x-axis
            int frameHeight = boomRect.size.y; // Assuming 1 row for explosion

            // Debug: print frame dimensions
            std::cout << "Frame Width: " << frameWidth << ", Frame Height: " << frameHeight << std::endl;

            // Add frames for animation
            for (int i = 0; i < BOOM_GS; i++) {
                // Create new sf::IntRect for each frame
                boomFrames.push_back(sf::IntRect(
                    boomRect.left + (frameWidth * i), // Adjust x position for each frame
                    boomRect.top, // y position stays the same (since it’s one row)
                    frameWidth, frameHeight
                ));
                // Debug: print each frame's coordinates
                std::cout << "Frame " << i << ": (" << boomFrames[i].left << ", " << boomFrames[i].top << ", "
                    << boomFrames[i].size.x << ", " << boomFrames[i].size.y << ")" << std::endl;
            }

            std::cout << "Boom Atlas Parsed Successfully!" << std::endl; // Debug message
        }
        else {
            std::cout << "Boom Atlas Failed to Parse!" << std::endl; // Debug message if "boom" is not found
        }

        // Set up explosion sprite
        explosionSprite.setTexture(*resources->getTexture());
        if (!boomFrames.empty()) {
            explosionSprite.setTextureRect(boomFrames[0]); // Set first frame
            explosionSprite.setPosition({ TILE_SIZE * 2, TILE_SIZE * 2 }); // Position of the explosion
        }
    }



    void update(sf::Time deltaTime) override {
        // Update the explosion animation
        frameTime += deltaTime;
        if (frameTime >= frameDuration) {
            frameTime -= frameDuration;
            currentFrame = (currentFrame + 1) % boomFrames.size(); // Loop through frames
            explosionSprite.setTextureRect(boomFrames[currentFrame]);
        }
    }

    void draw(sf::RenderWindow* window) override {
        // Draw background, game objects, and text
        for (int i = 0; i < gameBackground.size(); i++)
            window->draw(gameBackground[i]);
        for (int i = 0; i < gameObjects.size(); i++)
            window->draw(gameObjects[i]);
        for (int i = 0; i < menuText.size(); i++)
            window->draw(menuText[i]);

        // Draw explosion sprite (animate it)
        window->draw(explosionSprite); // Ensure this is visible
    }

    void keyPressTrigger(sf::Keyboard::Scan keyCode) override {
        if (keyCode == sf::Keyboard::Scan::Up)
            gameObjects[PLAYER_IDX].move({ 0.0f, -TILE_SIZE });
        else if (keyCode == sf::Keyboard::Scan::Down)
            gameObjects[PLAYER_IDX].move({ 0.0f, TILE_SIZE });
        else if (keyCode == sf::Keyboard::Scan::Left)
            gameObjects[PLAYER_IDX].move({ -TILE_SIZE, 0.0f });
        else if (keyCode == sf::Keyboard::Scan::Right)
            gameObjects[PLAYER_IDX].move({ TILE_SIZE, 0.0f });
    }

    void keyReleaseTrigger(sf::Keyboard::Scan keyCode) override {
        // Handle key release events if necessary
    }

    ~Game() {
        delete resources;
    }
};

int main() {
    Game game;
    game.run("Explosion Animation", TILE_SIZE * GAME_GRID_SIZE, TILE_SIZE * (GAME_GRID_SIZE + MENU_HEIGHT));
    return 0;
}
