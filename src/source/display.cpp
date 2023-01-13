#include "../include/display.h"

SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);
OLEDDisplayUi ui(&display);

uint8_t currentFrame = 0;
String timenow = "00:00:00";

int16_t x_axis = 0;
int16_t y_axis = 0;
int16_t last_x_axis = 0;
int16_t last_y_axis = 0;
int analogValue = 0;

int displayBPM = 0;

String* displayWeather;

void setDisplayTime(String currentTime)
{
    timenow = currentTime;
}

void clock(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{

    display->setFont(ArialMT_Plain_24);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(64 + x, 10 + y, timenow);
}

void weatherCast(OLEDDisplay *display, OLEDDisplayUiState *state, int16_t x, int16_t y)
{
}

void drawWeather(){
    if (displayWeather == 0){
        displayWeather = fetchWeather();
    }
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 16, String(displayWeather[0]) + " " + String(displayWeather[1]) + "\n" + String(displayWeather[2]));
    display.display();
}

FrameCallback frames[] = {clock, weatherCast};

// how many frames are there?
int frameCount = 2;

void initDispaly()
{
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 16, "Booting...");
    display.display();
}

void initUI()
{
    ui.setTargetFPS(60);
    ui.disableAutoTransition();
    ui.disableAllIndicators();
    ui.setFrameAnimation(SLIDE_LEFT);
    ui.setFrames(frames, frameCount);
    ui.init();
    display.flipScreenVertically();
}

void clearDisplay()
{
    display.clear();
}

int16_t remainingTimeBudget()
{
    if (currentFrame != 1)
    {
        return ui.update();
    }
    else
    {
        // show weather cast
        return -1;
    }
}

void uiControl()
{
    ui.nextFrame();
    OLEDDisplayUiState *currentState = ui.getUiState();
    currentFrame = currentState->currentFrame; //This variable is inside the library!
    Serial.println("current Frame : " + String(currentFrame));
}

void turnOffDisplay()
{
    display.displayOff();
}

void onDispaly()
{
    display.displayOn();
}

void changeFrame()
{
    clearDisplay();
    ui.switchToFrame(!currentFrame);
    OLEDDisplayUiState *currentState = ui.getUiState();
    currentFrame = currentState->currentFrame; //This variable is inside the library!
}