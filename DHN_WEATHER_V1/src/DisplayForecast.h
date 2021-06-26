#include <stdio.h>
#include <stdint.h>
#include <WString.h>
#include <Arduino_GFX_Library.h>

#define DARKBLUE_CUSTOM 0x0009		/// text color of the day theme

/// Main and core class
class CDisplayForecast {

    public:
        CDisplayForecast();
        ~CDisplayForecast();

        /// Draw solid rectangle, rectangle coordinates
        /// @param    x1 top left coordinate, x-axis
        /// @param    y1 top left coordinate, y-axis
        /// @param    x2 bottom right coordinate, x-axis
        /// @param    y2 bottom right coordinate, y-axis
        /// @param    color 16-bit color
        
        void displayWindSpeed(String szWindSpeed);
        void displayWindDeg(String szWindDeg);
        void displayHumidity(String szHumidity);
        void displayPressure(String szPressure);
        void displayClouds(String szClouds);
        void displayVisibility(String szVisibility);

        
        static void displayLocation(Arduino_ILI9225 *tftDisplay, String szLocation, uint16_t color);
        static void displayTemperature(Arduino_ILI9225 *tftDisplay, String szTemperature, bool bIsDay, uint16_t color);
        static void displayDescription(Arduino_ILI9225 *tftDisplay, String szDescription, uint16_t color);

    protected:
        static void drawLocation(Arduino_ILI9225 *tftDisplay, int16_t x, int16_t y, String szLocation, int8_t nTextSize, uint16_t color);
        static void drawTemperature(Arduino_ILI9225 *tftDisplay, int16_t x, int16_t y, String szTemperature, bool bIsDay, int8_t nTextSize, uint16_t color);
        static void drawDescription(Arduino_ILI9225 *tftDisplay, int16_t x, int16_t y, String szDescription, int8_t nTextSize, uint16_t color);

        static void drawIcon(Arduino_ILI9225 *tftDisplay, int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);

};
