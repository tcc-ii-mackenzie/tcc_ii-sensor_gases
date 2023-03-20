#include <MQUnifiedsensor.h>
#include <DHT.h>

#define mq3pin A0
#define mq4pin A1
#define mq135pin A2
#define mq7pin A3
#define mq8pin A6
#define mq9pin A7
#define dhtPin 11
#define yellowLed 12
#define greenLed 13

#define board "Arduino Nano"
#define ratioMQ3CleanAir 60 //RS / R0 = 60 ppm 
#define ratioMQ4CleanAir 4.4 //RS / R0 = 4.4 ppm 
#define ratioMQ135CleanAir 3.6 //RS / R0 = 10 ppm 
#define ratioMQ7CleanAir 27.5 //RS / R0 = 27.5 ppm
#define ratioMQ8CleanAir 70 //RS / R0 = 70 ppm   
#define ratioMQ9CleanAir 9.6 //RS / R0 = 9.6 ppm 
#define adc_bit_resolution 10 // 10 bit ADC 
#define voltage_resolution 5 // Volt resolution to calc the voltage
#define dhtType DHT22 

//Declare Sensor
MQUnifiedsensor mq3(board, voltage_resolution, adc_bit_resolution, mq3pin, board);
MQUnifiedsensor mq4(board, voltage_resolution, adc_bit_resolution, mq4pin, board);
MQUnifiedsensor mq135(board, voltage_resolution,adc_bit_resolution, mq135pin, board);
MQUnifiedsensor mq7(board, voltage_resolution, adc_bit_resolution, mq7pin, board);
MQUnifiedsensor mq8(board, voltage_resolution, adc_bit_resolution, mq8pin, board);
MQUnifiedsensor mq9(board, voltage_resolution, adc_bit_resolution, mq9pin, board);

bool calibrate = true;
DHT dht(dhtPin, dhtType);

void setup() {
  Serial.begin(9600);
  
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);

  dht.begin();
  mq3.init();
  mq3.setRegressionMethod(1);
  //_PPM =  a*ratio^b
  mq3.setR0(0.45);
  
  mq4.init();
  mq4.setRegressionMethod(1);
  //_PPM =  a*ratio^b
  mq4.setR0(14.23);
  
  mq135.init();
  mq135.setRegressionMethod(1);
  //_PPM =  a*ratio^b
  mq135.setR0(9.03);
  
  mq7.init();
  mq7.setRegressionMethod(1);
  //_PPM =  a*ratio^b
  mq7.setR0(5.90);
  
  mq8.init();
  mq8.setRegressionMethod(1);
  //_PPM =  a*ratio^b
  mq8.setR0(0.91);
  
  mq9.init();
  mq9.setRegressionMethod(1);
  //_PPM =  a*ratio^b
  mq9.setR0(13.93);


  if (calibrate) {
    //While calibrating Your sensor Uncomment this calibration portion and calibrate for R0.
    Serial.print("Calibrando, por favor aguarde...");
    digitalWrite(yellowLed, HIGH);
    
    float mq3calcR0 = 0, mq4calcR0 = 0, mq135calcR0 = 0, mq7calcR0 = 0, mq8calcR0 = 0, mq9calcR0 = 0;
    
    for (int i = 1; i <= 10; i ++) {
      //Update the voltage lectures
      mq3.update();
      mq4.update();
      mq135.update();
      mq7.update();
      mq8.update();
      mq9.update();
  
      mq3calcR0 += mq3.calibrate(ratioMQ3CleanAir);
      mq4calcR0 += mq4.calibrate(ratioMQ4CleanAir);
      mq135calcR0 += mq135.calibrate(ratioMQ135CleanAir);
      mq7calcR0 += mq7.calibrate(ratioMQ7CleanAir);
      mq8calcR0 += mq8.calibrate(ratioMQ8CleanAir);
      mq9calcR0 += mq9.calibrate(ratioMQ9CleanAir);
  
      Serial.print(".");
    }
    
    mq3.setR0(mq3calcR0 / 10);
    mq4.setR0(mq4calcR0 / 10);
    mq135.setR0(mq135calcR0 / 10);
    mq7.setR0(mq7calcR0 / 10);
    mq8.setR0(mq8calcR0 / 10);
    mq9.setR0(mq9calcR0 / 10);
    Serial.println("");
    Serial.println("Calibrado!");
  
    Serial.print("(MQ3 - MQ9):");
    Serial.print(mq3calcR0 / 10); Serial.print(" | ");
    Serial.print(mq4calcR0 / 10); Serial.print(" | ");
    Serial.print(mq135calcR0 / 10); Serial.print(" | ");
    Serial.print(mq7calcR0 / 10); Serial.print(" | ");
    Serial.print(mq8calcR0 / 10); Serial.print(" | ");
    Serial.print(mq9calcR0 / 10); Serial.println(" |");
    digitalWrite(yellowLed, LOW);
  }
}

void loop() {
  //Update the voltage lectures
  mq3.update();
  mq4.update();
  mq135.update();  
  mq7.update();
  mq8.update();
  mq9.update();

  mq3.setA(0.3934); 
  mq3.setB(-1.504); // Alcohol
  float alcohol = mq3.readSensor(); 

  mq3.setA(4.8387); 
  mq3.setB(-2.68); // Benzene
  float benzene = mq3.readSensor(); 
  
  mq3.setA(7585.3); 
  mq3.setB(-2.849); // Hexane
  float hexane = mq3.readSensor(); 

  mq4.setA(1012.7); 
  mq4.setB(-2.786); // CH4
  float ch4 = mq4.readSensor(); 

  mq4.setA(30000000);
  mq4.setB(-8.308); // Smoke
  float smoke = mq4.readSensor(); 
 
  mq135.setA(110.47);
  mq135.setB(-2.862); //CO2 
  float co2 = mq135.readSensor(); 
  
  mq135.setA(44.947);
  mq135.setB(-3.445); // Toluene
  float toluene = mq135.readSensor(); 
  
  mq135.setA(102.2 );
  mq135.setB(-2.473); // NH4 
  float nh4 = mq135.readSensor(); 
  
  mq135.setA(34.668);
  mq135.setB(-3.369); // Acetone
  float acetone = mq135.readSensor(); 
 
  mq7.setA(99.042);
  mq7.setB(-1.518); // CO
  float co = mq7.readSensor(); 

  mq8.setA(976+.97);
  mq8.setB(-0.688); // H2
  float h2 = mq8.readSensor();

  mq9.setA(1000.5);
  mq9.setB(-2.186); // Flamable Gas
  float fg = mq9.readSensor();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  Serial.print("Alcohol:"); 
  Serial.println(alcohol);
  
  Serial.print("Benzene:");
  Serial.println(benzene);
  
  Serial.print("Hexane:");
  Serial.println(hexane);

  Serial.print("Methane:");
  Serial.println(ch4);
  
  Serial.print("Smoke:");
  Serial.println(smoke);
  
  Serial.print("CO2:");
  Serial.println(co2);

  Serial.print("Toluene:");
  Serial.println(toluene);
  
  Serial.print("NH4:");
  Serial.println(nh4);
  
  Serial.print("Acetone:");
  Serial.println(acetone);
  
  Serial.print("CO:");
  Serial.println(co);
  
  Serial.print("H2:");
  Serial.println(h2);
  
  Serial.print("FG:");
  Serial.println(fg);

  Serial.print("Temperature:");
  Serial.println(temperature);

  Serial.print("Humidity:");
  Serial.println(humidity);

  Serial.println("--------------------------------------------------------");

   digitalWrite(greenLed, HIGH);
   delay(1000);
   digitalWrite(greenLed, LOW);
  
  delay(3000);
}
