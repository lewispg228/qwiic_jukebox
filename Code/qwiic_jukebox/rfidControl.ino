// All the stuff we need to talk to the qwiic rfID reader

#define RFID_ADDR 0x7D // Default I2C address 
#define TAG_REQUEST 6

// 20 tags can be stored by the product at a time, the first one to be scanned is the first one
// to be pulled from the tag stack. If the tag reads '000000' or the interrupt line never went low
// then there are no new tags waiting to be read.
// Pete's mod here - returns true if a valid RFID tag is read (that matches one of our tagList Strings).
boolean checkTagID()
{
  byte tempTag = 0;
  // This variable stores the tag and could easily be a global variable. Just
  // make sure to clear it in between reads.
  String tagID;

  Wire.requestFrom((uint8_t)RFID_ADDR, TAG_REQUEST);
  for ( int x = 0; x < TAG_REQUEST; x++ ) {
    tempTag = Wire.read();
    // Concatenating the bytes onto the end of "tagID".
    tagID += String(tempTag);
  }
//  if (tagID != "000000")
  if (tagID != tagList[0])
  {
    Serial.print("RFID Tag ID: ");
    Serial.println(tagID);
    track = tagToTrackNumber(tagID);
    Serial.println(track);
    return true;
  }
  return false; // If we get here, then the RFID tag does not exhist in our tagList. 
                // It's probably a new RFID tag.
}

byte tagToTrackNumber(String tag)
{
  for (byte i = 1 ; i < sizeof(tagList) ; i++)
  {
    if ( tag == tagList[i] ) return i;
  }
  return 0;
}
