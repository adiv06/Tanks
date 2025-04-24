// GameLogic.h
#ifndef GAMELOGIC_H
#define GAMELOGIC_H

void updateTerrainHeights(int16_t terrainHeights[],  int length, const uint16_t *image);
void updateSlopes(int16_t terrainHeights[],  int length, int16_t slopes[], int lengthSlope, bool playerOne);
void offsetHeights(int16_t terrainHeights[],  int length, int16_t slopes[], int lengthSlope, bool playerOne);

void switchPlayers();


void drawRoundedButton(int x, int y, int width, int height, uint16_t borderColor, uint16_t fillColor);
void drawWelcomeScreen(void);
void gameOver(void);
void drawHealthBar(int32_t health1, int32_t health2);
void restartGame(void);
void drawButtonWithLabel(int x, int y, uint16_t bgColor, uint16_t textColor, const char* label);
void drawLanguageButtons(bool englishSelected);
void drawEnglishSelectionScreen(void);
void drawSpanishSelectionScreen(void);

#endif
