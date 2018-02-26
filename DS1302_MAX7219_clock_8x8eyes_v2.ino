// DS1302_Serial_Easy (C)2010 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// Adopted for DS1302RTC library by Timur Maksimov 2014
//
// A quick demo of how to use my DS1302-library to
// quickly send time and date information over a serial link
//
// I assume you know how to connect the DS1302.
// DS1302:  CE pin    -> Arduino Digital 27
//          I/O pin   -> Arduino Digital 29
//          SCLK pin  -> Arduino Digital 31
//          VCC pin   -> Arduino Digital 33
//          GND pin   -> Arduino Digital 35

#include <TimeLib.h>
#include <DS1302RTC.h>
// Set pins:  CE, IO,CLK
DS1302RTC RTC(15, 4, 5);

// MAX7219
#include "LedControl.h" //  need the library
#define PIN_EYES_DIN 13
#define PIN_EYES_CS 12
#define PIN_EYES_CLK 14
LedControl lc = LedControl(PIN_EYES_DIN, PIN_EYES_CLK, PIN_EYES_CS, 2);
int ledintensity = 1; // LED Intensity 0-15

#include "fontTiny.h"
int present_minute = 0;
int previous_minute = 0;
int present_hour = 0;
int previous_hour = 0;
char minute_ones[4];
int c_minute_ones = 100;
char minute_tens[4];
int c_minute_tens = 100;
char hour_ones[4];
int c_hour_ones = 100;
char hour_tens[4];
int c_hour_tens = 100;
char temp_num[8];
char temp_char;

// rotation
bool rotateMatrix0 = 1;  // rotate 0 matrix by 180 deg
bool rotateMatrix1 = 1;  // rotate 1 matrix by 180 deg

// define eye ball without pupil
byte eyeBall[8] = {
  B00111100,
  B01111110,
  B11111111,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100
};

byte eyePupil = B11100111;

// stores current state of LEDs
byte eyeCurrent[8];
int currentX;
int currentY;
int cntLoop = 0;
int cntEffect = 0;

// min and max positions
#define MIN -2
#define MAX  2

// delays
#define DELAY_BLINK 40

// perform an effect every # of loop iterations, 0 to disable
#define EFFECT_ITERATION 4
int intensity = 1;

// Timer Interval
#include <SimpleTimer.h>
SimpleTimer timer;

void setup()
{
  Serial.begin(9600);
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.shutdown(1, false);
  lc.setIntensity(0, ledintensity); // sets brightness (0~15 possible values)
  lc.setIntensity(1, ledintensity);
  lc.clearDisplay(0);// clear screen
  lc.clearDisplay(1);
  Serial.println("");
  Serial.println("");

  if (RTC.haltRTC()) {
    Serial.println("The DS1302 is stopped.  Please run the SetTime");
    Serial.println("example to initialize the time and begin running.");
    Serial.println();
  }
  if (!RTC.writeEN()) {
    Serial.println("The DS1302 is write protected. This normal.");
    Serial.println();
  }

  // LED test
  // vertical line
  byte b = B10000000;
  for (int c = 0; c <= 7; c++)
  {
    for (int r = 0; r <= 7; r++)
    {
      setColumn(0, r, b);
      setColumn(1, r, b);
    }
    b = b >> 1;
    delay(50);
  }
  // full module
  b = B11111111;
  for (int r = 0; r <= 7; r++)
  {
    setColumn(0, r, b);
    setColumn(1, r, b);
  }
  delay(500);

  // clear both modules
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  delay(500);

  // random seed
  randomSeed(analogRead(0));

  // center eyes, crazy blink
  displayEyes(0, 0);
  delay(2000);
  blinkEyes(true, false);
  blinkEyes(false, true);
  delay(1000);

}

void display_minute_ones()
{
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_minute_ones][7 - cntr]; // copy to temp variable
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      minute_ones[col_cntr] = minute_ones[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      minute_ones[col_cntr] = minute_ones[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }

  //  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  //  {
  //    lc.setRow(0, 7 - col_cntr, minute_ones[col_cntr]); // 180deg mirror
  //  }

  char c_bit;
  for (int row_cntr = 0; row_cntr < 8; row_cntr++)
  {
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      lc.setLed(0, 7 - col_cntr, 7 - row_cntr, true );
    }
    delay(150);
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      // lc.setLed(0, 0, 0, true); // lower left
      // lc.setLed(0, 7, 0, true); // lower right
      c_bit = minute_ones[col_cntr] >> row_cntr;
      c_bit = c_bit & 0x01;
      lc.setLed(0, 7 - col_cntr, 7 - row_cntr, c_bit ); // lower left
    }
  }
}

void display_minute_tens()
{
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_minute_tens][7 - cntr]; // copy to temp variable
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      minute_tens[col_cntr] = minute_tens[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      minute_tens[col_cntr] = minute_tens[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }

  //  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  //  {
  //    lc.setRow(0, 3 - col_cntr, minute_tens[col_cntr]); // 180deg mirror
  //  }

  char c_bit;
  for (int row_cntr = 0; row_cntr < 8; row_cntr++)
  {
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      lc.setLed(0, 3 - col_cntr, 7 - row_cntr, true );
    }
    delay(150);
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      // lc.setLed(0, 0, 0, true); // lower left
      // lc.setLed(0, 7, 0, true); // lower right
      c_bit = minute_tens[col_cntr] >> row_cntr;
      c_bit = c_bit & 0x01;
      lc.setLed(0, 3 - col_cntr, 7 - row_cntr, c_bit ); // lower left
    }
  }

}

void display_hour_ones()
{
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_hour_ones][7 - cntr]; // copy to temp variable
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      hour_ones[col_cntr] = hour_ones[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      hour_ones[col_cntr] = hour_ones[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }

  //  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  //  {
  //    lc.setRow(1, 7 - col_cntr, hour_ones[col_cntr]); // 180deg mirror
  //  }

  char c_bit;
  for (int row_cntr = 0; row_cntr < 8; row_cntr++)
  {
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      lc.setLed(1, 7 - col_cntr, 7 - row_cntr, true );
    }
    delay(150);
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      // lc.setLed(0, 0, 0, true); // lower left
      // lc.setLed(0, 7, 0, true); // lower right
      c_bit = hour_ones[col_cntr] >> row_cntr;
      c_bit = c_bit & 0x01;
      lc.setLed(1, 7 - col_cntr, 7 - row_cntr, c_bit ); // lower left
    }
  }
}

void display_hour_tens()
{
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_hour_tens][7 - cntr]; // copy to temp variable
  }

  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      hour_tens[col_cntr] = hour_tens[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      hour_tens[col_cntr] = hour_tens[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }

  //  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  //  {
  //    lc.setRow(1, 3 - col_cntr, hour_tens[col_cntr]); // 180deg mirror
  //  }

  char c_bit;
  for (int row_cntr = 0; row_cntr < 8; row_cntr++)
  {
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      lc.setLed(1, 3 - col_cntr, 7 - row_cntr, true );
    }
    delay(150);
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      // lc.setLed(0, 0, 0, true); // lower left
      // lc.setLed(0, 7, 0, true); // lower right
      c_bit = hour_tens[col_cntr] >> row_cntr;
      c_bit = c_bit & 0x01;
      lc.setLed(1, 3 - col_cntr, 7 - row_cntr, c_bit ); // lower left
    }
  }
}

void display_RTC()
{
  tmElements_t tm;

  if (!RTC.read(tm)) {
    present_minute = tm.Minute;
    present_hour = tm.Hour;

    c_minute_ones = present_minute % 10;
    display_minute_ones();
    c_minute_tens = present_minute / 10;
    display_minute_tens();
    c_hour_ones = present_hour % 10;
    display_hour_ones();
    c_hour_tens = present_hour / 10;
    display_hour_tens();
  } else {
    Serial.println("DS1302 read error!  Please check the circuitry.");
    Serial.println();
    delay(9000);
  }

}

void loop()
{
  // move to random position, wait random time
  moveEyes(random(MIN, MAX + 1), random(MIN, MAX + 1), 50);
  delay(random(5, 7) * 500);

  // blink time?
  if (random(0, 5) == 0)
  {
    delay(500);
    blinkEyes();
    delay(500);
  }

  // effect time?
  if (EFFECT_ITERATION > 0)
  {
    cntLoop++;
    if (cntLoop == EFFECT_ITERATION)
    {
      cntLoop = 0;
      if (cntEffect > 13) cntEffect = 0;
      switch (cntEffect)
      {
        case 0: // cross eyes
          crossEyes();
          delay(1000);
          break;

        case 1: // display Time
          display_RTC();
          delay(20000);
          break;
          
        case 2: // round spin
          roundSpin(2);
          delay(1000);
          break;

        case 3: // display Time
          display_RTC();
          delay(20000);
          break;
          
        case 4: // crazy spin
          crazySpin(2);
          delay(1000);
          break;

        case 5: // display Time
          display_RTC();
          delay(20000);
          break;
          
        case 6: // meth eyes
          methEyes();
          delay(1000);
          break;

        case 7: // display Time
          display_RTC();
          delay(20000);
          break;
          
        case 8: // lazy eye
          lazyEye();
          delay(1000);
          break;

        case 9: // display Time
          display_RTC();
          delay(20000);
          break;
          
        case 10: // crazy blink
          blinkEyes(true, false);
          blinkEyes(false, true);
          delay(1000);
          break;

        case 11: // display Time
          display_RTC();
          delay(20000);
          break;
          
        case 12: // glow
          glowEyes(3);
          delay(1000);
          break;

        case 13: // display Time
          display_RTC();
          delay(20000);
          break;

        default:
          break;
      }
      cntEffect++;
    }
  }
}

void blinkEyes()
{
  blinkEyes(true, true);
}

/*
  This method blinks eyes as per provided params
*/
void blinkEyes(boolean blinkLeft, boolean blinkRight)
{
  // blink?
  if (!blinkLeft && !blinkRight)
    return;

  // close eyelids
  for (int i = 0; i <= 3; i++)
  {
    if (blinkLeft)
    {
      setColumn(0, i, 0);
      setColumn(0, 7 - i, 0);
    }
    if (blinkRight)
    {
      setColumn(1, i, 0);
      setColumn(1, 7 - i, 0);
    }
    delay(DELAY_BLINK);
  }

  // open eyelids
  for (int i = 3; i >= 0; i--)
  {
    if (blinkLeft)
    {
      setColumn(0, i, eyeCurrent[i]);
      setColumn(0, 7 - i, eyeCurrent[7 - i]);
    }
    if (blinkRight)
    {
      setColumn(1, i, eyeCurrent[i]);
      setColumn(1, 7 - i, eyeCurrent[7 - i]);
    }
    delay(DELAY_BLINK);
  }
}

/*
  This methods moves eyes to center position,
  then moves horizontally with wrapping around edges.
*/
void crazySpin(int times)
{
  if (times == 0)
    return;

  moveEyes(0, 0, 50);
  delay(500);

  byte row = eyePupil;
  for (int t = 0; t < times; t++)
  {
    // spin from center to L
    for (int i = 0; i < 5; i++)
    {
      row = row >> 1;
      row = row | B10000000;
      setColumn(0, 3, row);  setColumn(1, 3, row);
      setColumn(0, 4, row);  setColumn(1, 4, row);
      delay(50);
      if (t == 0)
        delay((5 - i) * 10); // increase delay on 1st scroll (speed up effect)
    }
    // spin from R to center
    for (int i = 0; i < 5; i++)
    {
      row = row >> 1;
      if (i >= 2)
        row = row | B10000000;
      setColumn(0, 3, row);  setColumn(1, 3, row);
      setColumn(0, 4, row);  setColumn(1, 4, row);
      delay(50);
      if (t == (times - 1))
        delay((i + 1) * 10); // increase delay on last scroll (slow down effect)
    }
  }
}

/*
  This method crosses eyes
*/
void crossEyes()
{
  moveEyes(0, 0, 50);
  delay(500);

  byte pupilR = eyePupil;
  byte pupilL = eyePupil;

  // move pupils together
  for (int i = 0; i < 2; i++)
  {
    pupilR = pupilR >> 1;
    pupilR = pupilR | B10000000;
    pupilL = pupilL << 1;
    pupilL = pupilL | B1;

    setColumn(0, 3, pupilR); setColumn(1, 3, pupilL);
    setColumn(0, 4, pupilR); setColumn(1, 4, pupilL);

    delay(100);
  }

  delay(2000);

  // move pupils back to center
  for (int i = 0; i < 2; i++)
  {
    pupilR = pupilR << 1;
    pupilR = pupilR | B1;
    pupilL = pupilL >> 1;
    pupilL = pupilL | B10000000;

    setColumn(0, 3, pupilR); setColumn(1, 3, pupilL);
    setColumn(0, 4, pupilR); setColumn(1, 4, pupilL);

    delay(100);
  }
}

/*
  This method displays eyeball with pupil offset by X, Y values from center position.
  Valid X and Y range is [MIN,MAX]
  Both LED modules will show identical eyes
*/
void displayEyes(int offsetX, int offsetY)
{
  // ensure offsets are  in valid ranges
  offsetX = getValidValue(offsetX);
  offsetY = getValidValue(offsetY);

  // calculate indexes for pupil rows (perform offset Y)
  int row1 = 3 - offsetY;
  int row2 = 4 - offsetY;

  // define pupil row
  byte pupilRow = eyePupil;

  // perform offset X
  // bit shift and fill in new bit with 1
  if (offsetX > 0) {
    for (int i = 1; i <= offsetX; i++)
    {
      pupilRow = pupilRow >> 1;
      pupilRow = pupilRow | B10000000;
    }
  }
  else if (offsetX < 0) {
    for (int i = -1; i >= offsetX; i--)
    {
      pupilRow = pupilRow << 1;
      pupilRow = pupilRow | B1;
    }
  }

  // pupil row cannot have 1s where eyeBall has 0s
  byte pupilRow1 = pupilRow & eyeBall[row1];
  byte pupilRow2 = pupilRow & eyeBall[row2];

  // display on LCD matrix, update to eyeCurrent
  for (int r = 0; r < 8; r++)
  {
    if (r == row1)
    {
      setColumn(0, r, pupilRow1);
      setColumn(1, r, pupilRow1);
      eyeCurrent[r] = pupilRow1;
    }
    else if (r == row2)
    {
      setColumn(0, r, pupilRow2);
      setColumn(1, r, pupilRow2);
      eyeCurrent[r] = pupilRow2;
    }
    else
    {
      setColumn(0, r, eyeBall[r]);
      setColumn(1, r, eyeBall[r]);
      eyeCurrent[r] = eyeBall[r];
    }
  }

  // update current X and Y
  currentX = offsetX;
  currentY = offsetY;
}

/*
  This method corrects provided coordinate value
*/
int getValidValue(int value)
{
  if (value > MAX)
    return MAX;
  else if (value < MIN)
    return MIN;
  else
    return value;
}

/*
  This method pulsates eye (changes LED brightness)
*/
void glowEyes(int times)
{
  for (int t = 0; t < times; t++)
  {
    for (int i = 2; i <= 8; i++)
    {
      lc.setIntensity(0, i);
      lc.setIntensity(1, i);
      delay(50);
    }

    delay(250);

    for (int i = 7; i >= 1; i--)
    {
      lc.setIntensity(0, i);
      lc.setIntensity(1, i);
      delay(25);
    }

    delay(150);
  }
  lc.setIntensity(0, intensity);
  lc.setIntensity(1, intensity);
}

/*
  This method moves eyes to center, out and then back to center
*/
void methEyes()
{
  moveEyes(0, 0, 50);
  delay(500);

  byte pupilR = eyePupil;
  byte pupilL = eyePupil;

  // move pupils out
  for (int i = 0; i < 2; i++)
  {
    pupilR = pupilR << 1;
    pupilR = pupilR | B1;
    pupilL = pupilL >> 1;
    pupilL = pupilL | B10000000;

    setColumn(0, 3, pupilR); setColumn(1, 3, pupilL);
    setColumn(0, 4, pupilR); setColumn(1, 4, pupilL);

    delay(100);
  }

  delay(2000);

  // move pupils back to center
  for (int i = 0; i < 2; i++)
  {
    pupilR = pupilR >> 1;
    pupilR = pupilR | B10000000;
    pupilL = pupilL << 1;
    pupilL = pupilL | B1;

    setColumn(0, 3, pupilR); setColumn(1, 3, pupilL);
    setColumn(0, 4, pupilR); setColumn(1, 4, pupilL);

    delay(100);
  }
}

/*
  This method moves both eyes from current position to new position
*/
void moveEyes(int newX, int newY, int stepDelay)
{
  // set current position as start position
  int startX = currentX;
  int startY = currentY;

  // fix invalid new X Y values
  newX = getValidValue(newX);
  newY = getValidValue(newY);

  // eval steps
  int stepsX = abs(currentX - newX);
  int stepsY = abs(currentY - newY);

  // need to change at least one position
  if ((stepsX == 0) && (stepsY == 0))
    return;

  // eval direction of movement, # of steps, change per X Y step, perform move
  int dirX = (newX >= currentX) ? 1 : -1;
  int dirY = (newY >= currentY) ? 1 : -1;
  int steps = (stepsX > stepsY) ? stepsX : stepsY;
  int intX, intY;
  float changeX = (float)stepsX / (float)steps;
  float changeY = (float)stepsY / (float)steps;
  for (int i = 1; i <= steps; i++)
  {
    intX = startX + round(changeX * i * dirX);
    intY = startY + round(changeY * i * dirY);
    displayEyes(intX, intY);
    delay(stepDelay);
  }
}

/*
  This method lowers and raises right pupil only
*/
void lazyEye()
{
  moveEyes(0, 1, 50);
  delay(500);

  // lower left pupil slowly
  for (int i = 0; i < 3; i++)
  {
    setColumn(1, i + 2, eyeBall[i + 2]);
    setColumn(1, i + 3, eyeBall[i + 3] & eyePupil);
    setColumn(1, i + 4, eyeBall[i + 4] & eyePupil);
    delay(150);
  }

  delay(1000);

  // raise left pupil quickly
  for (int i = 0; i < 3; i++)
  {
    setColumn(1, 4 - i, eyeBall[4 - i] & eyePupil);
    setColumn(1, 5 - i, eyeBall[5 - i] & eyePupil);
    setColumn(1, 6 - i, eyeBall[6 - i]);
    delay(25);
  }
}

/*
  This method spins pupils clockwise
*/
void roundSpin(int times)
{
  if (times == 0)
    return;

  moveEyes(2, 0, 50);
  delay(500);

  for (int i = 0; i < times; i++)
  {
    displayEyes(2, -1); delay(40); if (i == 0) delay(40);
    displayEyes(1, -2); delay(40); if (i == 0) delay(30);
    displayEyes(0, -2); delay(40); if (i == 0) delay(20);
    displayEyes(-1, -2); delay(40); if (i == 0) delay(10);
    displayEyes(-2, -1); delay(40);
    displayEyes(-2, 0); delay(40);
    displayEyes(-2, 1); delay(40); if (i == (times - 1)) delay(10);
    displayEyes(-1, 2); delay(40); if (i == (times - 1)) delay(20);
    displayEyes(0, 2); delay(40); if (i == (times - 1)) delay(30);
    displayEyes(1, 2); delay(40); if (i == (times - 1)) delay(40);
    displayEyes(2, 1); delay(40); if (i == (times - 1)) delay(50);
    displayEyes(2, 0); delay(40);
  }
}

/*
  This method sets values to matrix row
  Performs 180 rotation if needed
*/
void setColumn(int addr, int row, byte rowValue)
{
  if (((addr == 0) && (rotateMatrix0)) || (addr == 1 && rotateMatrix1))
  {
    row = abs(row - 7);
    //rowValue = bitswap(rowValue);
  }
  lc.setColumn(addr, row, rowValue);
}

/*
  void display_time_1000ms()
  {
  display_minute_ones();
  display_minute_tens();
  display_hour_ones();
  display_hour_tens();
  }
  void display_minute_ones()
  {
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_minute_ones][7 - cntr]; // copy to temp variable
  }
  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      minute_ones[col_cntr] = minute_ones[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      minute_ones[col_cntr] = minute_ones[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }
  //  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  //  {
  //    // lc.setRow(0, 7 - col_cntr, minute_ones[3 - col_cntr]); // 180 deg
  //    // lc.setRow(0, col_cntr + 4, minute_ones[col_cntr]); // 180 deg
  //    lc.setRow(1, 3 - col_cntr, hour_tens[col_cntr]); // 180deg mirror
  //  }
  char c_bit;
  for (int row_cntr = 0; row_cntr < 8; row_cntr++)
  {
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      lc.setLed(0, 7 - col_cntr, 7 - row_cntr, true );
    }
    delay(150);
    for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
    {
      // lc.setLed(0, 0, 0, true); // lower left
      // lc.setLed(0, 7, 0, true); // lower right
      c_bit = minute_ones[col_cntr] >> row_cntr;
      c_bit = c_bit & 0x01;
      lc.setLed(0, 7 - col_cntr, 7 - row_cntr, c_bit ); // lower left
    }
  }
  }
  void display_minute_tens()
  {
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_minute_tens][7 - cntr]; // copy to temp variable
  }
  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      minute_tens[col_cntr] = minute_tens[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      minute_tens[col_cntr] = minute_tens[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }
  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    lc.setRow(0, 3 - col_cntr, minute_tens[col_cntr]); // 180deg mirror
  }
  }
  void display_hour_ones()
  {
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_hour_ones][7 - cntr]; // copy to temp variable
  }
  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      hour_ones[col_cntr] = hour_ones[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      hour_ones[col_cntr] = hour_ones[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }
  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    lc.setRow(1, 7 - col_cntr, hour_ones[col_cntr]); // 180deg mirror
  }
  }
  void display_hour_tens()
  {
  for (int cntr = 0; cntr < 7; cntr++)
  {
    temp_num[cntr] = tinyFont[c_hour_tens][7 - cntr]; // copy to temp variable
  }
  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    for (int row_cntr = 0; row_cntr < 8; row_cntr++)
    {
      hour_tens[col_cntr] = hour_tens[col_cntr] << 1; // initial shift
      temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
      hour_tens[col_cntr] = hour_tens[col_cntr] | temp_char; // AND the bit to
      temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
    }
  }
  for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  {
    lc.setRow(1, 3 - col_cntr, hour_tens[col_cntr]); // 180deg mirror
  }
  }
  // for debugging
  //for (int cntr = 0; cntr < 8; cntr++)
  //{
  //  minute_ones[0] = minute_ones[0] << 1;
  //  Serial.print("minute_ones[0] << 1        :"); Serial.println(minute_ones[0], BIN);
  //  minute_ones[0] = minute_ones[0] & 0xfe;
  //  Serial.print("minute_ones[0] & 0xfe      :"); Serial.println(minute_ones[0], BIN);
  //  temp_char = temp_num[cntr] & 0x01;
  //  Serial.print("temp_num[cntr] & 0x01      :"); Serial.println(temp_char, BIN);
  //  minute_ones[0] = minute_ones[0] | temp_char;
  //  Serial.print("minute_ones[0] | temp_char :"); Serial.println(minute_ones[0], BIN);
  //  Serial.println("*******************");
  //  Serial.println(minute_ones[0], BIN);
  //}
  //for (int col_cntr = 0; col_cntr < 4 ; col_cntr++)
  //{
  //  for (int row_cntr = 0; row_cntr < 8; row_cntr++)
  //  {
  //    minute_ones[col_cntr] = minute_ones[col_cntr] << 1; // initial shift
  //    minute_ones[col_cntr] = minute_ones[col_cntr] & 0xfe; // set zero the last bit because if last no is 1, it will be 1 also
  //    temp_char = temp_num[row_cntr] & 0x01; // check the bit if 0 or 1
  //    minute_ones[col_cntr] = minute_ones[col_cntr] | temp_char; // AND the bit to
  //    temp_num[row_cntr] = temp_num[row_cntr] >> 1; // shift the array right and
  //  }
  //  Serial.print("col_cntr  :"); Serial.println(col_cntr);
  //  Serial.print("minute_ones[col_cntr] :"); Serial.println(minute_ones[col_cntr], BIN);
  //}
*/
