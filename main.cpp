#include <SFML/Graphics.hpp>
#include<iostream>
using namespace std;
using namespace sf;

const int ROWS = 8;
const int COLS = 8; 

 Vector2f Fitter(Vector2f ToBeFitIn , Vector2u ToBeFit){
        Vector2f tmmp;
        tmmp.x = ToBeFitIn.x / ToBeFit.x;
        tmmp.y = ToBeFitIn.y / ToBeFit.y;
        return tmmp;
    }

class Plant{

    Sprite plant;
    int health;
    Texture PlantTexture;

    public:

    Plant(int h){
        health = h;
        if (!PlantTexture.loadFromFile("Images\\Attack_1.png"))
        {
            cout<<"error while loading plant"<<endl;
        }
    }

    Vector2u GetTextureSize(){
        return PlantTexture.getSize();
    }

    void PlantAnimation(Vector2f pos , Vector2f Size){
        Vector2u Frame = GetTextureSize(); 
        int framWidth = (Frame.x)/6;
        Vector2f plantScale =  Fitter(Size , Frame);
        IntRect shooter_plant(0 ,0 , framWidth , Frame.y*1.05);
        plant.setTexture(PlantTexture);
        plant.setTextureRect(shooter_plant);
        plant.setPosition(pos);
        plant.setScale(plantScale.x*5.5 , plantScale.y*0.7 );
    }
    Sprite GetSprite() const{
        return plant;
    }
   
};

struct Cells{
    bool isOcuppied;
    RectangleShape Cell;
};

int main()
{
    RenderWindow window(VideoMode(1280, 720) , "Plants Vs Zombies Lite");

    RectangleShape Cell_sprite;
    
    Vector2u WindowSize = window.getSize();

    Cells All_Cells[ROWS][COLS];

    Vector2f CellSize;
    CellSize.x = (WindowSize.x)/ROWS;
    CellSize.y = (WindowSize.y)/COLS;
    Cell_sprite.setSize(CellSize); 

    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < ROWS; j++)
        {
            All_Cells[i][j].Cell = Cell_sprite;
            All_Cells[i][j].Cell.setPosition(i*(CellSize.x) , j*(CellSize.y));
            if ((i + j) % 2 == 0)
            {
                All_Cells[i][j].Cell.setFillColor(Color::White);
            }else{
                All_Cells[i][j].Cell.setFillColor(Color::Yellow);
            }
        }
    }
    
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            
        }
       window.clear( );
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                window.draw(All_Cells[i][j].Cell);
            }
        }
        Vector2f cell_pos =  All_Cells[2][3].Cell.getPosition();
        Plant My_plant(100);
        Vector2f CellSize = All_Cells[2][3].Cell.getSize();
        My_plant.PlantAnimation(cell_pos , CellSize);
        window.draw(My_plant.GetSprite());
        window.display();
    }   
}