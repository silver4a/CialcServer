bool wiFiConfig()
{
  String temp_pref_ssid = preferences.getString("pref_ssid");
  String temp_pref_pass = preferences.getString("pref_pass");
  String ipp = preferences.getString("pref_ip");
  String subnett = preferences.getString("pref_subnet");
  String maskk = preferences.getString("pref_gateway");
  String ipNumber = preferences.getString("pref_ipNumber");
  host = preferences.getString("pref_host");
  
  
  ssid = temp_pref_ssid.c_str();
  password = temp_pref_pass.c_str();

  Serial.println(ssid);
  Serial.println(password);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);

  if(!ipp.equals("")){
    split(ipp,'.');
    Serial.print("IP: ");
    for(int k=0;k<=3;k++){
        ipStatic[k] = dataSplit[k].toInt();  
        Serial.print(String(ipStatic[k]) + ".");
    }

    Serial.print("\nsubnet: ");
    split(subnett,'.');
    for(int k=0;k<=3;k++){
        subnetStatic[k] = dataSplit[k].toInt();  
        Serial.print(String(subnetStatic[k]) + ".");
    }
    
    Serial.print("\nGateway: ");
    split(maskk,'.');
    for(int k=0;k<=3;k++){
        gatewayStatic[k] = dataSplit[k].toInt();  
        Serial.print(String(gatewayStatic[k]) + ".");
    }
    
    IPAddress ip(ipStatic[0],ipStatic[1],ipStatic[2],ipStatic[3]);
    IPAddress gateway(subnetStatic[0],subnetStatic[1],subnetStatic[2],subnetStatic[3]);
    IPAddress subnet(gatewayStatic[0],gatewayStatic[1],gatewayStatic[2],gatewayStatic[3]);
    WiFi.config(ip,gateway,subnet);
  }
    
  if(temp_pref_ssid.equals("")){
    return false;
  }
  //WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);

  delay(1000);
  start_wifi_millis = millis();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - start_wifi_millis > wifi_timeout) {
      WiFi.disconnect();
      return false;
    }
  }

    bool restart = false;
    if(ipp.equals("") && ipNumber.length() > 0)
      restart = true;
      
    Serial.println("Se ha conectado! obteniendo direcciones. . .");
    ipp = WiFi.localIP().toString();
    subnett = WiFi.subnetMask().toString();
    maskk = WiFi.gatewayIP().toString();
    Serial.println(ipp);
    Serial.println(subnett);
    Serial.println(maskk);
    preferences.putString("pref_ip",ipp);
    preferences.putString("pref_subnet", subnett);
    preferences.putString("pref_gateway", maskk);

    if(restart){
      ipp = armarIp(ipp,ipNumber);
      preferences.putString("pref_ip",ipp);
      Serial.println("Configurando proximo reinicio. . .");
      Serial.println("IP para el proximo reinicio. . . "+ipp);
      delay(10000);
      ESP.restart();  
    }
    return true;
}


String armarIp(String ip,String number){
    String ipReturn = "";
    split(ip,'.');
    Serial.print("IP: ");
    for(int k=0;k<=3;k++){
      if(k!=3){
        ipStatic[k] = dataSplit[k].toInt();  
        ipReturn += String(ipStatic[k]) + ".";
      }
      else{
        ipStatic[k] = number.toInt();
        ipReturn += String(ipStatic[k]);
      }
    }
    return ipReturn;
}

void loopWiFi(){
WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;   
        }
        
        if (currentLine.endsWith("GET /data?state=1")) {
          Serial.println("ENCENDIDO");
          digitalWrite(2,1);
        }
        if (currentLine.endsWith("GET /data?state=0")) {
          Serial.println("APAGADO");
          digitalWrite(2,0);
        }
        if (currentLine.endsWith("GET /data?clear=1")) {
          Serial.println("Limpiando preferencias y reseteando. . .");
          delay(1000);
          clearPreferences();
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

int scan_wifi_networks()
{
  WiFi.mode(WIFI_STA);
  // WiFi.scanNetworks will return the number of networks found
  int n =  WiFi.scanNetworks();
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.println();
    Serial.print(n);
    Serial.println(" networks found");
    delay(1000);
    for (int i = 0; i < n; ++i) {
      ssids_array[i + 1] = String(WiFi.SSID(i)) + String((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(ssids_array[i + 1]);
      network_string = i + 1;
      network_string = network_string + ": " + WiFi.SSID(i) + " (Strength:" + WiFi.RSSI(i) + ")";
      Serial.println(network_string);
    }
    wifi_stage = SCAN_COMPLETE;
  }
  return n;
}
