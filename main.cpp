#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <random>
#include<ctime>
#include<chrono>

using namespace std;
using namespace sf;

const int winWidth = 1280;
const int winHeight = 720;
const int ROWS = 10;
const int COLUMNS = 10;
const int blockWidth = winWidth / ROWS;
const int blockHeight = winHeight / COLUMNS;
const int zomRow = 3;

class Character
{
protected:
    Sprite sprite;
    Texture texture;

public:
    Character(const Texture &texture) : sprite()
    {
        this->texture = texture;
        sprite.setTexture(texture);
        float scaleFactorX = static_cast<float>(blockWidth) / texture.getSize().x;
        float scaleFactorY = static_cast<float>(blockHeight) / texture.getSize().y;
        sprite.setScale(scaleFactorX * 0.6, scaleFactorY * 0.6);
    }

    virtual void draw(RenderWindow &window) const
    {
        window.draw(sprite);
    }

    FloatRect getBounds() const
    {
        return sprite.getGlobalBounds();
    }
};

class Plant;

struct Grid
{
    bool isOccupied = false;
    RectangleShape cell;
    Plant *plant = nullptr;
};

class Bullet : public Character
{
public:
    Bullet(const Texture &texture, const Vector2f &position) : Character(texture)
    {
        sprite.setPosition(position);
    }
    void update(float deltaTime)
    {
        sprite.move(50 * deltaTime, 0); // Adjust the speed here
    }
};

class Plant : public Character
{
protected:
    vector<Bullet> bullets;
    Texture bulletTexture;

public:
    Plant(const Texture &texture, const Texture &bulletTexture) : Character(texture), bulletTexture(bulletTexture) {}

    void setPosition(int x, int y , Grid& cell)
    {
        sprite.setPosition(x * blockWidth, y * blockHeight);
        cell.isOccupied = true;
        cell.plant = this;
    }

    void draw(RenderWindow &window) const override
    {
        window.draw(sprite);
        for (size_t i = 0; i < bullets.size(); i++)
        {
            bullets[i].draw(window);
        }
    }

    void shoot(const Vector2f &targetPosition)
    {
        Bullet bullet(bulletTexture, Vector2f(sprite.getPosition().x + sprite.getGlobalBounds().width / 2, sprite.getPosition().y));
        bullets.push_back(bullet);
    }

    void updateBullets(float deltaTime)
    {
        for (size_t i = 0; i < bullets.size(); i++)
        {
            bullets[i].update(deltaTime);
        }
    }

    void drawBullets(RenderWindow &window) const
    {
        for (size_t i = 0; i < bullets.size(); i++)
        {
            bullets[i].draw(window);
        }
    }
};

class Zombie : public Character
{
protected:
    int health;
    float speed;

public:
    Zombie(const Texture &texture, int health, float speed ,int row) : Character(texture), health(health), speed(speed)
    {
        sprite.setPosition(blockWidth * 9, blockHeight * row);
    }

    void update(float deltaTime)
    {
        float movement = speed * deltaTime;
        sprite.move(-movement, 0); // Move towards the left
    }

    void takeDamage(int damage)
    {
        health -= damage;
    }

    bool isDead() const
    {
        return health <= 0;
    }
};

int main()
{
    RenderWindow window(VideoMode(winWidth, winHeight), "Plants vs Zombies");
    Color brown(139, 69, 19);        // Brown
    Color darkGreen(0, 100, 0);      // Dark Green
    Color red(255, 0, 0);            // Red
    Color lightGreen(144, 238, 144); // Light Green
    Color leafGreen(0, 128, 0);      // Leaf Green

    Texture plantTexture;
    if (!plantTexture.loadFromFile("Images\\plant.png"))
    {
        return 1;
    }

    Texture zombieTexture;
    if (!zombieTexture.loadFromFile("Images\\zombie.png"))
    {
        return 1;
    }

    Texture bulletTexture;
    if (!bulletTexture.loadFromFile("Images\\bullet.png"))
    {
        return 1;
    }

    vector<Plant> AllPlants; // changed
    vector<Plant> tempPlants;

    Vector2f CellSize;
    CellSize.x = blockWidth;
    CellSize.y = blockHeight;

    Grid Cells[10][10];
    RectangleShape SampleCell;
    SampleCell.setSize(CellSize);

    vector<Zombie> AllZombies;

    vector<Bullet> bullets;

    Clock clock;
    Time elapsed = Time::Zero;
    Time Interval = seconds(0.0009);

    while (window.isOpen())
    {
        float timeTaken = clock.restart().asSeconds();
        elapsed += clock.restart();
        Event event;
        window.clear();
        for (int y = 0; y < ROWS; y++)
        {
            for (int x = 0; x < COLUMNS; x++)
            {
                Cells[y][x].cell = SampleCell;
                Cells[y][x].cell.setPosition(x * blockWidth, y * blockHeight);
                if (x == 0)
                {
                    Cells[y][x].cell.setFillColor(brown);
                }
                else if (x == COLUMNS - 1)
                {
                    Cells[y][x].cell.setFillColor(red);
                }
                else if (y == 0 || y == ROWS - 1)
                {
                    Cells[y][x].cell.setFillColor(darkGreen);
                }
                else
                {
                    if ((x + y) % 2 == 0)
                    {
                        Cells[y][x].cell.setFillColor(lightGreen);
                    }
                    else
                    {
                        Cells[y][x].cell.setFillColor(leafGreen);
                    }
                }
                window.draw(Cells[y][x].cell);
            }
        }
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            else if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left)
            {
                int x = event.mouseButton.x / blockWidth;
                int y = event.mouseButton.y / blockHeight;
                if (x == 0 && (y >= 0 && y < 3))
                {
                    if (y == 0)
                    {
                        Plant TempPlant(plantTexture, bulletTexture);
                        tempPlants.push_back(TempPlant);
                    }
                    if (y == 1)
                    {
                        Plant TempPlant(plantTexture, bulletTexture);
                        tempPlants.push_back(TempPlant);
                    }
                    if (y == 2)
                    {
                        Plant TempPlant(plantTexture, bulletTexture);
                        tempPlants.push_back(TempPlant);
                    }
                }

                else if (x >= 1 && x < ROWS - 1 && y >= 1 && y < COLUMNS - 1 && (!Cells[x][y].isOccupied))
                {
                    if (!tempPlants.empty())
                    {
                        AllPlants.push_back(tempPlants.back());
                        AllPlants.back().setPosition(x , y ,Cells[x][y]);
                        tempPlants.clear();
                    }
                }
            }
        }

        if (!AllPlants.empty())
        {
            for (const auto &plant : AllPlants)
            {
                plant.draw(window);
            }
        }
        // cout<<elapsed.asSeconds();
        // cout<<Interval.asSeconds();
        // cout<<timeTaken;
        
        if (AllZombies.size() < 20 && elapsed >= Interval)
        {
            mt19937 gen(chrono::high_resolution_clock::now().time_since_epoch().count());


            uniform_int_distribution<int> distrib(1, 8);
            int pos = distrib(gen); // Generate a new random number between 1 and 8

            Zombie tempZombie(zombieTexture , 5 , 30.0 , pos);
            AllZombies.push_back(tempZombie);
            elapsed = Time::Zero;
        }
        

        for (size_t i = 0; i < bullets.size(); i++)
        {
            bullets[i].update(timeTaken);
            bullets[i].draw(window);
        }
        for (Zombie &zombie : AllZombies )
        {
            zombie.update(timeTaken);
            zombie.draw(window);
        }
        

        // zombie.update(timeTaken);
        // zombie.draw(window);

        // hit-dishkiyaon
        // for (auto it = bullets.begin(); it != bullets.end();)
        // {
        //     if (zombie.getBounds().intersects(it->getBounds()))
        //     {
        //         it = bullets.erase(it);
        //         zombie.takeDamage(1);
        //         if (zombie.isDead())
        //         {
        //             // grid[0][0].increaseScore(50);
        //             zombie = Zombie(zombieTexture, 5, 80.0f); // Respawn with increased speed
        //         }
        //     }
        //     else
        //     {
        //         ++it;
        //     }
        // }
        window.display();
    }

    return 0;
}
