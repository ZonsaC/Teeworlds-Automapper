#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include "game.h"

std::string DDNET_PATH;
std::string PNG_NAME;

//Private functions
void Game::initVariables() 
{
    version = "Version 1.1";

    this->window = nullptr;
    Scene = 0;
    rClick = false;
    lClick = false;
    configFilePath = "Properties.cfg";

    hoverColor = sf::Color(96, 96, 96, 150);            //GREY
    defaultColor = sf::Color(192, 192, 192, 100);       //WHITE
    clickedColor[0] = sf::Color(230, 110, 110, 100);    //RED
    clickedColor[1] = sf::Color(110, 230, 127, 100);    //GREEN

    GetPathsFromConfig(configFilePath);
    font.loadFromFile("assets/knowyour.ttf");
    BackgroundTexture.loadFromFile("assets/background.png");
    fillerTexture.loadFromFile("assets/fillerTiles.png");
    exportTexture.loadFromFile("assets/exportButton.png");

    for(int i = 0; i < 256; i++)
        for(int x = 0; x < 4; x++)
        {
            automapRandom[i][x] = 0;
            for(int j = 0; j < 9; j++)
                automap[i][x][j] = 0;
        }
}

void Game::initWindow() 
{
    this->videoMode.height = 600;
    this->videoMode.width = 800;

    this->window = new sf::RenderWindow(this->videoMode, "Teeworlds Automapper", sf::Style::Titlebar | sf::Style::Close);

    Background.setTexture(BackgroundTexture);
}

void Game::GetPathsFromConfig(std::string configFile)
{
    std::string line;
    std::ifstream file(configFile);
    std::string ddnetMapresPath;
    std::string tilesetName;

    if (file.is_open())
    {
        while (getline(file, line))
        {
            if (line.find("DDNET_MAPRES_PATH") != std::string::npos)
            {
                DDNET_PATH = line.substr(line.find("=") + 1);
            }
            else if (line.find("TILESET_NAME") != std::string::npos)
            {
                PNG_NAME = line.substr(line.find("=") + 1);
            }
        }
        file.close();
    }
    else
    {
        std::cout << "Unable to open file" << std::endl;
    }
}


//Constructors / Destructors
Game::Game()
{
    this->initVariables();
    this->initWindow();
    this->setText();
    this->setExportButton();
    this->setBackButton();
}

Game::~Game()
{
delete this->window;
}

//Accessors
const bool Game::getWindowIsOpen() const
{
    return this->window->isOpen();
}


//Functions
void Game::pollEvent()
{
    while(this->window->pollEvent(this->ev))
    {
        switch(this->ev.type)
        {
            case sf::Event::Closed:
                this->window->close();
            break;

            case sf::Event::KeyPressed:
                if (this->ev.key.code == sf::Keyboard::Escape)
                    this->window->close();

                if(this->ev.key.code == sf::Keyboard::Enter && Scene == 0)
                    getTileset();
                
                if(this->ev.key.code == sf::Keyboard::Q && Scene == 2)
                    Scene = 1;
            break;

            case sf::Event::MouseButtonPressed:

                mousePosition = sf::Mouse::getPosition(*window);

                if (ev.mouseButton.button == sf::Mouse::Left && Scene == 1)
                {
                    checkTilesetClicked();

                    if(exportButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) exportAutomap();
                    
                } else
                if(ev.mouseButton.button == sf::Mouse::Left && Scene == 2)
                {
                    for(int i = 0; i < 4; i++)
                    {
                        if(rotatedTile[i].getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                        {
                            curRotation = i;
                            randomInput = std::to_string(automapRandom[curIndex][curRotation]);
                            randomText.setString("Random: " + std::to_string(automapRandom[curIndex][curRotation]));
                            
                            curTile = drawTileMatrix(rotatedTile[i]);
                        }
                    }
                    

                    if(backButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) Scene = 1;

                    lClick = true;

                } else
                if(ev.mouseButton.button == sf::Mouse::Right && Scene == 2)
                {
                    rClick = true;
                }
            break;

            case sf::Event::MouseButtonReleased:
                if (ev.mouseButton.button == sf::Mouse::Left)
                {
                    lClick = false;
                    rClick = false;
                }
            break;



            case sf::Event::TextEntered:
            if (Scene == 0)
            {
                if (ev.text.unicode == 8) // ASCII code for backspace
                {
                    if (!userInput.empty())
                    {
                        userInput.pop_back(); // remove last character
                    }
                }
                else if (ev.text.unicode < 128) // only add ASCII characters
                {
                    userInput += static_cast<char>(ev.text.unicode);
                }

                inputText.setString(userInput);
            }

            if(Scene == 2)
            {
                if (ev.text.unicode == 8) // ASCII code for backspace
                {
                    if (!randomInput.empty())
                    {
                        randomInput.pop_back(); // remove last character
                        automapRandom[curIndex][curRotation] = atoi(randomInput.c_str());
                    }
                } else if (ev.text.unicode > 47 && ev.text.unicode < 58) // only add ASCII characters
                {
                    randomInput += static_cast<char>(ev.text.unicode);
                    automapRandom[curIndex][curRotation] = atoi(randomInput.c_str());
                }

                randomText.setString("Random: " + std::to_string(automapRandom[curIndex][curRotation]));
            }
            break;
        }
    
    }    
}

void Game::setText()
{
    versionText.setFillColor(sf::Color(255,255,255,150));
    versionText.setFont(font);
    versionText.setCharacterSize(15.f);
    versionText.setString(version);
    versionText.setPosition(sf::Vector2f(5.f, videoMode.height - versionText.getCharacterSize() - 5.f));

    randomText.setFillColor(sf::Color::White);
    randomText.setFont(font);
    randomText.setCharacterSize(20.f);
    randomText.setString("Random: ");

    inputText.setFillColor(sf::Color::White);
    inputText.setFont(font);
    inputText.setCharacterSize(40.f);

    nameText.setFillColor(sf::Color::White);
    nameText.setFont(font);
    nameText.setCharacterSize(50.f);
    nameText.setString("Name your Current Automap");
    
    while(nameText.getGlobalBounds().width > videoMode.width)
    {
        nameText.setCharacterSize(nameText.getCharacterSize() - 3);
        nameText.setOrigin(nameText.getGlobalBounds().width / 2, nameText.getGlobalBounds().height / 2);
        nameText.setPosition(sf::Vector2f(videoMode.width / 2, videoMode.height / 2 - 20));

    }
}

void Game::setExportButton()
{
    exportButton.setFillColor(sf::Color::White);
    exportButton.setSize(sf::Vector2f(150, 50));
    exportButton.setOrigin(sf::Vector2f(exportButton.getGlobalBounds().width / 2, exportButton.getGlobalBounds().height / 2));
    exportButton.setPosition(sf::Vector2f(videoMode.width - exportButton.getGlobalBounds().width / 2 - 10, 10 + exportButton.getGlobalBounds().height / 2));
    exportButton.setTexture(&exportTexture);

    exportText.setFont(font);
    exportText.setString("Export");
    exportText.setCharacterSize(25.f);
    exportText.setOrigin(sf::Vector2f(exportText.getGlobalBounds().width / 2, exportText.getGlobalBounds().height / 2));
    exportText.setPosition(sf::Vector2f(exportButton.getPosition().x, exportButton.getPosition().y - 5));
}

void Game::setBackButton()
{
    backButton.setFillColor(sf::Color::White);
    backButton.setSize(sf::Vector2f(150, 50));
    backButton.setOrigin(sf::Vector2f(backButton.getGlobalBounds().width / 2, backButton.getGlobalBounds().height / 2));
    backButton.setPosition(sf::Vector2f(videoMode.width - backButton.getGlobalBounds().width / 2 - 10, videoMode.height - 10 - backButton.getGlobalBounds().height / 2));
    backButton.setTexture(&exportTexture);

    backText.setFont(font);
    backText.setString("Back");
    backText.setCharacterSize(25.f);
    backText.setOrigin(sf::Vector2f(backText.getGlobalBounds().width / 2, backText.getGlobalBounds().height / 2));
    backText.setPosition(sf::Vector2f(backButton.getPosition().x, backButton.getPosition().y - 5));
}

void Game::getTileset()
{
    texture.loadFromFile(DDNET_PATH + "\\" + PNG_NAME);
    Tileset.setTexture(texture);
    sf::Sprite unscaledTileset = Tileset;

    //Scale correctly
    float tileset_height = Tileset.getGlobalBounds().height;
    scale = window->getSize().y / tileset_height;
    Tileset.setScale(scale, scale);

    //Set Tiles
    int top = 0;
    int left = 0;
    for(int i = 0; i < 256; i++)
    {
        tiles[i].height = unscaledTileset.getGlobalBounds().height / 16;
        tiles[i].width = unscaledTileset.getGlobalBounds().width / 16;
        
        if(i % 16 == 0 && i != 0)
            top += tiles[i].height;

        tiles[i].top = top;

        if(i % 16 == 0)
            left = 0;
            
        tiles[i].left = left;

        left += tiles[i].width;
    }


    Scene = 1;    
}

void Game::checkTilesetClicked()
{
    // Get the mouse position relative to the window
    sf::Vector2f mousePositionFloat = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*window));
    mousePositionFloat *= 1 /scale;

    for(int i = 0; i < 256; i++) if(tiles[i].contains(static_cast<sf::Vector2i>(mousePositionFloat))){

        //Tile Clicked                        
        clickedTile = tiles[i];
        curIndex = i;
        curRotation = 0;
        randomInput = std::to_string(automapRandom[curIndex][curRotation]);
        randomText.setString("Random: " + std::to_string(automapRandom[curIndex][curRotation]));
        tiledImage(); 
        break;
    }
}

void Game::tiledImage()
{
    curTile = drawTileMatrix(curTile);

    float posY = 40.f;
    for (int i = 0; i < 4; i++) {
        rotatedTile[i] = curTile;
        rotatedTile[i].setScale(0.6f, 0.6f);
        float height = rotatedTile[i].getGlobalBounds().height;
        float width = rotatedTile[i].getGlobalBounds().width;
        rotatedTile[i].setSize(sf::Vector2f(width, height));
        rotatedTile[i].setRotation(90 * i);
        rotatedTile[i].setOrigin(width / 2.f, height / 2.f);
        rotatedTile[i].setPosition(sf::Vector2f(videoMode.width - rotatedTile[i].getGlobalBounds().width / 2 - 10.f, posY));
        posY += rotatedTile[i].getGlobalBounds().height + 5.f;
        rotatedTile[i].setOutlineThickness(-5);
        rotatedTile[i].setOutlineColor(sf::Color(160, 160, 160, 150));
    }

    Scene = 2;
}

void Game::tileHoverUpdate()
{    
    for(int i = 0; i < 9; i++)
    {
        if(fillerTiles[i].getGlobalBounds().contains(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*window))))
        {
            if(rClick)
            {
                if(fillerTiles[i].getFillColor() == clickedColor[0])
                {
                    fillerTiles[i].setFillColor(defaultColor);
                    automap[curIndex][curRotation][i] = 0;
                } else
                {
                    fillerTiles[i].setFillColor(clickedColor[0]);
                    automap[curIndex][curRotation][i] = 1;
                }
            }

            if(lClick)
            {
                if(fillerTiles[i].getFillColor() == clickedColor[1])
                {
                    fillerTiles[i].setFillColor(defaultColor);
                    automap[curIndex][curRotation][i] = 0;
                } else
                {
                    fillerTiles[i].setFillColor(clickedColor[1]);
                    automap[curIndex][curRotation][i] = 2;
                }
            }

            //Hover
            if(fillerTiles[i].getFillColor() != clickedColor[0] && fillerTiles[i].getFillColor() != clickedColor[1])
                fillerTiles[i].setFillColor(hoverColor);
        } else 
        {
            //Unhover
            if(fillerTiles[i].getFillColor() != clickedColor[0] && fillerTiles[i].getFillColor() != clickedColor[1])
                fillerTiles[i].setFillColor(defaultColor);
        }
    }



    rClick = false;
    lClick = false;
}

void Game::showCurrentAutomap()
{
    std::cout << "------------------------------------------------------------------------" << "\n";
    for(int i = 0; i < 256; i++)
    {
        for(int x = 0; x < 4; x++){
            std::cout << x << " Index " << i << " ";
            for(int j = 0; j < 9; j++)
                std::cout << automap[i][x][j] << " ";
            std::cout << "\n";
        }
    }
    std::cout << "------------------------------------------------------------------------" << "\n";
}

void Game::exportAutomap()
{
    std::string filename("Output.txt");
    std::fstream file_out;

    file_out.open(filename, std::ios_base::out);
    if (!file_out.is_open()) {
        std::cout << "failed to open " << filename << '\n';
    } else {
        file_out << "[" << userInput << "]" << std::endl;
        for(int i = 0; i < 256; i++)
        {
            for(int x = 0; x < 4; x++){
                IndexWritten = false;
                for(int j = 0; j < 9; j++)
                {
                    switch(j)
                    {
                        case 0:
                            file_out << getExportString(i, x, j, "Pos -1 -1");
                        break;
                        case 1:
                            file_out << getExportString(i, x, j, "Pos 0 -1");
                        break;
                        case 2:
                            file_out << getExportString(i, x, j, "Pos 1 -1");
                        break;
                        case 3:
                            file_out << getExportString(i, x, j, "Pos -1 0");
                        break;
                        case 4:
                            file_out << getExportString(i, x, j, "Pos 0 0");
                        break;
                        case 5:
                            file_out << getExportString(i, x, j, "Pos 1 0");
                        break;
                        case 6:
                            file_out << getExportString(i, x, j, "Pos -1 1");
                        break;
                        case 7:
                            file_out << getExportString(i, x, j, "Pos 0 1");
                        break;
                        case 8:
                            file_out << getExportString(i, x, j, "Pos 1 1");
                        break;
                    }
                }

                if(automapRandom[i][x] != 0 && IndexWritten)
                {
                    file_out << "Random " + std::to_string(automapRandom[i][x]) + "\n";
                }

            }
        }
    }
}

std::string Game::getExportString(int i, int x, int j, std::string Pos)
{
    std::string output;
    if(automap[i][x][j] != 0 || automapRandom[i][x] != 0)
    {
        if(!IndexWritten)
        { 
            output = "\nIndex " + std::to_string(i); 
            switch(x)
            {
                case 1:
                    output += " ROTATE";
                break;
                case 2:
                    output += " XFLIP YFLIP";
                break;
                case 3:
                    output += " ROTATE XFLIP YFLIP";
                break;
            }
            output += "\n";
            IndexWritten = true; 
        }
        if(automap[i][x][j] != 0)
            output += Pos;
    }
    switch(automap[i][x][j])
    {
        case 1:
        output += " EMPTY\n";
        break;
        case 2:
        output += " FULL\n";
        break;
    }

    return output;
}

sf::RectangleShape Game::drawTileMatrix(sf::RectangleShape rec)
{
    float Size = 150;

    
    rec.setTexture(Tileset.getTexture());
    rec.setTextureRect(clickedTile);

    rec.setSize(sf::Vector2f(Size, Size));
    rec.setScale(1.f, 1.f);
    rec.setOrigin(rec.getGlobalBounds().width / 2.f, rec.getGlobalBounds().height / 2.f);
    rec.setRotation(curRotation * 90);

    float posX = 0, posY = 0;
    for(int i = 0; i < 9; i++)
    {
        if(automap[curIndex][curRotation][i] == 0)
        fillerTiles[i].setFillColor(defaultColor);
        else if(automap[curIndex][curRotation][i] == 1)
        fillerTiles[i].setFillColor(clickedColor[0]);
        else if(automap[curIndex][curRotation][i] == 2)
        fillerTiles[i].setFillColor(clickedColor[1]);
        
        fillerTiles[i].setTexture(&fillerTexture);
        fillerTiles[i].setOutlineThickness(-5);
        fillerTiles[i].setOutlineColor(sf::Color(160, 160, 160, 150));
        fillerTiles[i].setSize(sf::Vector2f(Size ,Size));
        fillerTiles[i].setPosition(sf::Vector2f(posX, posY));

        if(i == 4)
            rec.setPosition(sf::Vector2f(posX + rec.getGlobalBounds().width / 2.f, posY + rec.getGlobalBounds().height / 2.f));

        posX += Size;
        if((i + 1) % 3 == 0)
        {
            posX = 0;
            posY += Size;
        }
    }
    return rec;
}

void Game::textUpdate()
{
    inputText.setOrigin(inputText.getGlobalBounds().width / 2, inputText.getGlobalBounds().height / 2);
    inputText.setPosition(sf::Vector2f(videoMode.width / 2, videoMode.height / 2 + 30));

    while (inputText.getGlobalBounds().width > videoMode.width)
    {
        inputText.setCharacterSize(inputText.getCharacterSize() - 1);
    }

    randomText.setOrigin(randomText.getGlobalBounds().width / 2, randomText.getGlobalBounds().height / 2);
    randomText.setPosition(sf::Vector2f(videoMode.width / 2, videoMode.height - 50));
    
}

void Game::update() 
{
    if(Scene == 2)
        tileHoverUpdate();

    textUpdate();
    //Event polling
    this->pollEvent();
}

void Game::render() 
{
    /*
        @return void

        * clear old frame
        * render objects
        * display frame in window
        
        Renders the game objects.
    */
    this->window->clear(sf::Color(255, 0, 0, 255));

    window->draw(Background);
    window->draw(versionText);
    //Full Tileset
    switch(Scene){
        case 0: // Start Screen
            window->draw(inputText);
            window->draw(nameText);
        break;
        case 1:  // Tileset Screen
            window->draw(Tileset);
            window->draw(exportButton);
            window->draw(exportText);
        break;
        case 2: // Tiled Screen
            window->draw(curTile);
            window->draw(backButton);
            window->draw(backText);
            window->draw(randomText);

            for(sf::RectangleShape rec : fillerTiles)
                window->draw(rec);
            for(sf::RectangleShape rec : rotatedTile)
                window->draw(rec);
        break;

    }
    

    //Draw game objects

    this->window->display();
}




