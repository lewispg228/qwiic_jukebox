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

#include <Wire.h>

byte mp3Address = 0x37; //Unshifted 7-bit default address for Qwiic MP3

byte track = 0; // global variable to stroe which track number we'd like to play, 
                // this is updated when we see a new rfid tag come in.
void setup()
{
  Serial.begin(9600);

  Wire.begin();

  //Check to see if MP3 is present
  if (mp3IsPresent() == false)
  {
    Serial.println("Qwiic MP3 failed to respond. Please check wiring and possibly the I2C address. Freezing...");
    while (1);
  }

  mp3ChangeVolume(15); //Volume can be 0 (off) to 31 (max)

  Serial.print("Song count: ");
  Serial.println(mp3SongCount());

  Serial.println("Press S to stop or P to pause.");

  //mp3PlayFile(3); //Play file F003.mp3
}

void loop()
{
  if (checkTagID() == true) // returns true if a new RFID tag has been placed, and it is found in tagList
  {
    Serial.println("gonna play a track now...");
    mp3Stop();
    delay(100);
    mp3PlayFile(track); //Play the new track (for example, if track = 3, then "F003.mp3")
  }
  delay(1000); // Slow it down

  if (Serial.available() == true) //pull in any serial that arrives
  {
    byte incoming = Serial.read();

    switch (incoming)
    {
      case 'S':
      case 's':
        mp3Stop();
        break;
      case 'P':
      case 'p':
        mp3Pause(); //Pause, or play from pause, the current track
        break;
      default:
        Serial.print("Unknown character: ");
        Serial.write(incoming);
        Serial.println();
        break;
    }
  }
}
