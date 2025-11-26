#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/canvas.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/screen/terminal.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/screen/color.hpp>

#include <iostream>
#include <ctime>
#include <cmath>
#include <random>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

using namespace ftxui;
// comment

void setup(int w, int h);
void updateSimulation(int w, int h);
double getDistance(int x1, int y1, int x2, int y2);
Element renderScene(int w, int h);
int randomInt(int min, int max);

int numPlanets;
int numSatellites;

std::mutex g_mutex;
std::atomic<bool> quit_simulation = false;

struct satellite
{
    double xPos;
    double yPos;
    int radius;
    Color color;

    double xVelocity;
    double yVelocity;
};

const float G_SCALE = 2.0f;

struct planet
{
    int xPos;
    int yPos;
    int radius;
    float gravity;
    Color color;
};

void checkCollisions(satellite &s, planet &p);

Canvas staticCanvas;

std::vector<satellite> satVec;
std::vector<planet> planetVec;

int main()
{
    auto screen = ScreenInteractive::Fullscreen();

    // adjust for braille scaling
    int canvas_width = screen.dimx() * 2;
    int canvas_height = screen.dimy() * 4;

    staticCanvas = Canvas(canvas_width, canvas_height);
    setup(canvas_width, canvas_height);

    std::thread simulation_thread([&]
                                  {
        while (!quit_simulation) {
            updateSimulation(canvas_width, canvas_height);
            // Sleep for 16ms to target ~60 updates per second
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        } });

    auto renderer = Renderer([&]
                             { screen.PostEvent(Event::Custom); return renderScene(canvas_width, canvas_height); });

    screen.Loop(renderer);

    quit_simulation = true;

    // Wait for the thread to finish its current work and exit gracefully
    if (simulation_thread.joinable())
    {
        simulation_thread.join();
    }

    return 0;
}

void setup(int w, int h)
{

    numPlanets = randomInt(1, 3);
    numSatellites = randomInt(4, 6);
    // init planets
    for (int i = 0; i < numPlanets; i++)
    {
        planet p;
        p.gravity = (float)(randomInt(2, 10));
        p.radius = randomInt(8, 14);
        p.xPos = randomInt(w / 10, w - w / 10);
        p.yPos = randomInt(h / 10, h - h / 10);
        p.color = Color(static_cast<Color::Palette256>(randomInt(0, 255)));

        staticCanvas.DrawPointCircleFilled(p.xPos, p.yPos, p.radius, p.color);
        planetVec.push_back(p);
    }
    // init sats
    for (int i = 0; i < numSatellites; i++)
    {
        satellite s;
        s.radius = randomInt(2, 6);
        s.xPos = randomInt(w / 10, w - w / 10);
        s.yPos = randomInt(h / 10, h - h / 10);
        s.color = Color(static_cast<Color::Palette256>(randomInt(0, 255)));

        s.xVelocity = randomInt(-2, 2);
        s.yVelocity = randomInt(-2, 2);

        satVec.push_back(s);
    }
}

Element renderScene(int w, int h)
{

    std::lock_guard<std::mutex> lock(g_mutex);

    Canvas satCanvas = staticCanvas;

    for (auto &s : satVec)
    {
        satCanvas.DrawPointCircleFilled((int)(s.xPos), (int)(s.yPos), s.radius, s.color);
    }

    return canvas(satCanvas);
}

// distance formula
double getDistance(int x1, int y1, int x2, int y2)
{
    return (double)(sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2)));
}

std::mt19937 rng(std::random_device{}());

int randomInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

void updateSimulation(int w, int h)
{

    std::lock_guard<std::mutex> lock(g_mutex);

    for (auto &s : satVec)
    {

        for (auto &p : planetVec)
        {
            checkCollisions(s, p);

            double r = (double)getDistance(s.xPos, s.yPos, p.xPos, p.yPos);

            if (r < 1.0)
                r = 1.0;

            double dx = (double)p.xPos - (double)s.xPos;
            double dy = (double)p.yPos - (double)s.yPos;

            // Calculate Acceleration Magnitude (A = G*M / r^2)
            double acceleration = p.gravity / (r * r);

            // Normalize the direction vector (dx/r, dy/r) and apply acceleration and scaling
            // The new acceleration vector is (accel_x, accel_y) = A * (dx/r, dy/r)
            s.xVelocity += (acceleration * (dx / r) * G_SCALE);
            s.yVelocity += (acceleration * (dy / r) * G_SCALE);
        }

        s.xPos += s.xVelocity;
        s.yPos += s.yVelocity;

        if (s.xPos > w)
        {
            s.xPos = 0;
        }
        else if (s.xPos < 0)
        {
            s.xPos = w;
        }

        if (s.yPos > h)
        {
            s.yPos = 0;
        }
        else if (s.yPos < 0)
        {
            s.yPos = h;
        }
    }
}

void checkCollisions(satellite &s, planet &p)
{
    float minDistance = (float)s.radius + (float)p.radius;
    float actualDistance = getDistance(s.xPos, s.yPos, p.xPos, p.yPos);
    if (actualDistance <= minDistance)
    {
        s.xVelocity = 0;
        s.yVelocity = 0;
        s.color = Color::RGBA(0, 0, 0, 0);
        // explode
        // remove from vec later
    }
}