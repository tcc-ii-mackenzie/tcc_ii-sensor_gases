void gsmConfigGprs() {
  Serial.println("Configurando GPRS");
  gsmSendSerial("AT+SAPBR=3,1,Contype,GPRS");
  gsmSendSerial("AT+CIPCSGP=1, \"" + apn + "\", \"" + apn_u + "\", \"" + apn_p + "\"");
}

String gsmSendSerial(String command) {
  String response = "";
  
  Serial.println("Send ->: " + command);
  Serial1.println(command);
  
  long wtimer = millis();
  
  while (wtimer + 3000 > millis()) {
    while (Serial1.available()) {
      char c = Serial1.read();
      Serial.write(c);
      response += c;
      delay(3);
    }
  }
  
  Serial.println();
  return response;
}

String gsmHttpPost(String endpoint, String postData) {
  String url = mountUrl(endpoint);
  String response = "";
  
  Serial.println("Inicando GPRS e HTTP");
  gsmSendSerial("AT+CGATT?");
  gsmSendSerial("AT+SAPBR=1,1");
  gsmSendSerial("AT+SAPBR=2,1");
  gsmSendSerial("AT+HTTPINIT");
  gsmSendSerial("AT+HTTPPARA=CID,1");
  gsmSendSerial("AT+HTTPPARA=URL," + url);
  gsmSendSerial("AT+HTTPPARA=CONTENT,application/json");
  gsmSendSerial("AT+HTTPDATA=" + String(postData.length()) + ",5000");
  gsmSendSerial(postData);
  gsmSendSerial("AT+HTTPACTION=1");
  gsmSendSerial("WAIT=30");
  response = gsmSendSerial("AT+HTTPREAD");
  gsmSendSerial("AT+HTTPTERM");
  gsmSendSerial("AT+SAPBR=0,1");

  return response;
}

String gsmGetTime() {
  Serial.println("Obtendo data e hora via GPRS");
  return gsmSendSerial("AT+CCLK?");
}

String mountUrl(String endpoint) {
  return host + "/api" + endpoint;
}
