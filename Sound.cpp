// Sound.cpp
// Runs on MSPM0
// Sound assets in sounds/sounds.h
// Jackson Herleth
// JH77847

#include "Sound.h"
#include "../inc/DAC5.h"
#include "../inc/Timer.h"
#include "sounds.h"
#include <stdint.h>
#include <ti/devices/msp/msp.h>

volatile const uint8_t *soundPt = 0;
volatile uint32_t soundCount = 0;
uint32_t period = 0;

const uint8_t Waveform[2] = {0, 31}; // square wave
volatile uint32_t index = 0;

void SysTick_IntArm(uint32_t period, uint32_t priority) {
  SysTick->CTRL = 0x00;
  SysTick->LOAD = period - 1;
  SysTick->VAL = 0;
  SCB->SHP[1] = SCB->SHP[1] & (~0xC0000000); // set priority = 0 (bits 31,30)
  SCB->SHP[1] = SCB->SHP[1] |= (priority << 30);
  SysTick->CTRL = 0x07;
}

void Sound_Init(uint32_t p, uint32_t priority) {
  soundPt = 0;
  soundCount = 0;
  period = p;
  DAC5_Init();
  SysTick->CTRL = 0; // Disable SysTick initially
}

void Sound_Start(const uint8_t *pt, uint32_t count) {
  uint32_t status = SysTick->CTRL;
  if (pt == 0 || count == 0)
    return;
  soundPt = pt;
  soundCount = count;
  SysTick_IntArm(period, 0); // Start SysTick interrupts
}

extern "C" void SysTick_Handler(void) {
  if (soundCount > 0) {
    DAC5_Out(*soundPt); // Output the current value to DAC
    soundPt++;          // Move to the next sample
    soundCount--;       // Decrement the sample count
  } else {
    SysTick->CTRL = 0; // Disable SysTick when sound is finished
    soundPt = 0;       // Clear the pointer
  }
}

void Sound_Button(void) {
  // Sound_Start(MarioJumpSound, sizeof(MarioJumpSound));
  // Sound_Start(marioKartWinSound, sizeof(marioKartWinSound));
  Sound_Start(MarioJumpSound, 11000);
}

void Sound_Win(void) { Sound_Start(invaderkilled, 3377); }

void Sound_Shoot(void) { Sound_Start(shoot, 4080); }