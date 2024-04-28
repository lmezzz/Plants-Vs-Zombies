#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>

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
    static vector<Plant> AllPlants;
    vector<Bullet> bullets;
    Texture bulletTexture;
    int health;

public:
    Plant(const Texture &texture, const Texture &bulletTexture) : Character(texture), bulletTexture(bulletTexture) {}

    void setPosition(int x, int y, Grid (&cell)[10][10])
    {
        sprite.setPosition(x * blockWidth, y * blockHeight);
        cell[x][y].isOccupied = true;
        cell[x][y].plant = this;
        cout<<"placed";
        health = 6;
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
    void TakeDamage()
    {
        health--;
    }
    bool isdead()
    {
        if (health <= 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    static void PlacePlant(Plant plant , int x , int y ,Grid (&Cell)[10][10]) {
        AllPlants.push_back(plant);
        AllPlants.back().setPosition(x, y, Cell);
    }
    static bool isEmpty(){
        return AllPlants.empty();
    }
    static void DrawAllPlants(RenderWindow &window){
        for (const auto &plant : Plant::AllPlants)
        {
            plant.draw(window);
        }
    }
    static void RemovePlant(Plant removablePlant){
        for (auto it = AllPlants.begin(); it != AllPlants.end(); ++it)
        {
            if (it->sprite.getPosition() == removablePlant.sprite.getPosition() )
            {
                AllPlants.erase(it);
                cout<<"erased";
                break;
            }
            
        }   
    }
};

class Zombie : public Character
{
protected:
    static vector<Zombie> AllZombies;
    int health;
    float speed;

public:
    Zombie(const Texture &texture, int health, float speed) : Character(texture), health(health), speed(speed)
    {
        
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
    void PlantDetector(Grid (&Cells)[10][10])
    {
        int x = sprite.getPosition().x / blockWidth;
        int y = sprite.getPosition().y / blockHeight;
        if (Cells[x][y].isOccupied)
        {
            if (Cells[x][y].plant->getBounds().contains(sprite.getPosition()))
            {
                speed = 0;
                Cells[x][y].plant->TakeDamage();
                if (Cells[x][y].plant->isdead())
                {
                    Cells[x][y].isOccupied = false;
                    Plant::RemovePlant(*(Cells[x][y].plant));
                    Cells[x][y].plant = nullptr;
                    cout<<"Nice";
                    speed = 30;
                }
            }
        }
    }

    static void PlaceZombie(Zombie zombie) 
    {
        mt19937 gen(chrono::high_resolution_clock::now().time_since_epoch().count());
        uniform_int_distribution<int> distrib(1, 8);
        int pos = distrib(gen); // Generate a new random number between 1 and 8
        zombie.sprite.setPosition(blockWidth * 9, blockHeight * pos);
        AllZombies.push_back(zombie);
    }

    static void UpdateDrawCheck( float timeTaken ,RenderWindow& window , Grid (&Cells)[10][10]){
        for (Zombie &zombie : AllZombies)
        {
            zombie.update(timeTaken);
            zombie.draw(window);
            zombie.PlantDetector(Cells);
        }
    }

    static bool IsEmpty() {
        return AllZombies.empty();
    }

    static int GetSize(){
        return AllZombies.size();
    }

};

void MakeGrid(Grid &Cells, int x, int y, RectangleShape SampleCell, RenderWindow &window)
{
    Color brown(139, 69, 19);        // Brown
    Color darkGreen(0, 100, 0);      // Dark Green
    Color red(255, 0, 0);            // Red
    Color lightGreen(144, 238, 144); // Light Green
    Color leafGreen(0, 128, 0);      // Leaf Green

    Cells.cell = SampleCell;
    Cells.cell.setPosition(x * blockWidth, y * blockHeight);
    if (x == 0)
    {
        Cells.cell.setFillColor(brown);
    }
    else if (x == COLUMNS - 1)
    {
        Cells.cell.setFillColor(red);
    }
    else if (y == 0 || y == ROWS - 1)
    {
        Cells.cell.setFillColor(darkGreen);
    }
    else
    {
        if ((x + y) % 2 == 0)
        {
            Cells.cell.setFillColor(lightGreen);
        }
        else
        {
            Cells.cell.setFillColor(leafGreen);
        }
    }
    window.draw(Cells.cell);
}

vector<Plant> Plant::AllPlants;
vector<Zombie> Zombie::AllZombies;

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

    // vector<Plant> AllPlants; // changed
    vector<Plant> tempPlants;

    Vector2f CellSize;
    CellSize.x = blockWidth;
    CellSize.y = blockHeight;

    Grid Cells[10][10];
    RectangleShape SampleCell;
    SampleCell.setSize(CellSize);

    // vector<Zombie> AllZombies;

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
                MakeGrid(Cells[x][y], x, y, SampleCell, window);
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
                        Plant::PlacePlant(tempPlants.back() , x , y ,Cells);
                        tempPlants.clear();
                    }
                }
            }
        }

        if (!Plant::isEmpty())
        {
            Plant::DrawAllPlants(window);
        }

        if (Zombie::GetSize() < 20 && elapsed >= Interval)
        {
            Zombie tempZombie(zombieTexture, 5, 30.0);
            
            Zombie::PlaceZombie(tempZombie);
            elapsed = Time::Zero;
        }

        for (size_t i = 0; i < bullets.size(); i++)
        {
            bullets[i].update(timeTaken);
            bullets[i].draw(window);
        }

        if (!Zombie::IsEmpty())
        {
            Zombie::UpdateDrawCheck(timeTaken , window , Cells);
        }

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
