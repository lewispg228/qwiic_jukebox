/*
   Proving out the "IR binary card method"
   4 IR sensors (in a row) are looking for light/dark readings
   They are located inside a "pocket" where you can slide a card into it with markings on one side.
   a cardboard "key" is placed over the IR sensors, with black/white pattern of a binary number.
   Reading which one's are "dark" we can see which card is preset.
   Each card has 4 spots on it, and you can draw in with a dark marker the binary number.
*/

boolean checkIRID()
{
  // pull in 4 readings
  // make sure they are the same.
  // adjust global track variable.
  // return true.

  byte trackReadings[] = {0, 0, 0, 0}; // used to store 4 sequencial readings.

  //Serial.print("trackReadings:");
  for (byte i = 0 ; i < 4 ; i++)
  {
    readLightSensors();
    trackReadings[i] = valuesToNumber();
    //Serial.print(trackReadings[i]);
    delay(100);
  }
  //Serial.println();
  if (
    (trackReadings[0] != track) &&    // only report changes (i.e. something new)
    (trackReadings[0] != 0) &&        // "0" means there is no card in there.
    (trackReadings[0] == trackReadings[1]) &&
    (trackReadings[0] == trackReadings[2]) &&
    (trackReadings[0] == trackReadings[3])
  )
  {
    //Serial.println("four in a row");
    track = trackReadings[0];
    return true;
  }
  else
  {
    //Serial.println("error");
    return false;
  }
}

void readLightSensors()
{
  for (int i = 0 ; i < 4 ; i++)
  {
    lightValues[i] = readQD(lightPins[i]);
  }
}

void printLightValues()
{
  Serial.print("LightValues: ");
  for (int i = 0 ; i < 4 ; i++)
  {
    Serial.print(lightValues[i]);
    Serial.print("\t");
  }
  Serial.print(track, DEC);
  Serial.println();
}

// looks at all light values, treats them as bits, and returns a number
byte valuesToNumber()
{

  byte number = 0; // number to return

  // get max
  //int maxVal = lightValues[0]; // start with first value, then check if any others are greater
  //for (int i = 0 ; i < 4 ; i++)
  // {
  //  if (lightValues[i] > maxVal) maxVal = lightValues[i];
  //}

  //Serial.print("\n\rmaxVal: ");
  //Serial.println(maxVal);

  // create threshhold
  //int threshhold = ( maxVal - (maxVal / 3) );
  int threshhold = 400;
  //Serial.print("\n\rthreshhold: ");
  //Serial.println(threshhold);

  for (int i = 0 ; i < 4 ; i++)
  {
    if (lightValues[i] < threshhold)
    {
      number |= (1 << i);  // set the bit
    }
  }
  //  track = number;
  return number;
}


int readQD(byte QRE1113_Pin)
{
  //Returns value from the QRE1113
  //Lower numbers mean more refleacive
  //More than 3000 means nothing was reflected.
  pinMode( QRE1113_Pin, OUTPUT );
  digitalWrite( QRE1113_Pin, HIGH );
  delayMicroseconds(10);
  pinMode( QRE1113_Pin, INPUT );

  long time = micros();

  //time how long the input is HIGH, but quit after 3ms as nothing happens after that
  int timeout = 1500;
  while (timeout)
  {
    if (digitalRead(QRE1113_Pin) == LOW)
    {
      break;
    }
    timeout--;
  }
  int diff = (micros() - time);
  return diff;
}
