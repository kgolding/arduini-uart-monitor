#include <Arduino.h>

#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

#include <SoftwareSerial.h>
SoftwareSerial rs485(10, 11);

#define COLS 15
#define ROWS 6
char rx[ROWS][COLS + 1]; // 0x00 on the end

void setup()
{
  Serial.begin(9600);
  Serial.println("RS485 Monitor by Kevin Golding");

  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0, 0, " RS485 Monitor");
  u8x8.drawString(0, 1, "---------------");

  rs485.begin(9600);

  // Initiliase with spaces
  for (int row = 0; row < ROWS; row++)
  {
    for (int col = 0; col < COLS; col++)
    {
      rx[row][col] = 0x20;
    }
  }
}

int row = 0;
int col = -1;

void loop()
{
  while (rs485.available() > 0)
  {
    // Scroll by row
    col++;
    if (col == COLS)
    {
      row++;
      if (row == ROWS)
      { // Shift rows up
        row = ROWS - 1; // Last row
        for (int r = 1; r < ROWS; r++)
        {
          for (int c = 0; c < COLS; c++)
          {
            rx[r - 1][c] = rx[r][c];
          }
          u8x8.setCursor(0, r + 2);
          u8x8.print(rx[r]);
        }
        // Blank out last row
        for (int c = 0; c < COLS; c++)
        {
          rx[ROWS - 1][c] = 0x20;
        }
      }
      col = 0; // First col
    }

    // Read a byte
    rx[row][col] = rs485.read();

    // Write to USB serial
    Serial.print(rx[row][col]);

    // Replace control chars with a dot
    if (rx[row][col] < 0x20) {
      rx[row][col] = '.';
    }

    // Display the current row
    u8x8.setCursor(0, row + 2);
    u8x8.print(rx[row]);

    // Scroll char by char - too slow!
    // for (int row = 0; row < ROWS; row++)
    // {
    //   for (int col = 0; col < COLS; col++)
    //     if (col == 0)
    //     {
    //       if (row > 0)
    //       {
    //         rx[row - 1][COLS - 1] = rx[row][col]; // Shift up and to the end
    //       }
    //     }
    //     else
    //     {
    //       rx[row][col - 1] = rx[row][col]; // Shift left
    //     }
    // }

    // rx[ROWS - 1][COLS - 1] = rs485.read();
    // // Serial.print(b);

    // for (int row = 0; row < ROWS; row++)
    // {
    //   u8x8.setCursor(0, row + 2);
    //   u8x8.print(rx[row]);
    // };
  }
}