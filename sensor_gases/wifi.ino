void wifiConfig() {
  espData("AT+RST", 1000, DEBUG);
  espData("AT+CWMODE=1", 1000, DEBUG);
  espData("AT+CWJAP=\"" + wifi_ssid + "\",\"" + wifi_password + "\"", 1000, DEBUG);

  delay(10000);
}

void wifiSendData(String metricsEndpoint, String payload) {
  digitalWrite(blueLed, HIGH);
  Serial.println(payload);
  
  String sendData = "POST /api" + metricsEndpoint + " HTTP/1.1\r\n";
  sendData += "Host: " + host + ":" + port + "\r\nContent-Type: application/json\r\n";
  sendData += "Content-Length: " + String(payload.length()) + "\r\n\r\n";
  sendData += payload;
  
  espData("AT+CIPMUX=1", 1000, DEBUG);
  espData("AT+CIPSTART=0,\"TCP\",\""+ host + "\"," + port, 1000, DEBUG);
  espData("AT+CIPSEND=0,"  + String(sendData.length()), 1000, DEBUG);

  Serial1.print(sendData);
  Serial.println(sendData); 
  espData("AT+CIPCLOSE=0", 1000, DEBUG);
  delay(2000);
  digitalWrite(blueLed, LOW);
}

String espData(String command, const int timeout, boolean debug) {
  Serial.print("AT Command ==> ");
  Serial.print(command);
  Serial.println("");
  
  String response = "";
  Serial1.println(command);
  long int time = millis();
  while ((time + timeout) > millis()) {
    while (Serial1.available()) {
      char c = Serial1.read();
      response += c;
    }
  }

  if (debug) {
    Serial.print(response);
  }
  
  return response;
}
