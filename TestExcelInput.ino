//always starts in line 0 and writes the thing written next to LABEL
#include <DallasTemperature.h>
#include <TimeLib.h>
#include <HX711_ADC.h>
#include <OneWire.h>

#define ONE_WIRE_BUS 3 //dallas Temperature sensor pin number. 
#define PH_PIN A0 //pH probe pin number connected to Po 
HX711_ADC LoadCell(4, 5); //Weight scale Pin. HX711 constructor (dout pin, sck pin)
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass the oneWire reference to Dallas Temperature. 

unsigned long pause_time = 1; // pause time between reads of weight, pH and Temp.(in seconds)
unsigned long wait_time = 2000; // Wait time between rows of reads. tare preciscion can be improved by adding a few seconds of wait time
long t;   //current timer
time_t timer;

void setup() {
  Serial.begin(9600); // the bigger number the better
  LoadCell.begin();
  delay(wait_time);
  timer= now();
  Initialize_PlxDaq();

  //LoadCell.start(wait_time);
  LoadCell.setCalFactor(192.5); // user set calibration factor (float)
  Serial.println("Startup + tare is complete");
}

void loop() {
  LoadCell.update();
  timer= now();
  if (now() > t + wait_time) {
    Write_PlxDaq();
    t = now();
  }
  //delay(wait_time); //add a delay
   //receive from serial terminal
  
  if (Serial.available() > 0) {
      //float i;
      char inByte = Serial.read();
      if (inByte == 't') LoadCell.tareNoDelay();
      else if (inByte== 'i') wait_time = wait_time + 1000; //Increase Wait time by one second
      else if (inByte== 'd') wait_time = wait_time - 1000; //Decrease Wait time by one second
   }

    //check if last tare operation is complete
  if (LoadCell.getTareStatus() == true) {
      Serial.println("Tare complete");
   }  
}

void Initialize_PlxDaq()
{
  Serial.println("CLEARDATA"); //clears up any data left from previous projects
  Serial.println("LABEL,Time,Timer,Weight (g),pH, Temperature (C)"); //always write LABEL, to indicate it as first line
}
void Write_PlxDaq()
  {
    Serial.print("DATA"); //always write "DATA" to Inidicate the following as Data
    Serial.print(","); //Move to next column using a ","

    Serial.print("TIME"); //Store date on Excel
    Serial.print(","); //Move to next column using a ","

    Serial.print("TIMER"); //Store date on Excel
    Serial.print(","); //Move to next column using a ","

    //Obtain Reading from Weight Sensor.    
    float i = LoadCell.getData();
    Serial.print(i);
    Serial.print(","); //Move to next column using a ","

    //Obtain Reading from pH Meter.
    int measure = analogRead(PH_PIN);
    double voltage = 5 / 1024.0 * measure; //classic digital to voltage conversion  
    //Serial.print(measure);
    //Serial.print(",");
    //Serial.print(voltage, 2);
    //Serial.print(",");
    // PH_step = (voltage@PH7 - voltage@PH4) / (PH7 - PH4)
    // PH_probe = PH7 - ((voltage@PH7 - voltage@probe) / PH_step)
    float Po = 7 + ((2.5 - voltage) / 0.18);
    Serial.print(Po, 2);
    Serial.print(","); //Move to next column using a ","

    //Obtain Reading from Temperature Sensor.
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0)*1.0;
    Serial.print(temperature,2);
    Serial.println(); //End of Row move to next row

  }
