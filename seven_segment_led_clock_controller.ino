/*
 * The following code has been brought to you by Jeremiah Duke aka TheScriptKeeper
 * 
 * Last updated March 15th, 2022
 * 
 * Arduino UNO
 * DS3234 RTC Module
 * M5451 LED Display Driver
 * 
 * Loosely based on the following SparkFun writeup
 * https://learn.sparkfun.com/tutorials/deadon-rtc-breakout-hookup-guide/all
 * 
 * Arduino Time library used
 * https://github.com/PaulStoffregen/Time
 * 
 * Seven segment display wiring diagram for M5451
 * https://github.com/supercrab/arduino-seven-segment/blob/master/Wiring%20Guides/M5450%20M5451.pdf
 * 
 * Arduino Seven Segment library used
 * https://github.com/supercrab/arduino-seven-segment
 */


#include <SevenSegment.h>
#include <SPI.h>
#include <SparkFunDS3234RTC.h>
#include <TimeLib.h>
#include <Time.h>

#define CLOCK 3 // Clock pin
#define DATA 4   // Data pin
#define DS3234_CS_PIN 10 // Chip Select pin for RTC

#define PRINT_USA_DATE // Enables USA date format for serial printing

// Set the following line true to print the date/time to the serial output for debugging
#define PRINT_TO_SERIAL_DISPLAY false

// Setup seven segment
SevenSegment screen(CLOCK, DATA);

void setup(){
#if PRINT_TO_SERIAL_DISPLAY
    Serial.begin(9600);
#endif
    screen.begin("M5451","8.8|8.8.");
    
    rtc.begin(DS3234_CS_PIN);
    rtc.set24Hour();
    
    // This line is used to initially set the time on the RTC - After setting the time, comment this line as an RTC only needs the time set once if it contains an onboard battery
    //rtc.setTime(00, 55, 19, 3, 15, 3, 22);
    
}

void loop(){
#if PRINT_TO_SERIAL_DISPLAY
    printTime();
#endif
    digitalClockDisplay();

    // One second delay on main loop to blink colon and update time as needed
    delay(1000);
}

// Display the time on the clock
void digitalClockDisplay(){
  rtc.update();
  static boolean showColon;
  static char buf[9];
    
  // Create time string, display a colon or not?
  if (!showColon)
    sprintf(buf, "%02d%02d\0", rtc.hour()%12, rtc.minute());
  else
    sprintf(buf, "%02d:%02d\0", rtc.hour()%12, rtc.minute());

  // Display the time on our screen
  screen.print(buf);

  // Toggle display of colon
  showColon = !showColon;
}

// Print the date and time to the serial output for debugging - Only fires if PRINT_TO_SERIAL_DISPLAY is set to true above
void printTime()
{
  Serial.print(String(rtc.hour()) + ":"); // Print hour
  if (rtc.minute() < 10)
    Serial.print('0'); // Print leading '0' for minute
  Serial.print(String(rtc.minute()) + ":"); // Print minute
  if (rtc.second() < 10)
    Serial.print('0'); // Print leading '0' for second
  Serial.print(String(rtc.second())); // Print second

  if (rtc.is12Hour()) // If we're in 12-hour mode
  {
    // Use rtc.pm() to read the AM/PM state of the hour
    if (rtc.pm()) Serial.print(" PM"); // Returns true if PM
    else Serial.print(" AM");
  }
  
  Serial.print(" | ");

  // Few options for printing the day, pick one:
  Serial.print(rtc.dayStr()); // Print day string
  //Serial.print(rtc.dayC()); // Print day character
  //Serial.print(rtc.day()); // Print day integer (1-7, Sun-Sat)
  Serial.print(" - ");
#ifdef PRINT_USA_DATE
  Serial.print(String(rtc.month()) + "/" +   // Print month
                 String(rtc.date()) + "/");  // Print date
#else
  Serial.print(String(rtc.date()) + "/" +    // (or) print date
                 String(rtc.month()) + "/"); // Print month
#endif
  Serial.println(String(rtc.year()));        // Print year
}
