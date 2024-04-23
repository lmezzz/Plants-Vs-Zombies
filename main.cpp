#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

using namespace sf;
using namespace std;

const int winWidth = 1280;
const int winHeight = 720;
const int ROWS = 10;
const int COLUMNS = 10;
const int blokWidth = winWidth / ROWS;
const int blokHeight = winHeight / COLUMNS;
const int zomRow = 3;
const int houseColumn = 0;

class Plant;
class Zombie;

struct Grid{
    RectangleShape Cell;
    bool IsOccupied;
    Plant *plant;
    Zombie *zombie;
};

class Plant {
private:
    Texture texture;
    Texture shooterTexture;
    Sprite sprite;
    Sprite shooterSprite;
    bool isAvailable;
    bool hasShot; 
    float scaleFactor;
    float shooterScaleFactor;
    int gridX;
    int gridY;
    int score;
public:
    Plant(const Texture& texture, const Texture& shooterTexture) : texture(texture), shooterTexture(shooterTexture), sprite(), shooterSprite(), isAvailable(true), hasShot(false), scaleFactor(1.0f), shooterScaleFactor(1.0f), score(0) {
        sprite.setTexture(texture);
        shooterSprite.setTexture(shooterTexture);
    }
    void setPosition(int x, int y) {
        sprite.setPosition(x * blokWidth, y * blokHeight);
        gridX = x;
        gridY = y;
        isAvailable = false;

        Vector2u Frame = texture.getSize(); 
        int framWidth = (Frame.x)/6;
        IntRect shooter_plant(0 ,0 , framWidth , Frame.y*1.05);
        sprite.setTexture(texture);
        sprite.setTextureRect(shooter_plant);

        float scaleFactorX = static_cast<float>(blokWidth) / texture.getSize().x;
        float scaleFactorY = static_cast<float>(blokHeight) / texture.getSize().y;
        if (scaleFactorX < scaleFactorY) {
            scaleFactor = scaleFactorX;
        }
        else {
            scaleFactor = scaleFactorY;
        }
        sprite.setScale(scaleFactor, scaleFactor);
        shooterSprite.setTexture(shooterTexture);
        shooterSprite.setPosition(sprite.getPosition());
    }

    void draw(RenderWindow& window) const {
        if (!isAvailable) {
            window.draw(sprite);
            if (hasShot)
                window.draw(shooterSprite);
        }
    }

    bool available() const {
        return isAvailable;
    }
    void setAvailable(bool value) {
        isAvailable = value;
    }
    int getGridX() const {
        return gridX;
    }
    int getGridY() const {
        return gridY;
    }
    int getScore() const {
        return score;
    }
    void increaseScore(int value) {
        score += value;
    }
    bool hasShotZombie() const {
        return hasShot;
    }
    void shoot() {
        hasShot = true;
    }
    void resetShot() {
        hasShot = false;
    }
    FloatRect getShooterBounds() const {
        Vector2f position = shooterSprite.getPosition();
        Vector2f scale = shooterSprite.getScale();
        Vector2u textureSize = shooterSprite.getTexture()->getSize();
        float sizeX = static_cast<float>(textureSize.x) * scale.x;
        float sizeY = static_cast<float>(textureSize.y) * scale.y;
        return FloatRect(position.x, position.y, sizeX, sizeY);
    }
};

class Zombie {
private:
    Texture texture;
    Sprite sprite;
    int health;
    float speed;
public:

    Zombie(const Texture& texture, int health, float speed) : texture(texture), sprite(), health(health), speed(speed) {
        sprite.setTexture(texture);
        float scaleFactorX = static_cast<float>(blokWidth) / texture.getSize().x;
        float scaleFactorY = static_cast<float>(blokHeight) / texture.getSize().y;    // Scale down to fit in a 120x72 box
        sprite.setScale(scaleFactorX, scaleFactorY);
        sprite.setPosition(blokWidth * 9, blokHeight * (zomRow));   //place it on the right side
    }
    
    void update(float deltaTime) {
        float movement = speed * deltaTime ;
        sprite.move(-movement, 0); // Move towards the left
    }

    void draw(RenderWindow& window) const {
        window.draw(sprite);
    }

    void takeDamage(int damage) {
        health -= damage;
    }

    bool isDead() const {
        return health <= 0;
    }

    FloatRect getBounds() const {
        Vector2f position = sprite.getPosition();
        Vector2f scale = sprite.getScale();
        Vector2u textureSize = sprite.getTexture()->getSize();
        float sizeX = static_cast<float>(textureSize.x) * scale.x;
        float sizeY = static_cast<float>(textureSize.y) * scale.y;
        return FloatRect(position.x, position.y, sizeX, sizeY);
    }
};

class ShooterBall {
private:
    Texture texture;
    Sprite sprite;
public:
    ShooterBall(const Texture& texture, const Vector2f& position) : texture(texture), sprite() {
        sprite.setTexture(texture);
        float scaleFactorX = static_cast<float>(blokWidth) / texture.getSize().x;
        float scaleFactorY = static_cast<float>(blokHeight) / texture.getSize().y;    // Scale down to fit in a 120x72 box
        sprite.setScale(scaleFactorX, scaleFactorY);
        sprite.setPosition(position);
    }

    void update(float deltaTime) {
        sprite.move(100 * deltaTime, 0); // Adjust the velocity here
    }

    void draw(RenderWindow& window) const {
        window.draw(sprite);
    }

    FloatRect getBounds() const {
        Vector2f position = sprite.getPosition();
        Vector2f scale = sprite.getScale();
        Vector2u textureSize = sprite.getTexture()->getSize();
        float sizeX = static_cast<float>(textureSize.x) * scale.x;
        float sizeY = static_cast<float>(textureSize.y) * scale.y;
        return FloatRect(position.x, position.y, sizeX, sizeY);
    }
};

int main() {
    RenderWindow window(VideoMode(winWidth, winHeight), "Plants vs Zombies");
    Color brown(139, 69, 19);           // Brown
    Color darkGreen(0, 100, 0);         // Dark Green
    Color red(255, 0, 0);               // Red
    Color lightGreen(144, 238, 144);    // Light Green
    Color leafGreen(0, 128, 0);         // Leaf Green

    Texture plantTexture;
    if (!plantTexture.loadFromFile("Images\\plant.png")) {
        return 1;
    }

    Texture zombieTexture;
    if (!zombieTexture.loadFromFile("Images\\zombie.png")) {
        return 1;
    }

    Texture shooterTexture;
    if (!shooterTexture.loadFromFile("Images\\bullet.png")) {
        return 1;
    }

    vector<Plant> AllPlants;//changed 
    Vector2f CellSize;
    CellSize.x = blokWidth;
    CellSize.y = blokHeight;
    Grid Cells[10][10];
    RectangleShape SampleCell;
    SampleCell.setSize(CellSize);

    Zombie zombie(zombieTexture, 5, 80.0f); // Increased zombie speed

    vector<ShooterBall> shooterBalls;

    Clock clock;

    while (window.isOpen()) {
        float timeTaken = clock.restart().asSeconds();

        Event event;
        

        window.clear();

        for (int y = 0; y < ROWS; y++) {
            for (int x = 0; x < COLUMNS; x++) {
                Cells[y][x].Cell = SampleCell;
                Cells[y][x].Cell.setPosition(x * blokWidth, y * blokHeight);
                if (x == 0) {
                    Cells[y][x].Cell.setFillColor(brown);
                }
                else if (x == COLUMNS - 1) {
                    Cells[y][x].Cell.setFillColor(red);
                }
                else if (y == 0 || y == ROWS - 1) {
                    Cells[y][x].Cell.setFillColor(darkGreen);
                }
                else {
                    if ((x + y) % 2 == 0) {
                        Cells[y][x].Cell.setFillColor(lightGreen);
                    }
                    else {
                        Cells[y][x].Cell.setFillColor(leafGreen);
                    }
                }
                window.draw(Cells[y][x].Cell);
            }
        }
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            else if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                int x = event.mouseButton.x / blokWidth;
                int y = event.mouseButton.y / blokHeight;
                if (x >= 0 && x < ROWS && y >= 0 && y < COLUMNS) {
                    Plant TempPlant(plantTexture , shooterTexture);
                    TempPlant.setPosition(x , y);
                    AllPlants.push_back(TempPlant);
                    cout<<y<<x<<endl;
                    // AllPlants.back().draw(window);
                    // Spawn 
                    shooterBalls.push_back(ShooterBall(shooterTexture, Cells[y][x].Cell.getPosition()));
                }
            }
        }
        for (const auto& plant : AllPlants) {
        plant.draw(window);
        }

        for (size_t i = 0; i < shooterBalls.size(); i++) {
            shooterBalls[i].update(timeTaken);
            shooterBalls[i].draw(window);
        }

        zombie.update(timeTaken);
        zombie.draw(window);

        //hit-dishkiyaon
        for (auto it = shooterBalls.begin(); it != shooterBalls.end();) {
            if (zombie.getBounds().intersects(it->getBounds())) {
                it = shooterBalls.erase(it);
                zombie.takeDamage(1);
                if (zombie.isDead()) {
                    // grid[0][0].increaseScore(50);
                    zombie = Zombie(zombieTexture, 5, 80.0f); // Respawn with increased speed
                }
            }
            else {
                ++it;
            }
        }
        

        window.display();
    }

}
