#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace sf;

const int winWidth = 1280;
const int winHeight = 720;
const int ROWS = 10;
const int COLUMNS = 10;
const int blockWidth = winWidth / ROWS;
const int blockHeight = winHeight / COLUMNS;
const int zomRow = 3;

class Character {
protected:
    Sprite sprite;
    Texture texture;

public:
    Character(const Texture& texture) : sprite() {
        this->texture = texture;
        sprite.setTexture(texture);
        float scaleFactorX = static_cast<float>(blockWidth) / texture.getSize().x;
        float scaleFactorY = static_cast<float>(blockHeight) / texture.getSize().y;
        sprite.setScale(scaleFactorX, scaleFactorY);
    }

    virtual void draw(RenderWindow& window) const {
        window.draw(sprite);
    }

    FloatRect getBounds() const {
        return sprite.getGlobalBounds();
    }
};

struct Grid {
    bool isOccupied = true;
    RectangleShape cell;
    Character* character = nullptr;
};

class Bullet : public Character {
public:
    Bullet(const Texture& texture, const Vector2f& position) : Character(texture) {
        sprite.setPosition(position);
    }
    void update(float deltaTime) {
        sprite.move(500 * deltaTime, 0); // Adjust the speed here
    }
};

class Plant : public Character {
protected:
    Grid cell;
    vector<Bullet> bullets;
    Texture bulletTexture;

public:
    Plant(const Texture& texture, const Texture& bulletTexture) : Character(texture), bulletTexture(bulletTexture), cell() {}

    void setPosition(int x, int y) {
        sprite.setPosition(x * blockWidth, y * blockHeight);
        cell.isOccupied = false;
        cell.character = this;
    }

    void draw(RenderWindow& window) const override {
        window.draw(sprite);
        for (size_t i = 0; i < bullets.size(); i++) {
            bullets[i].draw(window);
        }
    }

    void shoot(const Vector2f& targetPosition) {
        Bullet bullet(bulletTexture, Vector2f(sprite.getPosition().x + sprite.getGlobalBounds().width / 2, sprite.getPosition().y));
        bullets.push_back(bullet);
    }

    void updateBullets(float deltaTime) {
        for (size_t i = 0; i < bullets.size(); i++) {
            bullets[i].update(deltaTime);
        }
    }

    void drawBullets(RenderWindow& window) const {
        for (size_t i = 0; i < bullets.size(); i++) {
            bullets[i].draw(window);
        }
    }
};

class Zombie : public Character {
protected:
    int health;
    float speed;

public:
    Zombie(const Texture& texture, int health, float speed) : Character(texture), health(health), speed(speed) {
        sprite.setPosition(blockWidth * 9, blockHeight * zomRow);
    }

    void update(float deltaTime) {
        float movement = speed * deltaTime;
        sprite.move(-movement, 0); // Move towards the left
    }

    void takeDamage(int damage) {
        health -= damage;
    }

    bool isDead() const {
        return health <= 0;
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
    if (!plantTexture.loadFromFile("plant.png")) {
        return 1;
    }

    Texture zombieTexture;
    if (!zombieTexture.loadFromFile("zombie.png")) {
        return 1;
    }

    Texture bulletTexture;
    if (!bulletTexture.loadFromFile("shooter.png")) {
        return 1;
    }

    vector<Plant> AllPlants;//changed 
    Vector2f CellSize;
    CellSize.x = blockWidth;
    CellSize.y = blockHeight;
    Grid Cells[10][10];
    RectangleShape SampleCell;
    SampleCell.setSize(CellSize);

    Zombie zombie(zombieTexture, 5, 80.0f); // Increased zombie speed

    vector<Bullet> bullets;

    Clock clock;

    while (window.isOpen()) {
        float timeTaken = clock.restart().asSeconds();
        Event event;
        window.clear();
        for (int y = 0; y < ROWS; y++) {
            for (int x = 0; x < COLUMNS; x++) {
                Cells[y][x].cell = SampleCell;
                Cells[y][x].cell.setPosition(x * blockWidth, y * blockHeight);
                if (x == 0) {
                    Cells[y][x].cell.setFillColor(brown);
                }
                else if (x == COLUMNS - 1) {
                    Cells[y][x].cell.setFillColor(red);
                }
                else if (y == 0 || y == ROWS - 1) {
                    Cells[y][x].cell.setFillColor(darkGreen);
                }
                else {
                    if ((x + y) % 2 == 0) {
                        Cells[y][x].cell.setFillColor(lightGreen);
                    }
                    else {
                        Cells[y][x].cell.setFillColor(leafGreen);
                    }
                }
                window.draw(Cells[y][x].cell);
            }
        }
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            else if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                int x = event.mouseButton.x / blockWidth;
                int y = event.mouseButton.y / blockHeight;
                if (x >= 0 && x < ROWS && y >= 0 && y < COLUMNS) {
                    Plant TempPlant(plantTexture, bulletTexture);
                    TempPlant.setPosition(x, y);
                    AllPlants.push_back(TempPlant);
                    //cout << y << x << endl;
                    bullets.push_back(Bullet(bulletTexture, Cells[y][x].cell.getPosition()));
                }
            }
        }
        for (const auto& plant : AllPlants) {
            plant.draw(window);
        }

        for (size_t i = 0; i < bullets.size(); i++) {
            bullets[i].update(timeTaken);
            bullets[i].draw(window);
        }

        zombie.update(timeTaken);
        zombie.draw(window);

        //hit-dishkiyaon
        for (auto it = bullets.begin(); it != bullets.end();) {
            if (zombie.getBounds().intersects(it->getBounds())) {
                it = bullets.erase(it);
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

    return 0;
}
