// Lab9HMain.cpp
// Runs on MSPM0G3507
// Lab 9 ECE319H
// Your name
// Last Modified: 12/26/2024

#include <cstdint>
#include <stdio.h>
#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"
#include "../inc/Clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/Timer.h"
#include "../inc/SlidePot.h"
#include "../inc/DAC5.h"
#include "SmallFont.h"
#include "LED.h"
#include "Switch.h"
#include "Sound.h"
#include "Tank.h"
#include "images/images.h"
#include "GameLogic.h"
#include <math.h>
#include "BulletLogic.h"
#include "sounds.h"


extern "C" void __disable_irq(void);
extern "C" void __enable_irq(void);
extern "C" void TIMG12_IRQHandler(void);


// static tank_t tank = {
//   .spriteX = 0, .spriteY = 80, .pivotDx = 40, .pivotDy = -13, .angleDeg = 45
// };

Bullet bullet = {0};
// static aim_t aim = {0};
static bool lastBtnShoot = true;
static bool btnUpdate = true;
static int randomNumber = 0;
static bool playerTurn = true;
static bool isBotMoving = true;

static bool leftPlayerButton = true;
static bool rightPlayerButton = true;

static bool collision = false;

static bool language = true;
//My calm luh prototypes

#define XOFFSETTANKDRAW 13
#define TANKWIDTH 27
#define TANKHEIGHT 24
#define TANK_SPEED 3
#define AIM_LENGTH 20
#define HITBOXPAD 0
#define HITBOXVERTICALSHIFT 0

#define BULLETWIDTH 6


// ****note to ECE319K students****
// the data sheet says the ADC does not work when clock is 80 MHz
// however, the ADC seems to work on my boards at 80 MHz
// I suggest you try 80MHz, but if it doesn't work, switch to 40MHz
void PLL_Init(void){ // set phase lock loop (PLL)
  // Clock_Init40MHz(); // run this line for 40MHz
  Clock_Init80MHz(0);   // run this line for 80MHz
}

int16_t tank1x = 50;
int16_t tank1y = 30;

Tank player = Tank(120, 30, tankRotArray[0], 100, 20, 40);
Tank bot = Tank(XOFFSETTANKDRAW, 30, tankRotArray[0], 100, 20, 40);

int16_t terrainHeights[ST7735_TFTHEIGHT];
int16_t slopes[ST7735_TFTHEIGHT];




uint32_t M=1;
bool semaphore = false;
bool drawFlag = false;
uint32_t Random32(void){
  M = 1664525*M+1013904223;
  return M;
}
uint32_t Random(uint32_t n){
  return (Random32()>>16)%n;
}

SlidePot Sensor(1825,155); // copy calibration from Lab 7

// games  engine runs at 30Hz
void TIMG12_IRQHandler(void){uint32_t pos,msg;
  if((TIMG12->CPU_INT.IIDX) == 1){ // this will acknowledge
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
// game engine goes here
    // 1) sample slide pot
    // 2) read input switches
    // 3) move sprites
        // if(terrainHeights[player.getX()] >= player.getY()){
        //       drawFlag = true;
        // }
        if(player.getX() > XOFFSETTANKDRAW){
          drawFlag = true;
        }
        

    //uint32_t data = Sensor.In();
    //Gettting data, fake for now
          btnUpdate = true;
    //drawFlag = true;
    
    // //redrawing background
    // //can replace with draw bitmap later

    // 4) start sounds
    // 5) set semaphore
      semaphore = false;
    // NO LCD OUTPUT IN INTERRUPT SERVICE ROUTINES
    GPIOB->DOUTTGL31_0 = GREEN; // toggle PB27 (minimally intrusive debugging)
  }
}
uint8_t TExaS_LaunchPadLogicPB27PB26(void){
  return (0x80|((GPIOB->DOUT31_0>>26)&0x03));
}

typedef enum {English, Spanish, Portuguese, French} Language_t;
Language_t myLanguage=English;
typedef enum {HELLO, GOODBYE, LANGUAGE} phrase_t;
const char Hello_English[] ="Hello";
const char Hello_Spanish[] ="\xADHola!";
const char Hello_Portuguese[] = "Ol\xA0";
const char Hello_French[] ="All\x83";
const char Goodbye_English[]="Goodbye";
const char Goodbye_Spanish[]="Adi\xA2s";
const char Goodbye_Portuguese[] = "Tchau";
const char Goodbye_French[] = "Au revoir";
const char Language_English[]="English";
const char Language_Spanish[]="Espa\xA4ol";
const char Language_Portuguese[]="Portugu\x88s";
const char Language_French[]="Fran\x87" "ais";
const char *Phrases[3][4]={
  {Hello_English,Hello_Spanish,Hello_Portuguese,Hello_French},
  {Goodbye_English,Goodbye_Spanish,Goodbye_Portuguese,Goodbye_French},
  {Language_English,Language_Spanish,Language_Portuguese,Language_French}
};
// use main1 to observe special characters
int main1(void){ // main1
    char l;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
  ST7735_FillScreen(0x0000);            // set screen to black
  for(int myPhrase=0; myPhrase<= 2; myPhrase++){
    for(int myL=0; myL<= 3; myL++){
         ST7735_OutString((char *)Phrases[LANGUAGE][myL]);
      ST7735_OutChar(' ');
         ST7735_OutString((char *)Phrases[myPhrase][myL]);
      ST7735_OutChar(13);
    }
  }
  Clock_Delay1ms(3000);
  ST7735_FillScreen(0x0000);       // set screen to black
  l = 128;
  while(1){
    Clock_Delay1ms(2000);
    for(int j=0; j < 3; j++){
      for(int i=0;i<16;i++){
        ST7735_SetCursor(7*j+0,i);
        ST7735_OutUDec(l);
        ST7735_OutChar(' ');
        ST7735_OutChar(' ');
        ST7735_SetCursor(7*j+4,i);
        ST7735_OutChar(l);
        l++;
      }
    }
  }
}

// use main2 to observe graphics
int main2(void){ // main2
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_DrawBitmap(22, 159, PlayerShip0, 18,8); // player ship bottom
  ST7735_DrawBitmap(53, 151, Bunker0, 18,5);
  ST7735_DrawBitmap(42, 159, PlayerShip1, 18,8); // player ship bottom
  ST7735_DrawBitmap(62, 159, PlayerShip2, 18,8); // player ship bottom
  ST7735_DrawBitmap(82, 159, PlayerShip3, 18,8); // player ship bottom
  ST7735_DrawBitmap(0, 9, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(20,9, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(40, 9, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(60, 9, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(80, 9, SmallEnemy30pointA, 16,10);

  for(uint32_t t=500;t>0;t=t-5){
    SmallFont_OutVertical(t,104,6); // top left
    Clock_Delay1ms(50);              // delay 50 msec
  }
  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString((char *)"GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString((char *)"Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString((char *)"Earthling!");
  ST7735_SetCursor(2, 4);
  ST7735_OutUDec(1234);
  while(1){
  }
}

// use main3 to test switches and LEDs
int main3(void){ // main3
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  while(1){
    // write code to test switches and LEDs

  }
}
// use main4 to test sound outputs
int main4(void){ uint32_t last=0,now;
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init(); // initialize LED
  //Sound_Init();  // initialize sound
  TExaS_Init(ADC0,6,0); // ADC1 channel 6 is PB20, TExaS scope
  __enable_irq();
  while(1){
    // now = Switch_In(); // one of your buttons
    // if((last == 0)&&(now == 1)){
    //   Sound_Win(); // call one of your sounds
    // }
    // if((last == 0)&&(now == 2)){
    //   Sound_Button(); // call one of your sounds
    // }
    // if((last == 0)&&(now == 4)){
    //   Sound_Button(); // call one of your sounds
    // }
    // if((last == 0)&&(now == 8)){
    //   Sound_Button(); // call one of your sounds
    // }
    // // modify this to test all your sounds
    Sound_Start(testSound, 32);
  }
}

const uint8_t testWave[16] = {
  9, 10, 12, 14, 15, 14, 12, 10,
  9,  7,  5,  4,  3,  4,  5,  7
};

// int main(void) {
//   LaunchPad_Init(); // Initialize the LaunchPad
//   PLL_Init();       // Initialize PLL
//   Sound_Init(7273, 0); // Initialize sound with 7273 period (~11kHz)
//   // DAC5_Init();      // Initialize the DAC
//   Sound_Start(marioKartWinSound, 1500); // Play the test waveform

//   while(1) {
//     __asm("wfi");   // Wait for interrupt (low power idle)
//   }
// }


// ALL ST7735 OUTPUT MUST OCCUR IN MAIN




int main(void){ // final main
  __disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  ST7735_InitPrintf();
  Sensor.Init(); // PB18 = ADC1 channel 5, slidepot
  Switch_Init(); // initialize switches
  LED_Init();    // initialize LED
  //Sound_Init();  // initialize sound
  Sound_Init(7273, 0); // Initialize sound with 7273 period (~11kHz)

  TExaS_Init(0,0,&TExaS_LaunchPadLogicPB27PB26); // PB27 and PB26
    //note: if you colors are weird, see different options for
    // ST7735_InitR(INITR_REDTAB); inside ST7735_InitPrintf()

    drawWelcomeScreen();

    int32_t health1 = 100;
    int32_t health2 = 100;
    bool first = true;
    uint32_t data = Sensor.In();
    const uint32_t THRESHOLD = 50;

    while (1) {
      uint32_t newData = Sensor.In();
      bool shouldRedraw = false;
      if (first || newData != data) {
        data = newData;
        shouldRedraw = true;
        first = false;
      }

      if (shouldRedraw) {
        drawLanguageButtons(data >= 2047);
      }

      if ((GPIOA->DIN31_0 & (1 << 15)) != 0) {
        Sound_Button();
        if (data >= 2047) {
          drawEnglishSelectionScreen();
        } else {
          drawSpanishSelectionScreen();
        }
        Clock_Delay1ms(100);  
        break;
      }

      Clock_Delay1ms(100);  
    }

    while (1) {
      if (GPIOA->DIN31_0 & (1 << 15)) {
        Sound_Button();
        ST7735_FillScreen(ST7735_BLACK);
        break;
      }
    }





  //Initialization
  ST7735_SetRotation(1);
  updateTerrainHeights(terrainHeights,ST7735_TFTHEIGHT , prettyTerrain);
  updateSlopes(terrainHeights, ST7735_TFTHEIGHT, slopes, ST7735_TFTHEIGHT, true);
  //offsetHeights(terrainHeights, ST7735_TFTHEIGHT, slopes, ST7735_TFTHEIGHT, true);



  player.setX(120);
  player.setY(terrainHeights[player.getX()]);
  bot.setX(XOFFSETTANKDRAW);
  bot.setY(terrainHeights[bot.getX()]);
  
  bullet.x = player.getX();
  bullet.y = player.getY();


  //Make this whats its supposed to be to start with and the cut off will be gone
  int rotIndex = 0;
  player.setImage(tankRotArray[rotIndex]);
  bot.setImage(tankRotArray[rotIndex]);





  //Make objects

  //Draw initial bg
  //ST7735_DrawTerrain(0, 127, terrainMidLatest, 127, 62, 0xFFFF, ST7735_CYAN);
  ST7735_DrawBitmap(0, 128, prettyTerrain, 160, 128);
  //Draw tank, make the drawing dynamic later..
  
  ST7735_DrawBitmapWithBG(player.getX() - XOFFSETTANKDRAW, player.getY(), player.getImage(), TANKWIDTH, TANKHEIGHT, prettyTerrain);
  ST7735_DrawBitmapWithBG(bot.getX() - XOFFSETTANKDRAW, bot.getY(), bot.getImage(), TANKWIDTH, TANKHEIGHT, prettyTerrain);
  

  
  //ST7735_DrawReplaceSolidBitmap(tank1x, tank1y, tank, 27, 15, 0xFFFF, 0);
  

    // initialize interrupts on TimerG12 at 30 Hz
    TimerG12_IntArm(2666667, 0);
    // TimerG12_IntArm(40000000, 0);
  
  // initialize all data structures
  __enable_irq();

  drawHealthBar(bot.getHealth(), player.getHealth());

  while(1){

    if (bot.getHealth() <= 0 || player.getHealth() <= 0) {
    __disable_irq();
    drawHealthBar(bot.getHealth(), player.getHealth());
    Sound_Win();
    Clock_Delay1ms(1000);
    ST7735_SetRotation(0);
    gameOver();
    break;
  }
    
    // wait for semaphore
    while(semaphore){}
    if(drawFlag){
      if(collision){
        drawHealthBar(bot.getHealth(), player.getHealth());
        collision = false;
      }
      if(playerTurn){

      
        //ST7735_DrawReplaceSolidBitmap(player.getX(), player.getY(), tank, 27, 15,prettyTerrain, 0xFFFF, ST7735_CYAN);
        ST7735_MakeBGTransparent(player.getX() - XOFFSETTANKDRAW, player.getY(), player.getImage(), TANKWIDTH, TANKHEIGHT, prettyTerrain);

        //Makes tank move..
        // player.setX(player.getX() - 1);
        // player.setY(terrainHeights[player.getX()]);

        if(leftPlayerButton && player.getX() > bot.getX() + TANKWIDTH){
          player.setX(player.getX() - 1);
          player.setY(terrainHeights[player.getX()]);
        }else if(rightPlayerButton && player.getX() < 160 - XOFFSETTANKDRAW){
          player.setX(player.getX() + 1);
          player.setY(terrainHeights[player.getX()]);
        }

        

        //Making bot move
        if(btnUpdate){
          uint32_t data = Sensor.In();
          player.setAngleDeg(PotAngle(data));
          
          //updateAimLine();
          pollButton();
          updateBullet(&bullet, bot, playerTurn);
          btnUpdate = false;
        }



        //Allow for the other tank to be linked
        //drawFlag = false;
        rotIndex = slopes[player.getX()];
        if(rotIndex > 5){
          rotIndex = 5;
        }
        
        if(rotIndex < 0){
          rotIndex = rotIndex * -1;
          // ST7735_DrawRotateImageCCW90(tankRotArray[rotIndex], 27, 24, tankBuffer);
        // }else{
          // ST7735_copyToBuffer(tankRotArray[rotIndex], 27, 24, tankBuffer);
          player.setImage(tankRotArray[rotIndex]);

        }else{
            player.setImage(tankNegRotArray[rotIndex]);
        }

        ST7735_DrawBitmapWithBG(player.getX() - XOFFSETTANKDRAW, player.getY(), player.getImage(), TANKWIDTH, TANKHEIGHT, prettyTerrain);
      }else{
        //Clearing BG
        if(isBotMoving){

        ST7735_MakeBGTransparent(bot.getX() - XOFFSETTANKDRAW, bot.getY(), bot.getImage(), TANKWIDTH, TANKHEIGHT, prettyTerrain);

        if(randomNumber == 0){
          isBotMoving = false;
          //Redrawing before exit so it doesnt disappear
          ST7735_DrawBitmapWithBG(bot.getX() - XOFFSETTANKDRAW, bot.getY(), bot.getImage(), TANKWIDTH, TANKHEIGHT, prettyTerrain);
          Clock_Delay1ms(1000);
          //Need to make some random angle that it shoots at
          int tempRand = Random(50);
          tempRand += 30;
          // tempRand += (player.getX() - bot.getX()) *20;
          // if(tempRand >= 4095) tempRand = 4095;
          //Setting the random degree to the bot..
          bot.setAngleDeg(tempRand);

          spawnBulletFromTank(&bot, &bullet, playerTurn);
        }else if(randomNumber > 0){
          randomNumber--;
          if(bot.getX() < player.getX() - TANKWIDTH){
            bot.setX(bot.getX() + 1);
            bot.setY(terrainHeights[bot.getX()]);
          }
        }else{
          randomNumber++;
          if(bot.getX() > 0){
            bot.setX(bot.getX() - 1);
            bot.setY(terrainHeights[bot.getX()]);
          }
        }

        bot.setImage(botNicePicture);

        //Moving bot once position is set
        // bot.setImage(tankRotArray[0]);
        // rotIndex = slopes[bot.getX()];

        // if(rotIndex > 5){
        //   rotIndex = 5;
        // }
        // if(rotIndex < 0){
        //   rotIndex = rotIndex * -1;
        //   player.setImage(tankRotArray[rotIndex]);

        // }else{
        //     player.setImage(tankNegRotArray[rotIndex]);
        // }

        ST7735_DrawBitmapWithBG(bot.getX() - XOFFSETTANKDRAW, bot.getY(), bot.getImage(), TANKWIDTH, TANKHEIGHT, prettyTerrain);

        }else{
          //go until you hit the player
          ST7735_DrawBitmapWithBG(bot.getX() - XOFFSETTANKDRAW, bot.getY(), bot.getImage(), TANKWIDTH, TANKHEIGHT, prettyTerrain);
          updateBullet(&bullet, player, playerTurn);
        }

        }
      

      drawFlag = false;
    }

    //ST7735_FillScreen(0);


       // clear semaphore
       semaphore = true;
       // update ST7735R
    // check for end game or level switch
  }
}


static void drawBullet(int16_t x, int16_t y, uint16_t color) {
  //ST7735_MakeBGTransparent(x, y, BulletImage, 6, 6, prettyTerrain);
  ST7735_DrawBitmapWithBG(x, y, player.getImage(), TANKWIDTH, TANKHEIGHT, prettyTerrain);


}

void spawnBulletFromTank(Tank *playerOne, Bullet *bullet, bool playerTurn) {
  Sound_Shoot();
  int16_t px = playerOne->getX();
  int16_t py = playerOne->getY();

  float rad = playerOne->getAngleDeg() * (M_PI / 180.0f);

  int16_t mx = px + (int16_t)(1 * cosf(rad) + 0.5f);
  int16_t my = py - (int16_t)(1 * sinf(rad) + 0.5f);

  bullet->x = mx - 3 / 2;
  bullet->y = my - 3 / 2;

  bullet->vx = (int16_t)(TANK_SPEED * cosf(rad) + 0.5f);
  bullet->vy = (int16_t)(-TANK_SPEED * sinf(rad) + 0.5f);
  bullet->vy *= 4;

  if(!playerTurn){
    bullet->vx *= -1;
  }

  bullet->active = true;
}

void updateBullet(Bullet *bullet, Tank target, bool playerTurnBullet) {
  // Early return if bullet is not active
  if (!bullet->active) {
    return;
  }
  
  // Validate bullet pointer
  if (bullet == NULL) {
    return;
  }
  
  // Save previous position to restore background later
  int16_t prevX = bullet->x;
  int16_t prevY = bullet->y;
  
  // Clear bullet from previous position if within bounds
  if (prevX >= 0 && prevX < 160 - 6 && prevY >= 0 && prevY < 128 - 6) {
    ST7735_MakeBGTransparent(prevX, prevY, BulletImage, 6, 6, prettyTerrain);
  }

  // Update bullet position
  bullet->x -= bullet->vx;
  bullet->y += bullet->vy;

  // Check for out of bounds in X direction FIRST
  if (bullet->x < 0 || bullet->x >= 160 - BULLETWIDTH) {
    // Safely cap values to prevent array out-of-bounds access
    if (bullet->x < 0) {
      bullet->x = 0;
    } else {
      bullet->x = 159 - BULLETWIDTH;
    }
    
    // Deactivate bullet and switch players
    bullet->active = false;
    switchPlayers();
    return;
  }
  
  // // Check for out of bounds in Y direction
  // if (bullet->y < 0 || bullet->y >= 128 - 6) {
  //   // Safely cap values
  //   if (bullet->y < 0) {
  //     bullet->y = 0;
  //   } else {
  //     bullet->y = 127 - 6;
  //   }
    
  //   // Deactivate bullet and switch players
  //   bullet->active = false;
  //   switchPlayers();
  //   return;
  // }

  // Apply gravity (slowing it down)
  if (bullet->x % 4 == 0) {
    bullet->vy += 1; // gravity
  }

  // Check for collision with target tank
  // Ensure all comparisons use safe bounds
  if (bullet->y >= target.getY() - TANKHEIGHT + HITBOXVERTICALSHIFT && 
      bullet->y <= target.getY() + HITBOXVERTICALSHIFT &&
      bullet->x > target.getX() - HITBOXPAD - XOFFSETTANKDRAW &&
      bullet->x < target.getX() + HITBOXPAD + TANKWIDTH - XOFFSETTANKDRAW) {
    
    // Handle collision with tank
    bullet->active = false;
    
    // Apply damage based on whose turn it is
    if (playerTurnBullet) {
      bot.takeDamage(40);
    } else {
      player.takeDamage(40);
    }
    
    collision = true;
    switchPlayers();
    return;
  } 
  
  // Check for collision with terrain
  // Ensure bullet->x is valid for array access
  if (bullet->x >= 0 && bullet->x < 160 && bullet->y >= 0 && bullet->y < 128) {
    // Safe array access - double check bounds
    if (bullet->y >= terrainHeights[bullet->x]) {
      bullet->active = false;
      switchPlayers();
      return;
    }
  }
  
  // Only draw the bullet if it's within display bounds
  if (bullet->active && bullet->x >= 0 && bullet->x < 160 - 6 && bullet->y >= 0 && bullet->y < 128 - 6) {
    ST7735_DrawBitmapWithBG(bullet->x, bullet->y, BulletImage, 6, 6, prettyTerrain);
  }
}

void switchPlayers() {
  randomNumber = Random(40);
  M++;
  if (randomNumber % 2 == 0 && bot.getX() - randomNumber - XOFFSETTANKDRAW > 0) {
    randomNumber = -1 * randomNumber;
  }
  playerTurn = !playerTurn;
  if(playerTurn == false){
    isBotMoving = true;
  }
}

void pollButton(void) {
  bool btn = (GPIOA->DIN31_0 & (1 << 17)) == 0;
  if (btn && !lastBtnShoot && !bullet.active) {
    spawnBulletFromTank(&player, &bullet, playerTurn);
  }
  lastBtnShoot = btn;

  leftPlayerButton = (GPIOA->DIN31_0 & (1 << 18)) != 0;
  rightPlayerButton = (GPIOA->DIN31_0 & (1 << 16)) != 0;
}

static inline uint8_t PotAngle(uint32_t adc) {
  return 10 + (adc * 70 / 4095);

}


void drawRoundedButton(int x, int y, int width, int height, uint16_t borderColor, uint16_t fillColor) {
  ST7735_FillRect(x+2, y, width-4, height, fillColor);
  ST7735_FillRect(x, y+2, width, height-4, fillColor);
  
  ST7735_DrawPixel(x+1, y+1, fillColor);
  ST7735_DrawPixel(x+width-2, y+1, fillColor);
  ST7735_DrawPixel(x+1, y+height-2, fillColor);
  ST7735_DrawPixel(x+width-2, y+height-2, fillColor);
  
  ST7735_DrawFastHLine(x+2, y, width-4, borderColor);
  ST7735_DrawFastHLine(x+2, y+height-1, width-4, borderColor);
  ST7735_DrawFastVLine(x, y+2, height-4, borderColor);
  ST7735_DrawFastVLine(x+width-1, y+2, height-4, borderColor);
  
  ST7735_DrawPixel(x+1, y+1, borderColor);
  ST7735_DrawPixel(x+width-2, y+1, borderColor);
  ST7735_DrawPixel(x+1, y+height-2, borderColor);
  ST7735_DrawPixel(x+width-2, y+height-2, borderColor);
}

void drawWelcomeScreen() {
  for (int y = 0; y < 160; y++) {
    uint16_t color = ST7735_BLUE + (y / 10);
    ST7735_DrawFastHLine(0, y, 128, color);
  }
  
  for (int x = 0; x < 128; x += 4) {
    int height = 10 + (x % 20);
    ST7735_DrawFastVLine(x, 0, height, ST7735_CYAN);
  }
  
  const char *welcomeText = "Welcome!";
  int centerX = 32; 
  
  int k = centerX;
  for (int i = 0; welcomeText[i] != '\0'; i++) {
    ST7735_DrawChar(k+1, 41, welcomeText[i], ST7735_BLACK, ST7735_BLUE, 2);
    k += 12;
  }
  
  k = centerX;
  for (int i = 0; welcomeText[i] != '\0'; i++) {
    ST7735_DrawChar(k, 35, welcomeText[i], ST7735_WHITE, ST7735_BLUE, 2);
    k += 12;
  }
  
  const char *selectText = "Pick a language:";
  centerX = 14; 
  k = centerX;
  for (int i = 0; selectText[i] != '\0'; i++) {
    ST7735_DrawChar(k, 55, selectText[i], ST7735_WHITE, ST7735_BLUE, 1);
    k += 6;
  }
  
  const char *swText = "Use RGHT and Pot";
  centerX = 14; 
  k = centerX;
  for (int i = 0; swText[i] != '\0'; i++) {
    ST7735_DrawChar(k, 65, swText[i], ST7735_WHITE, ST7735_BLUE, 1);
    k += 6;
  }

  drawRoundedButton(10, 80, 108, 25, ST7735_MAGENTA, ST7735_BLUE);
  
  const char *englishText = "English";
  centerX = 40; 
  k = centerX;
  for (int i = 0; englishText[i] != '\0'; i++) {
    ST7735_DrawChar(k, 87, englishText[i], ST7735_WHITE, ST7735_BLUE, 1);
    k += 6;
  }
  
  drawRoundedButton(10, 110, 108, 25, ST7735_MAGENTA, ST7735_BLUE);
  
  const char *spanishText = "Spanish";
  centerX = 40; 
  k = centerX;
  for (int i = 0; spanishText[i] != '\0'; i++) {
    ST7735_DrawChar(k, 117, spanishText[i], ST7735_WHITE, ST7735_BLUE, 1);
    k += 6;
  }
  
  for (int x = 0; x < 128; x += 4) {
    int startY = 140 + (x % 10);
    ST7735_DrawFastVLine(x, startY, 160-startY, ST7735_CYAN);
  }
  
  const char *credits = "By Adiv & Jackson";
  centerX = 20;
  k = centerX;
  for (int i = 0; credits[i] != '\0'; i++) {
    ST7735_DrawChar(k, 145, credits[i], ST7735_YELLOW, ST7735_BLACK, 1);
    k += 6;
  }
}

void gameOver(void) {
  ST7735_FillScreen(ST7735_WHITE);
  Sound_Win();
  if (language) {
    const char *gameOverText = "GAME OVER!";

    int k = 6;
    for (int i = 0; gameOverText[i] != '\0'; i++) {
      ST7735_DrawChar(k, 40, gameOverText[i], ST7735_RED, ST7735_WHITE, 2);
      k += 12;
    }
  } else {
      const char *gameOverText1 = "JUEGO";
      const char *gameOverText2 = "TERMINADO";

    int k = 34;
    for (int i = 0; gameOverText1[i] != '\0'; i++) {
      ST7735_DrawChar(k, 40, gameOverText1[i], ST7735_RED, ST7735_WHITE, 2);
      k+=12;
    }

    k = 13;
    for (int i = 0; gameOverText2[i] != '\0'; i++) {
      ST7735_DrawChar(k, 80, gameOverText2[i], ST7735_RED, ST7735_WHITE, 2);
      k+=12;
    }
  }

}

void drawHealthBar(int32_t health1, int32_t health2) {
  int32_t displayHealth1 = (health1 < 0) ? 0 : health1;
  int32_t displayHealth2 = (health2 < 0) ? 0 : health2;
  
  ST7735_FillRect(10, 10, 60, 10, ST7735_WHITE);  
  ST7735_FillRect(80, 10, 60, 10, ST7735_WHITE);  
  
  ST7735_FillRect(9, 9, 62, 12, ST7735_BLACK);    
  ST7735_FillRect(79, 9, 62, 12, ST7735_BLACK);   
  
  uint32_t barLength1 = (displayHealth1 * 60) / 100; 
  uint32_t barLength2 = (displayHealth2 * 60) / 100; 
  
  uint16_t color1, color2;
  
  if (displayHealth1 > 70) {
    color1 = ST7735_GREEN;
  } else if (displayHealth1 > 30) {
    color1 = ST7735_YELLOW;
  } else {
    color1 = ST7735_RED;
  }
  
  if (displayHealth2 > 70) {
    color2 = ST7735_GREEN;
  } else if (displayHealth2 > 30) {
    color2 = ST7735_YELLOW;
  } else {
    color2 = ST7735_RED;
  }
  
  ST7735_FillRect(10, 10, barLength1, 10, color1); 
  ST7735_FillRect(80, 10, barLength2, 10, color2);  
  
  
}

void restartGame(void) {
  ST7735_FillScreen(ST7735_BLACK);
  GPIOA->DIN31_0 &= 0;
  Clock_Delay1ms(1000);
}

void drawButtonWithLabel(int x, int y, uint16_t bgColor, uint16_t textColor, const char* label) {
  drawRoundedButton(x, y, 108, 25, ST7735_MAGENTA, bgColor);
  int k = 40;
  int yOffset = y + 7;
  for (int i = 0; label[i] != '\0'; i++) {
    ST7735_DrawChar(k, yOffset, label[i], textColor, bgColor, 1);
    k += 6;
  }
}

void drawLanguageButtons(bool englishSelected) {
  if (englishSelected) {
    language = true;
    drawButtonWithLabel(10, 80, ST7735_CYAN, ST7735_BLACK, "English");
    drawButtonWithLabel(10, 110, ST7735_BLUE, ST7735_WHITE, "Spanish");
  } else {
    language = false;
    drawButtonWithLabel(10, 80, ST7735_BLUE, ST7735_WHITE, "English");
    drawButtonWithLabel(10, 110, ST7735_CYAN, ST7735_BLACK, "Spanish");
  }
}

void drawEnglishSelectionScreen() {
  const char* title = "ENGLISH";
  const char* sub = "SELECTED";
  const char* prompt = "PRESS RGHT TO PLAY";
  ST7735_FillScreen(ST7735_WHITE);

  int k = 20, j = 20;
  for (int i = 0; sub[i] != '\0'; i++) {
    ST7735_DrawChar(k, 60, title[i], ST7735_BLUE, ST7735_WHITE, 2);
    ST7735_DrawChar(k - 5, 75, sub[i], ST7735_BLUE, ST7735_WHITE, 2);
    k += 12;
  }
  for (int i = 0; prompt[i] != '\0'; i++) {
    ST7735_DrawChar(j - 4, 90, prompt[i], ST7735_BLUE, ST7735_WHITE, 1);
    j += 5;
  }
}

void drawSpanishSelectionScreen() {
  const char* title = "SPANISH";
  const char* sub = "SELECCION";
  const char* prompt1 = "PRESIONE 'DERECHA' PARA";
  const char* prompt2 = "JUGAR";

  ST7735_FillScreen(ST7735_WHITE);

  int k = 20, j = 20;
  for (int i = 0; title[i] != '\0'; i++) {
    ST7735_DrawChar(k, 60, title[i], ST7735_BLUE, ST7735_WHITE, 2);
    k += 12;
  }

  k = 20;
  for (int i = 0; sub[i] != '\0'; i++) {
    ST7735_DrawChar(k - 12, 75, sub[i], ST7735_BLUE, ST7735_WHITE, 2);
    k += 12;
  }

  for (int i = 0; prompt1[i] != '\0'; i++) {
    ST7735_DrawChar(j - 15, 90, prompt1[i], ST7735_BLUE, ST7735_WHITE, 1);
    j += 5;
  }

  j = 30;
  for (int i = 0; prompt2[i] != '\0'; i++) {
    ST7735_DrawChar(j + 15, 100, prompt2[i], ST7735_BLUE, ST7735_WHITE, 1);
    j += 5;
  }
}



int main6(void) {
  //__disable_irq();
  PLL_Init(); // set bus speed
  LaunchPad_Init();
  Switch_Init(); // initialize switches
  LED_Init();
  //Sensor.Init();

  ST7735_InitPrintf();
  ST7735_FillScreen(ST7735_WHITE);
  ST7735_DrawBitmap(0, 80, player.getImage(), 27, 24);



  TimerG12_IntArm(2666667, 0);

  __enable_irq();

  while (1) {
    __WFI();

  }
}





