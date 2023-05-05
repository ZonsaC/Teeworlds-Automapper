#ifndef __GAME_H
#define __GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

/*
    Class that acts as the game engine.
    Wrapper class.
*/

class Game
{
private:
    //Variables
    int Scene;
    float scale;
    bool lClick, rClick;
    bool IndexWritten;

    //Window
    sf::RenderWindow* window;
    sf::VideoMode videoMode;
    sf::Event ev;
    sf::Vector2i mousePosition;
    sf::Sprite Background;
    sf::Texture BackgroundTexture;

    std::string version;
    std::string userInput;
    std::string randomInput;
    sf::Text inputText;
    sf::Text nameText;
    sf::Text versionText;
    sf::Text randomText;
    sf::Font font;

    sf::Sprite Tileset;
    sf::Texture texture;
    sf::Texture fillerTexture;

    sf::IntRect tiles[256];
    sf::IntRect clickedTile;
    sf::RectangleShape curTile;
    sf::RectangleShape fillerTiles[9];

    sf::RectangleShape rotatedTile[4];

    //Colors
    sf::Color hoverColor;
    sf::Color defaultColor;
    sf::Color clickedColor[2];

    //Export Button
    sf::RectangleShape exportButton;
    sf::Texture exportTexture;
    sf::Text exportText;

    //Back Button
    sf::RectangleShape backButton;
    sf::Texture backTexture;
    sf::Text backText;

    int curIndex;
    int curRotation;
    int automap[256][4][9];
    int automapRandom[256][4];

    std::string configFilePath;

    //Private function
    void initVariables();
    void initWindow();
public:
    //Constructors / Destructors
    Game();
    virtual ~Game();

    //Accessors
    const bool getWindowIsOpen() const;
    void GetPathsFromConfig(std::string);

    //Functions
    void pollEvent();
    void setText();
    void setExportButton();
    void setBackButton();
    void textUpdate();
    sf::RectangleShape drawTileMatrix(sf::RectangleShape);
    void getTileset();
    void checkTilesetClicked();
    void tiledImage();
    void tileHoverUpdate();
    void showCurrentAutomap();
    void exportAutomap();
    std::string getExportString(int, int, int, std::string);
    void update();
    void render();
};

#endif