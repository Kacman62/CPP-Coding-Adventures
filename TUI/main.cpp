#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>

#include <iostream>
#include <ctime>
#include <cmath>
#include <random>
#include <vector>

using namespace ftxui;

void setup();
void update_simulation();
float getDistance(int x1, int y1, int x2, int y2);
Element renderScene();
int randomInt(int min, int max);

int numPlanets;
int numSatellites;

float gravityStrength;

struct satellite
{
    int xPos;
    int yPos;
    int radius;
    float gravity;
};

struct planet
{
    int xPos;
    int yPos;
    int radius;
    float gravity;
};

std::vector<satellite> satVec;
std::vector<planet> planetVec;

auto screen = ftxui::ScreenInteractive::TerminalOutput();

int main()
{
    setup();

    // Renderer Component: updates the scene each frame
    auto renderer = Renderer([&]
                             { return renderScene(); });

    auto screen = ScreenInteractive::TerminalOutput();

    screen.Loop(renderer); // Runs the interactive loop
}

void setup()
{

    numPlanets = rand() % 3 + 1;
    numSatellites = rand() % 6 + 2;
    // init planets
    for (int i = 0; i < numPlanets; i++)
    {
        planetVec.push_back(planet());
        planetVec.at(i).gravity = (float)(randomInt(2, 10));
        planetVec.at(i).radius = randomInt(2, 10);
        planetVec.at(i).xPos = randomInt(8, 20);
        planetVec.at(i).yPos = randomInt(8, 40);
    }
    // init sats
    for (int i = 0; i < numSatellites; i++)
    {
        satVec.push_back(satellite());
        satVec.at(i).gravity = (float)(randomInt(2, 10));
        satVec.at(i).radius = randomInt(2, 10);
        satVec.at(i).xPos = randomInt(8, 20);
        satVec.at(i).yPos = randomInt(8, 40);
    }
}

Element renderScene()
{

    Canvas c(screen.dimx_, screen.dimy_);

    // Draw satellites
    for (auto &s : satVec)
    {
        c.DrawPointCircleFilled(s.xPos, s.yPos, s.radius);
    }

    // Draw planets
    for (auto &p : planetVec)
    {
        c.DrawPointCircleFilled(p.xPos, p.yPos, p.radius);
    }

    return canvas(c);
}

// distance formula
float getDistance(int x1, int y1, int x2, int y2)
{
    return (float)(sqrt(pow((x2 + x1), 2) - pow((y2 + y1), 2)));
}

std::mt19937 rng(std::random_device{}());

int randomInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}