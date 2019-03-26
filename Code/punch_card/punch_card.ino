/*
   Proving out the "punch card method"
   4 light sensors are looking for ambient light.
   a cardboard "key" is placed over the light sensors, so as to block out certain sensors.
   Reading which one's are "dark" we can see which card is preset.
   Each card has 4 spots on it, and you can hole punch out the binary number.
   punched = set, unpunched = cleared.
*/



// the 4 light sensors are on A1-A4
byte lightPins[] = {A1, A2, A3, A4};

// store the light values in an array
int lightValues[] = {0, 0, 0, 0};

void setup() {
  // put your setup code here, to run once:
  for (int i = 0 ; i < 4 ; i++)
  {
    pinMode(lightPins[i], INPUT);

  }

  pinMode(A0, OUTPUT);    // GND on my setup is connected to A0
  digitalWrite(A0, LOW);
  pinMode(A5, OUTPUT);    // VCC on my light sensor proto is connected to A5
  digitalWrite(A5, HIGH);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
  readLightSensors();
  printLightValues();

}

void readLightSensors()
{
  for (int i = 0 ; i < 4 ; i++)
  {
    lightValues[i] = analogRead(lightPins[i]);
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
  Serial.println();
}
