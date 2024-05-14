#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>
#include <fstream>
#include <string>

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
    Character(const Texture& texture) : sprite()
    {
        this->texture = texture;
        sprite.setTexture(texture);
        float scaleFactorX = static_cast<float>(blockWidth) / texture.getSize().x;
        float scaleFactorY = static_cast<float>(blockHeight) / texture.getSize().y;
        sprite.setScale(scaleFactorX * 0.6, scaleFactorY * 0.6);
    }

    virtual void draw(RenderWindow& window) const
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
    Plant* plant = nullptr;
};

class Bullet : public Character
{
public:
    Bullet(const Texture& texture, const Vector2f& position) : Character(texture)
    {
        sprite.setPosition(position);
    }
    void update(float deltaTime)
    {
        sprite.move(50 * deltaTime, 0); // Adjust the speed here
    }
};

class Zombie;
int amout;
enum class PlantType {
    Normal,
    Fire
};
vector<Plant*> AllPlants;
class Plant : public Character
{
protected:
    Time ElapsedTillShot;
    Clock clock;
    //static vector<Plant*> AllPlants;
    vector<Bullet> bullets;
    Texture bulletTexture;
    int health;
    PlantType type;
public:
    // Plant(const Texture& texture, const Texture& bulletTexture) : Character(texture), bulletTexture(bulletTexture) {
      //   ElapsedTillShot = Time::Zero;
        // clock.restart().asSeconds();
     //}
    Plant(const Texture& texture, const Texture& bulletTexture, PlantType type)
        : Character(texture), bulletTexture(bulletTexture), type(type) {
        ElapsedTillShot = Time::Zero;
        clock.restart().asSeconds();
    }

    void setPosition(int x, int y, Grid(&cell)[10][10])
    {
        sprite.setPosition(x * blockWidth, y * blockHeight);
        cell[x][y].isOccupied = true;
        cell[x][y].plant = this;
        cout << "placed";
        health = 6;
    }
    void draw(RenderWindow& window) const override
    {
        window.draw(sprite);
        for (size_t i = 0; i < bullets.size(); i++)
        {
            bullets[i].draw(window);
        }
    }

    void updateBullets(float deltaTime)
    {
        for (size_t i = 0; i < bullets.size(); i++)
        {
            bullets[i].update(deltaTime);
        }
    }

    void drawBullets(RenderWindow& window) const
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

    void BulletDestroy();

    
    void Shoot(RenderWindow& window, float timeTaken) {
        ElapsedTillShot += clock.getElapsedTime();
        Time Interval = seconds(3000.0f);
        if (ElapsedTillShot >= Interval)
        {
            bullets.push_back(Bullet(bulletTexture, this->sprite.getPosition()));
            clock.restart();
            ElapsedTillShot = Time::Zero;
        }
        this->updateBullets(timeTaken);
        this->drawBullets(window);
        BulletDestroy();
    }

    static void PlantShooter(RenderWindow& window, float timeTaken) {
        for (size_t i = 0; i < AllPlants.size(); i++) {
            AllPlants[i]->Shoot(window, timeTaken);
            AllPlants[i]->paisay();
        }
    }
    
    static void PlacePlant(Plant* plant, int x, int y, Grid(&Cell)[10][10]);

    static bool isEmpty() {
        return AllPlants.empty();
    }
    static void DrawAllPlants(RenderWindow& window) {
        for (const auto& plant : AllPlants)
        {
            plant->draw(window);
        }
    }
    static void RemovePlant(Plant removablePlant) {
        for (auto it = AllPlants.begin(); it != AllPlants.end(); ++it)
        {
            if ((*it)->sprite.getPosition() == removablePlant.sprite.getPosition())
            {
                AllPlants.erase(it);
                cout << "erased";
                break;
            }

        }
    }
    virtual void paisay() {
        amout = amout;
    }
};
class NormalPlant : public Plant {
public:
    //NormalPlant(const Texture& texture, const Texture& bulletTexture)
      //  : Plant(texture, bulletTexture) {}
    NormalPlant(const Texture& texture, const Texture& bulletTexture)
        : Plant(texture, bulletTexture, PlantType::Normal) {

    }
    void paisay() override {
        amout -= 10;
    }
};

class FirePlant : public Plant {
public:
    // FirePlant(const Texture& texture, const Texture& bulletTexture)
      //   : Plant(texture, bulletTexture) {}
    FirePlant(const Texture& texture, const Texture& bulletTexture)
        : Plant(texture, bulletTexture, PlantType::Fire) {

    }
    void paisay() override {
        amout -= 50;
    }
};
void Plant::PlacePlant(Plant* plant, int x, int y, Grid(&Cell)[10][10]) {
    if (dynamic_cast<NormalPlant*>(plant)) {
        amout -= 10;
    }
    else if (dynamic_cast<FirePlant*>(plant)) {
        amout -= 50;
    }
    AllPlants.push_back(plant);
    AllPlants.back()->setPosition(x, y, Cell);
}
class Zombie : public Character
{
protected:
    static vector<Zombie> AllZombies;
    int health;
    float speed;

public:
    Zombie(const Texture& texture, int health, float speed) : Character(texture), health(health), speed(speed)
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
    void PlantDetector(Grid(&Cells)[10][10])
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
                    // cout << "Nice";
                    speed = 30;
                }
            }
        }
    }

    static bool CollisionDetector(Bullet& bullet) {

        for (auto it = AllZombies.begin(); it != AllZombies.end(); ++it)
        {
            if (bullet.getBounds().contains(it->sprite.getPosition()))
            {
                it->takeDamage(1);
                if (it->isDead())
                {
                    AllZombies.erase(it);
                    amout += 15;
                    return true;
                }
            }
        }
        return false;
    }
    
    static void PlaceZombie(Zombie zombie)
    {
        mt19937 gen(chrono::high_resolution_clock::now().time_since_epoch().count());
        uniform_int_distribution<int> distrib(2, 7);
        int pos = distrib(gen); // Generate a new random number between 1 and 8
        zombie.sprite.setPosition(blockWidth * 9, blockHeight * pos);
        AllZombies.push_back(zombie);
    }

    static void GameChecker(RenderWindow& window){
        for (auto &b : AllZombies)
        {
            int x = b.sprite.getPosition().x / blockWidth;
            int y = b.sprite.getPosition().y / blockHeight;
            if (x<=1)
            {
                window.close();
            }
            
        }
        
    }
    
    static void UpdateDrawCheck(float timeTaken, RenderWindow& window, Grid(&Cells)[10][10]) {
        for (Zombie& zombie : AllZombies)
        {
            zombie.update(timeTaken);
            zombie.draw(window);
            zombie.PlantDetector(Cells);


            // CollisionDetector();
        }
    }
    static bool IsEmpty() {
        return AllZombies.empty();
    }

    static int GetSize() {
        return AllZombies.size();
    }

};

void Plant::BulletDestroy() {

    for (auto it = bullets.begin(); it != bullets.end(); ++it) {

        if (Zombie::CollisionDetector(*it)) {
            bullets.erase(it);
        }
    }

}

void MakeGrid(Grid& Cells, int x, int y, RectangleShape SampleCell, RenderWindow& window)
{
    Color brown(139, 69, 19);        // Brown
    Color darkGreen(0, 100, 0);      // Dark Green
    Color red(255, 0, 0);            // Red
    Color lightGreen(144, 238, 144); // Light Green
    Color leafGreen(0, 128, 0);      // Leaf Green

    Cells.cell = SampleCell;
    Cells.cell.setPosition(x * blockWidth, y * blockHeight);
    if (x == 0 || x == 1)
    {
        Cells.cell.setFillColor(brown);
    }
    else if (x == COLUMNS - 1 || x == COLUMNS - 1 )
    {
        Cells.cell.setFillColor(red);
    }
    else if (y == 0 ||y == 1 || y == ROWS - 1|| y == ROWS - 2)
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
class User {
private:
    string username;

public:
    User() :username("") {}
    User(const string& name) :username(name) {}
    bool saveInFile() const {
        ofstream file("username.txt", ios::app);
        file << username << endl;
        file.close();
        return true;
    }

    bool check() const {
        ifstream file("username.txt");
        if (!file.is_open()) {
            cerr << "Error opening file!!" << endl;
            return false;
        }
        string line;
        string lowercaseUsername = username;
        transform(lowercaseUsername.begin(), lowercaseUsername.end(), lowercaseUsername.begin(), ::tolower);
        while (getline(file, line)) {
            string lowercaseLine = line;
            transform(lowercaseLine.begin(), lowercaseLine.end(), lowercaseLine.begin(), ::tolower);
            if (lowercaseLine == lowercaseUsername) {
                return true;
            }
        }
        return false;
    }


    string getUsername() const {
        return username;
    }

    void setUsername(const string& name) {
        username = name;
    }

    bool isValidUsername() const {
        return !check();
    }
};

//vector<Plant> Plant::AllPlants;
vector<Zombie> Zombie::AllZombies;

int main() {
    User u;
    RenderWindow userWindow(VideoMode(1000, 600), "Enter Your Username");
    userWindow.requestFocus();
    Font font;
    if (!font.loadFromFile("arial.ttf")) {
        cerr << "Error loading font" << endl;
        return EXIT_FAILURE;
    }
    Text t;
    t.setFont(font);
    t.setString("Enter your username:");
    t.setCharacterSize(24);
    t.setFillColor(Color::White);
    t.setPosition(50, 100);

    RectangleShape player(Vector2f(2, 30));
    player.setFillColor(Color::Black);

    Text usernameText;
    usernameText.setFont(font);
    usernameText.setCharacterSize(24);
    usernameText.setFillColor(Color::White);
    usernameText.setPosition(50, 300);

    Text statusText;
    statusText.setFont(font);
    statusText.setCharacterSize(16);
    statusText.setFillColor(Color::Red);
    statusText.setPosition(50, 250);

    bool validity = false;
    bool isBlinkVisible = true;
    Clock blinkClock;
    while (userWindow.isOpen() && !validity) {
        Event event;
        while (userWindow.pollEvent(event)) {
            if (event.type == Event::Closed) {
                userWindow.close();
            }
            if (event.type == Event::TextEntered) {
                if (event.text.unicode < 128) {
                    if (event.text.unicode == '\b') {
                        if (!u.getUsername().empty()) {
                            u.setUsername(u.getUsername().substr(0, u.getUsername().size() - 1));
                        }
                    }
                    else {
                        u.setUsername(u.getUsername() + static_cast<char>(event.text.unicode));
                    }
                }
            }
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Return && !u.getUsername().empty()) {
                    validity = u.isValidUsername();
                    if (!validity) {
                        statusText.setString("Username already taken. \nPlease choose another one.");
                        u.setUsername("");
                    }
                    else {
                        if (u.saveInFile()) {
                            statusText.setString("Username saved successfully!");
                        }
                        else {
                            statusText.setString("Failed to save username. Please try again.");
                        }
                    }
                }
            }
        }
        if (blinkClock.getElapsedTime().asSeconds() >= 0.5f) {
            isBlinkVisible = !isBlinkVisible;
            blinkClock.restart();
        }

        userWindow.clear();
        userWindow.draw(t);

        usernameText.setString("Username: " + u.getUsername());
        userWindow.draw(usernameText);
        userWindow.draw(statusText);
        if (isBlinkVisible && !u.getUsername().empty()) {
            player.setPosition(50 + usernameText.getLocalBounds().width + 2, 100);
            userWindow.draw(player);
        }

        userWindow.display();
    }
    Clock clock;
    Clock Zclock;
    Time ElapsedTillZombieSpawn = Time::Zero;
    Time Interval = seconds(1000.0f);
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
    vector<Plant*>tempPlants;

    Vector2f CellSize;
    CellSize.x = blockWidth;
    CellSize.y = blockHeight;

    Grid Cells[10][10];
    RectangleShape SampleCell;
    SampleCell.setSize(CellSize);

    // vector<Zombie> AllZombies;

    vector<Bullet> bullets;

    Clock Shootclock;
    Time elapsedTillShot = Time::Zero;
    if (validity) {
        RenderWindow gameWindow(VideoMode(winWidth, winHeight), "Plants vs Zombies");
        while (window.isOpen())
        {
            float timeTaken = clock.restart().asSeconds();
            ElapsedTillZombieSpawn += Zclock.getElapsedTime();
            elapsedTillShot += Shootclock.getElapsedTime();


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
                    // if (x == 0 && (y >= 0 && y < 3))
                     //{
                       //  if (y == 0)
                         //{
                           //  Plant TempPlant(plantTexture, bulletTexture);
                             //tempPlants.push_back(TempPlant);
                         //}
                         //if (y == 1)
                         //{
                           //  Plant TempPlant(plantTexture, bulletTexture);
                             //tempPlants.push_back(TempPlant);
                         //}
                         //if (y == 2)
                         //{
                           //  Plant TempPlant(plantTexture, bulletTexture);
                             //tempPlants.push_back(TempPlant);
                         //}
                     //}
                    if (x == 0 && (y >= 0 && y < 3)) {
                        if (y == 0) {
                            Plant* po = new NormalPlant(plantTexture, bulletTexture);
                            tempPlants.push_back(po);
                        }

                        if (y == 1){
                            Plant* po1 = new FirePlant(plantTexture, bulletTexture);
                            tempPlants.push_back(po1);
                        }
                        //if (y == 2) {
                          //  NormalPlant TempPlant(plantTexture, bulletTexture);
                          //  tempPlants.push_back(TempPlant);
                       // }
                    }
                    else if (x >= 2 && x < ROWS - 2 && y >= 2 && y < COLUMNS - 2 && (!Cells[x][y].isOccupied))
                    {
                        if (!tempPlants.empty())
                        {
                            Plant::PlacePlant(tempPlants.back(), x, y, Cells);
                            tempPlants.clear();
                        }
                    }
                }
            }

            if (!Plant::isEmpty())
            {
                Plant::DrawAllPlants(window);
                Plant::PlantShooter(window, timeTaken);
            }

            if (Zombie::GetSize() < 20 && ElapsedTillZombieSpawn >= Interval)
            {
                Zombie tempZombie(zombieTexture, 5, 30.0);
                
                Zombie::PlaceZombie(tempZombie);
                ElapsedTillZombieSpawn = Time::Zero;
            }

            for (size_t i = 0; i < bullets.size(); i++)
            {
                bullets[i].update(timeTaken);
                bullets[i].draw(window);
            }

            if (!Zombie::IsEmpty())
            {
                Zombie::UpdateDrawCheck(timeTaken, window, Cells);
                Zombie::GameChecker(window);
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
    }

    return 0;
}
