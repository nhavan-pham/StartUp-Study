#include "DisplayForecast.h"

#define COMMON_X_POS              20
#define LOCATION_Y_POS            20
#define TEMPERATURE_Y_POS         60
#define DESCRIPTION_Y_POS         105

extern uint8_t iconLocation[];
extern uint8_t iconTemperature_day[];
extern uint8_t iconTemperature_night[];
extern uint8_t iconDescription[];

// Constructor
CDisplayForecast::CDisplayForecast()
{

}

// Destructor
CDisplayForecast::~CDisplayForecast()
{

}

void CDisplayForecast::displayLocation(Arduino_ILI9225 *tftDisplay, String szLocation, uint16_t color)
{
    if(tftDisplay == NULL)
    {
        return;
    }

    drawLocation(tftDisplay, COMMON_X_POS, LOCATION_Y_POS, szLocation, 2, color);
}
void CDisplayForecast::displayTemperature(Arduino_ILI9225 *tftDisplay, String szTemperature, bool bIsDay, uint16_t color)
{
    if(tftDisplay == NULL)
    {
        return;
    }

    drawTemperature(tftDisplay, COMMON_X_POS, TEMPERATURE_Y_POS, szTemperature, bIsDay, 5, color);
}
void CDisplayForecast::displayDescription(Arduino_ILI9225 *tftDisplay, String szDescription, uint16_t color)
{
    if(tftDisplay == NULL)
    {
        return;
    }

    drawDescription(tftDisplay, COMMON_X_POS, DESCRIPTION_Y_POS, szDescription, 2, color);
}

void CDisplayForecast::displayWindSpeed(String szWindSpeed)
{

}
void CDisplayForecast::displayWindDeg(String szWindDeg)
{

}
void CDisplayForecast::displayHumidity(String szHumidity)
{

}
void CDisplayForecast::displayPressure(String szPressure)
{

}
void CDisplayForecast::displayClouds(String szClouds)
{

}
void CDisplayForecast::displayVisibility(String szVisibility)
{

}

void CDisplayForecast::drawLocation(Arduino_ILI9225 *tftDisplay, int16_t x, int16_t y, String szLocation, int8_t nTextSize, uint16_t color) 
{
	drawIcon(tftDisplay, 20, 10, iconLocation, 30, 30, color);
    
	tftDisplay->setTextColor(color);
	tftDisplay->setCursor(x + 35, y);
	tftDisplay->setTextSize(nTextSize);
	tftDisplay->println(szLocation);
}

void CDisplayForecast::drawTemperature(Arduino_ILI9225 *tftDisplay, int16_t x, int16_t y, String szTemperature, bool bIsDay, int8_t nTextSize, uint16_t color) 
{
    int8_t nX=x;
    int8_t nY=y;
    int8_t nSize=nTextSize;

    if(bIsDay)
    {
        drawIcon(tftDisplay, 15, nY - 3, iconTemperature_day, 40, 40, color);
    }
    else
    {
        drawIcon(tftDisplay, 15, nY - 3, iconTemperature_night, 40, 40, color);
    }

	tftDisplay->setTextColor(color);
	tftDisplay->setCursor(nX + 45, nY);
	tftDisplay->setTextSize(nSize);
	tftDisplay->println(szTemperature);

	tftDisplay->setCursor(nX + 65 + 45, nY - 6);
	tftDisplay->setTextSize(nSize-2);
	tftDisplay->println("o");
	tftDisplay->setCursor(nX + 85 + 45, nY);
	tftDisplay->setTextSize(nSize);
	tftDisplay->println("C");
}

void CDisplayForecast::drawDescription(Arduino_ILI9225 *tftDisplay, int16_t x, int16_t y, String szDescription, int8_t nTextSize, uint16_t color) 
{
    drawIcon(tftDisplay, 20, 115, iconDescription, 30, 30, color);

	tftDisplay->setTextColor(color);
	tftDisplay->setCursor(15, 145);
	tftDisplay->setTextSize(nTextSize);
	tftDisplay->println(szDescription);
}

void CDisplayForecast::drawIcon(Arduino_ILI9225 *tftDisplay, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) 
{
	int16_t byteWidth = (w + 7) / 8;
    uint8_t byte = 0;

    tftDisplay->startWrite();
    for (int16_t j = 0; j < h; j++, y++)
    {
        for (int16_t i = 0; i < w; i++)
        {
            if (i & 7)
            {
                byte <<= 1;
            }
            else
            {
                byte = bitmap[j * byteWidth + i / 8];
            }
            if (byte & 0x80)
            {
                tftDisplay->writePixel(x + i, y, color);
            }
        }
    }
    tftDisplay->endWrite();
}