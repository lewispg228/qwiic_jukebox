/*
   Proving out the "IR binary card method"
   4 IR sensors (in a row) are looking for light/dark readings
   They are located inside a "pocket" where you can slide a card into it with markings on one side.
   a cardboard "key" is placed over the IR sensors, with black/white pattern of a binary number.
   Reading which one's are "dark" we can see which card is preset.
   Each card has 4 spots on it, and you can draw in with a dark marker the binary number.
*/

#define REQUIRED_SAME_READ_COUNT 8

boolean checkIRID()
{
  // take one reading
  // compare it to previousRead. if the same, increment sameReadCount.
  // if sameReadCount == REQUIRED_SAME_READ_COUNT, then...
  // adjust global track variable.
  // return true.

  byte currentRead = 0;

  readLightSensors();
  currentRead = valuesToNumber();

  if (GLOBAL_DEBUG)
  {
    Serial.print("p: ");
    Serial.print(previousRead);
    Serial.print("\tc: ");
    Serial.print(currentRead);
  }

  if ( previousRead == currentRead && currentRead != 0) sameReadCount++; // if they are the same and non-zero
  else sameReadCount = 0; // reset

  if (GLOBAL_DEBUG)
  {
    Serial.print("\tsameReadCount: ");
    Serial.print(sameReadCount);
  }

  previousRead = currentRead; // remember for the check next time we read

  if (
    (currentRead != 0) &&                       // ignore 0 (no card)
    (currentRead != track) &&                   // only report changes (i.e. something new)
    (sameReadCount == REQUIRED_SAME_READ_COUNT) // only report if we've seen enough of the same reads
  )
  {
    sameReadCount = 0; // reset.
    track = currentRead; // set global variable to be called from main loop to play a track.
    return true;
  }

  if (sameReadCount > REQUIRED_SAME_READ_COUNT) sameReadCount = REQUIRED_SAME_READ_COUNT; // max out, so we don't risk rolling over
  return false; // if we get here, then we didn't yet see a good reading.
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
  Serial.print("\tvals: ");
  for (int i = 0 ; i < 4 ; i++)
  {
    Serial.print("\t");
    Serial.print(lightValues[i]);
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
  // ensure a good conversion. exlude 0 and 15.
  if ((number > 0) && (number < 15))
  {
    return number;
  }
  else
  {
    return 0;
  }
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
