//====================================================================================
// BlackRamElectroncs.com
//====================================================================================

#include "stdint.h"
#include "string.h"
#include "unistd.h"
#include "OLED_Driver.h"

// Image data files
#include "BR_logo_anim.h"
#include "BR_Logo_LCD.h"

void SplashScreen(void);
void TextDemo(void);
void ImageDemo(void);
void AnimationDemo(void);

// Graphics buffer for OLED display
uint8_t DisplayBuffer [(X_PIXELS * Y_PIXELS) / 8];

//====================================================================================
int main(int argc, char **argv)
{
	int i;
	
	printf("Black Ram Electronics Mono OLED Demo\n");
	
	// Configure interface hardware and setup the display
	InitOLED();
	
	// Loop through the demo three times
	for(i = 0; i < 3; i++)
	{
		SplashScreen();
		TextDemo();
		ImageDemo();
		AnimationDemo();
	}
	
	// Release the hardware
	DeinitOLED();

	return(0);
}

//====================================================================================
void SplashScreen(void)
{
	memset(DisplayBuffer, 0, sizeof(DisplayBuffer));

	DrawTextToBuffer(0, (uint8_t*)" 0.96\" OLED Display  ", DisplayBuffer);
	DrawTextToBuffer(2, (uint8_t*)"      BLACK RAM       ", DisplayBuffer);
	DrawTextToBuffer(3, (uint8_t*)"     ELECTRONICS      ", DisplayBuffer);
	DrawTextToBuffer(5, (uint8_t*)"Dev Boards & Displays ", DisplayBuffer);
	DrawTextToBuffer(6, (uint8_t*)"        With          ", DisplayBuffer);
	DrawTextToBuffer(7, (uint8_t*)" Examples & Tutorials ", DisplayBuffer);
	
	WriteBufferToDisplay(DisplayBuffer);
	sleep(3);
}

//====================================================================================
void TextDemo(void)
{
	uint32_t row = 0, col = 0, i;

	memset(DisplayBuffer, 0, sizeof(DisplayBuffer));
	DrawTextToBuffer(3, (uint8_t*)"      TEXT DEMO      ", DisplayBuffer);
	WriteBufferToDisplay(DisplayBuffer);
	sleep(1);
	
	memset(DisplayBuffer, 0, sizeof(DisplayBuffer));
	for(i = 0; i <= 96; i++)
	{
		DrawChar(33 + i, row, col++, DisplayBuffer);
		if(col >= TEXT_CHARACTERS_PER_ROW)
		{
			row++;
			col = 0;
		}
	}
	
	WriteBufferToDisplay(DisplayBuffer);
	sleep(2);
}

//====================================================================================
void ImageDemo(void)
{
	memset(DisplayBuffer, 0, sizeof(DisplayBuffer));
	DrawTextToBuffer(3, (uint8_t*)"     IMAGE DEMO      ", DisplayBuffer);
	WriteBufferToDisplay(DisplayBuffer);
	sleep(1);

	DisplayImage(BR_LOGO_LCD);
	sleep(2);
}

//====================================================================================
void AnimationDemo(void)
{
	uint32_t frame_index;
	
	memset(DisplayBuffer, 0, sizeof(DisplayBuffer));
	DrawTextToBuffer(3, (uint8_t*)"   ANIMATION DEMO    ", DisplayBuffer);
	WriteBufferToDisplay(DisplayBuffer);
	sleep(1);
	
	for(frame_index = 0; frame_index < FRAME_COUNT; frame_index++)
	{
		usleep(FRAME_DEALY * 1000);
		DisplayImage((unsigned char*)BR_LOGO_ANIM[frame_index]);
	}
	sleep(2);
}
