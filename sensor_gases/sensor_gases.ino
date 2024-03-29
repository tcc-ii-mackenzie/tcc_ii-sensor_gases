#include <ArduinoJson.h>
#include <MQUnifiedsensor.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <SPI.h>
#include <SD.h>

// Display LCD
#define lcd_i2c_address 0x27
#define lcd_disp_cols 16
#define lcd_disp_rows 2

// Sensores MQ
#define mq3pin A0
#define mq4pin A1
#define mq135pin A2
#define mq7pin A3
#define mq8pin A6
#define mq9pin A7

// Leitor de cartao SD
#define csPin 53

// DHT
#define dhtPin 22

// LEDs
#define redLed 23
#define yellowLed 24
#define greenLed 25
#define blueLed 26

// Configuracoes MQ
#define board "Arduino Mega 2560"
#define ratioMQ3CleanAir 60 // RS / R0 = 60 ppm 
#define ratioMQ4CleanAir 4.4 // RS / R0 = 4.4 ppm 
#define ratioMQ135CleanAir 3.6 // RS / R0 = 10 ppm 
#define ratioMQ7CleanAir 27.5 // RS / R0 = 27.5 ppm
#define ratioMQ8CleanAir 70 // RS / R0 = 70 ppm   
#define ratioMQ9CleanAir 9.6 // RS / R0 = 9.6 ppm 
#define adc_bit_resolution 10 // 10 bit ADC 
#define voltage_resolution 5 // Volt resolution to calc the voltage
#define dhtType DHT22 

// Declarando Sensores MQ
MQUnifiedsensor mq3(board, voltage_resolution, adc_bit_resolution, mq3pin, board);
MQUnifiedsensor mq4(board, voltage_resolution, adc_bit_resolution, mq4pin, board);
MQUnifiedsensor mq135(board, voltage_resolution,adc_bit_resolution, mq135pin, board);
MQUnifiedsensor mq7(board, voltage_resolution, adc_bit_resolution, mq7pin, board);
MQUnifiedsensor mq8(board, voltage_resolution, adc_bit_resolution, mq8pin, board);
MQUnifiedsensor mq9(board, voltage_resolution, adc_bit_resolution, mq9pin, board);

DHT dht(dhtPin, dhtType);
StaticJsonDocument<192> doc;
DynamicJsonDocument docSend(1024);
LiquidCrystal_I2C lcd(lcd_i2c_address, lcd_disp_cols, lcd_disp_rows);

// Medicao dos gases
float alcohol, benzene, hexane, ch4, smoke, co2, toluene, nh4, acetone, co, h2, fg, temperature, humidity = 0;

// APN
String apn = "marketplaceiot.arqia.br"; // APN
String apn_u = "arqia"; // Usuário da APN
String apn_p = "arqia"; // Senha da APN

// API
String host = "";
String metricsEndpoint = "/dispositivos";
String payload = "";
int id = 0;

// Tempo
unsigned long timerStart, timerEnd, interval = 0;

void setup() {
  Serial.println("Inicializando o J3M...");
  Serial.begin(9600);
  Serial1.begin(9600);

  lcd.init();
  lcd.clear();
  lcd.setBacklight(HIGH);
  lcdInitializeMessage();

  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(blueLed, OUTPUT);

  delay(10000);
  
  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }
  
  sdCardInit();
  readConfigs(sdCardRead());
  gsmConfigGprs();
  Serial.println("Inicializando sensores");
  lcdConfigMessage("Calibrando...");
  dht.begin();
  beginMqs();
  calibrateMqs();
  
  Serial.println("Inicializacao do J3M concuida!");
  lcdConfigMessage("Inicializado!!!");
}

void loop() {
  timerStart = millis();
  
  Serial.println("Iniciando serie de medicoes");
  lcdLoopMessage("Iniciando Coletas");
  delay(2000);
  
  for (int i = 1; i <= 5; i++) {
    Serial.println("Iniciando medicoes");
    lcdLoopMessage("Coletando Dados");
  
    delay(2000);

    Serial.println("Métrica " + (String) i + "/5");
    lcdLoopMessage("Coleta " + (String) i + "/5");
  
    delay(2000);
    
    updateMqs();
    payload = "";
  
    mq3.setA(0.3934); 
    mq3.setB(-1.504); // Alcohol
    alcohol = mq3.readSensor(); 
  
    mq3.setA(4.8387); 
    mq3.setB(-2.68); // Benzene
    benzene = mq3.readSensor(); 
    
    mq3.setA(7585.3); 
    mq3.setB(-2.849); // Hexane
    hexane = mq3.readSensor(); 
  
    mq4.setA(1012.7); 
    mq4.setB(-2.786); // CH4
    ch4 = mq4.readSensor(); 
  
    mq4.setA(30000000);
    mq4.setB(-8.308); // Smoke
    smoke = mq4.readSensor(); 
   
    mq135.setA(110.47);
    mq135.setB(-2.862); //CO2 
    co2 = mq135.readSensor(); 
    
    mq135.setA(44.947);
    mq135.setB(-3.445); // Toluene
    toluene = mq135.readSensor(); 
    
    mq135.setA(102.2 );
    mq135.setB(-2.473); // NH4 
    nh4 = mq135.readSensor(); 
    
    mq135.setA(34.668);
    mq135.setB(-3.369); // Acetone
    acetone = mq135.readSensor(); 
   
    mq7.setA(99.042);
    mq7.setB(-1.518); // CO
    co = mq7.readSensor(); 
  
    mq8.setA(976.97);
    mq8.setB(-0.688); // H2
    h2 = mq8.readSensor();
  
    mq9.setA(1000.5);
    mq9.setB(-2.186); // Flamable Gas
    fg = mq9.readSensor();
  
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();
  
    Serial.println("Alcool:");
    Serial.println(alcohol);
    
    Serial.println("Benzeno:");
    Serial.println(benzene);
  
    Serial.println("Hexano:");
    Serial.println(hexane);
  
    Serial.println("Metano:");
    Serial.println(ch4);
  
    Serial.println("Fumaça:");
    Serial.println(smoke);
  
    Serial.println("Dioxido de Carbono:");
    Serial.println(co2);
  
    Serial.println("Tolueno:");
    Serial.println(toluene);
  
    Serial.println("Amonia:");
    Serial.println(nh4);
  
    Serial.println("Acetona:");
    Serial.println(acetone);
  
    Serial.println("Monoxido de Carbono:");
    Serial.println(co);
  
    Serial.println("Hidrogenio:");
    Serial.println(h2);
  
    Serial.println("Gases Inflamaveis:");
    Serial.println(fg);
  
    Serial.println("Temperatura:");
    Serial.println(temperature);
  
    Serial.println("Umidade:");
    Serial.println(humidity);
  
    docSend["id_arduino"] = id;
    docSend["alcool"] = alcohol;
    docSend["benzeno"] = benzene;
    docSend["hexano"] = hexane;
    docSend["metano"] = ch4;
    docSend["fumaca"] = smoke;
    docSend["dioxido_carbono"] = co2;
    docSend["tolueno"] = toluene;
    docSend["amonia"] = nh4;
    docSend["acetona"] = acetone;
    docSend["monoxido_carbono"] = co;
    docSend["hidrogenio"] = h2;
    docSend["gases_inflamaveis"] = fg;
    docSend["temperatura"] = temperature;
    docSend["umidade"] = humidity;
    
    serializeJson(docSend, payload);
  
    lcdLoopMessage("Enviando Dados..");
    gsmHttpPost(metricsEndpoint, payload);
    lcdLoopMessage("Dados Enviados!");
  
    delay(2000);
    digitalWrite(greenLed, HIGH);
    delay(1000);
    digitalWrite(greenLed, LOW);
    
    Serial.println("Medicoes concluidas");
    lcdLoopMessage("Coleta Efetuada!");
    Serial.println("-------------------------------------------");
  }

  timerEnd = millis();

  lcdLoopMessage("");
  
  delay(interval - (timerEnd - timerStart));
}
