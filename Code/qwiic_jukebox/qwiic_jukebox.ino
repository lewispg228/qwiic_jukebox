/*
  Controlling the Qwiic MP3 Trigger with I2C Commands
  By: Nathan Seidle
  SparkFun Electronics
  Date: April 23rd, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  This example demonstrates how to pause or stop an MP3. If a track is playing then pausing
  will pause at that spot. Pressing pause again will continuing playing from that spot. If
  no track is playing and pause is pressed then the last played track will be started from
  the beginning.

  Note: For this example you will need to load an MP3 onto the device and name the file F003.mp3. You can
  use a microB cable to load a file directly onto the SD card.

  Hardware Connections:
  Plug in headphones
  Make sure the SD card is in the socket
  Don't have a USB microB cable connected right now
  If needed, attach a Qwiic Shield to your Arduino/Photon/ESP32 or other
  Plug the Qwiic device onto an available Qwiic port
  Open the serial monitor at 9600 baud
*/

/*
  SparkFun Qwiic RFID board fot the ID-xxLA Modules
  By: Elias Santistevan
  Sparkfun Electronics
  Date: February, 2018
  License: This code is public domain but if you use this and we meet someday, get me a beer!

  Feel like supporting our work? Buy a board from Sparkfun!
  https://www.sparkfun.com/products/15191

  This example requests an RFID tag's ID when the interrupt pin from the
  SparkFun Qwiic RFID board goes LOW indicating a tag has been scanned.
*/

// also some code by graham

#define GLOBAL_DEBUG 1

#include <Wire.h>

#include "SparkFun_Qwiic_MP3_Trigger_Arduino_Library.h" //http://librarymanager/All#SparkFun_MP3_Trigger
MP3TRIGGER mp3;

byte track = 0; // global variable to store which track number we'd like to play,
// this is updated when we see a new rfid tag come in.

int playPin = A0;
int stopPin = A2;

// statuses used to know how to react to play and stop buttons
#define STOPPED 0
#define PAUSED 1
#define PLAYING 2
byte jukebox_status = STOPPED;

boolean paused = false;

byte songCount = 0; // set in setup, by caling mp3.getSongCount()
// used to ensure we only send valid file numbers to play


// the 4 IR sensors
byte lightPins[] = {7, 6, 5, 4};

// store the light values in an array
int lightValues[] = {0, 0, 0, 0};

// count how many reads of the same number, to debounce IR readings
int sameReadCount = 0;
int previousRead = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Jukebox booting up...");
  delay(3000); // wait for Qwiic MP3 board to bootup

  pinMode(playPin, INPUT_PULLUP);
  pinMode(stopPin, INPUT_PULLUP);

  for (int i = 0 ; i < 4 ; i++)
  {
    pinMode(lightPins[i], INPUT);
  }

  pinMode(3, OUTPUT);    // GND
  digitalWrite(3, LOW);
  pinMode(2, OUTPUT);    // VCC
  digitalWrite(2, HIGH);

  Wire.begin();

  //Check to see if MP3 is present
  if (mp3.begin() == false)
  {
    Serial.println("Qwiic MP3 failed to respond. Please check wiring and possibly the I2C address. Freezing...");
    while (1);
  }

  mp3.setVolume(15); //Volume can be 0 (off) to 31 (max)
  Serial.println("Qwiic JukeBox");
  delay(10); // if no serial, need to wait

  Serial.print("Song count: ");
  songCount = mp3.getSongCount();
  delay(10); // if no serial, need to wait
}

void loop()
{
  if (
    (checkTagID() == true) // returns true if a new RFID tag has been placed, and it is found in tagList
    ||
    (checkIRID() == true) // returns true if new IR card has been placed in pocket reader, updates global track variable if non-zero.
  )
  {
    Serial.println("new tag detected");
    Serial.print("track: ");
    Serial.println(track, DEC);
    delay(10); // if no serial, need to wait
    switch (jukebox_status)
    {
      case STOPPED: // ignore
        break;
      case PAUSED:
        {
          mp3.stop();
          jukebox_status = STOPPED;
          break;
        }
      case PLAYING:
        {
          mp3.stop();
          jukebox_status = STOPPED;
          break;
        }
    }
    mp3.playFile(0); // "bing"
    delay(500);
  }

  if (digitalRead(playPin) == LOW)
  {
    Serial.println("play pressed");
    delay(10); // if no serial, need to wait
    switch (jukebox_status)
    {
      case STOPPED:
        {
          // ensure valid track number
          if ((track != 0) && (track <= songCount))
          {
            mp3.playFile(track); // start playing track from beginning
            jukebox_status = PLAYING;
          }
          break;
        }
      case PAUSED:
        {
          mp3.pause(); // resume with pause funtion
          jukebox_status = PLAYING;
          break;
        }
      case PLAYING:
        break; // ignore if we are already playing
    }
  }
  else if (digitalRead(stopPin) == LOW)
  {
    Serial.println("stop pressed");
    delay(10); // if no serial, need to wait
    switch (jukebox_status)
    {
      case STOPPED:
        break; // ignore if we are already stopped.
      case PAUSED:
        break; // ignore if we are already paused.
      case PLAYING:
        {
          // first send pause command, then find out if we want to stop or leave as paused
          mp3.pause();
          jukebox_status = PAUSED;
          while (digitalRead(stopPin) == LOW) // wait for release
            delay(100); // debounce

          // button has been released
          // count how long it's been HIGH, if too long, then consider it a "single press" (aka STOP)
          int counter = 0;
          while (digitalRead(stopPin) == HIGH)
          {
            counter++;
            Serial.println(counter);
            delay(10);
            if (counter >= 100)
            {
              Serial.println("timed out on double-press. STOPPED");
              mp3.stop();
              jukebox_status = STOPPED;
              break;
            }
          }
          if (counter < 100)
          {
            Serial.println("double-press detected. PAUSED");
            jukebox_status = PAUSED;
          }
          break;
        }
    }
  }
  if (GLOBAL_DEBUG)
  {
    Serial.print("\ttrack: ");
    Serial.println(track);
  }
  delay(50); // debounce
}
