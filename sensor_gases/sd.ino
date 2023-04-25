void sdCardInit() {
  Serial.println("Inicializando o cartao SD");
  lcdConfigMessage("Iniciando SD");
  while (!SD.begin(csPin)) {
    lcd.setCursor(0, 1);
    lcd.print("Iniciando SD");
    Serial.println("Erro ao inicializar cartao SD");
    lcdConfigMessage("Erro Iniciar SD");
    errorLed(2000);
  }
}

File sdCardRead() {
  Serial.println("Lendo conteudo do cartao SD");
  lcdConfigMessage("Lendo Cartao SD");
  return SD.open("config.txt");
}

void readConfigs(File configFile) {
  if (configFile) {
    DeserializationError error = deserializeJson(doc, configFile);
  
    if (error) {
      while (true) {
        Serial.println("Erro ler conteudo do arquivo");
        lcdConfigMessage("Erro Leitura SD");
        errorLed(1000);
      }
    }

    host = doc["host"].as<String>();
    id = doc["id"].as<int>();
    interval = mountInterval(doc["interval"].as<long>());
    
    configFile.close();
  } else {
    Serial.println("Erro ao abrir o arquivo");
    lcdConfigMessage("Erro Abrir JSON");
    errorLed(3000);
  }
}

long mountInterval(long interval) {
  if (interval < 60000) {
      interval = 60000;
    }

    if (interval > 3600000) {
      interval = 3600000;
    }

    return interval;
}
