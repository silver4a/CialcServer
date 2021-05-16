
void bluetoothConfig() {
  SerialBT.begin(bluetooth_name); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");
}

//$Ssid,password,hostname&
String dataBT = "";
String dataSplit[4];

void loopBT() {
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  while(SerialBT.available()) {
    dataBT += (char)SerialBT.read();
  }
  
  if(dataBT.length() > 0){
    if(dataBT.equals("init")){
      Serial.println("Conexion");
    }
    else{
        Serial.println("info -> "+dataBT);
        if(dataBT.indexOf("$")>=0 && dataBT.indexOf("&")>=0 && dataBT.indexOf("CONNECT")>=0){
            split(dataBT,',');
            Serial.println("SSID: "+dataSplit[0]);
            Serial.println("PASS: "+dataSplit[1]);
            Serial.println("HOST: "+dataSplit[2]);
            Serial.println("NUMBER: "+dataSplit[3]);
            preferences.putString("pref_ssid", dataSplit[0]);
            preferences.putString("pref_pass", dataSplit[1]);
            preferences.putString("pref_host", dataSplit[2]);
            preferences.putString("pref_ipNumber", dataSplit[3]);
            SerialBT.flush();
            ESP.restart();
            dataBT = "";
        }else{
          if(dataBT.indexOf("SCAN") >= 0){
            int networks = scan_wifi_networks();
            String txNetworks="";
            for(int k=0;k<=networks;k++){
             txNetworks += ssids_array[k] + "," ;
            }
            
            SerialBT.println("RESULT_SCAN," + txNetworks.substring(0,txNetworks.length()-1));
            dataBT = "";
          }else{
            dataBT = "";
          }
        }
    }
  }
  delay(20);
}

void bluetoothClose(){
  SerialBT.flush();  
  SerialBT.disconnect();
  SerialBT.end();
  Serial.println("BT stopped");
}

void split(String cadena,char index){
  int lenght = cadena.length();
  char charCadena[100];
  cadena.toCharArray(charCadena,100);
  String valor = "";
  int lenghtGlobal = 0;
  for(int k=0;k<=lenght;k++){
    if(charCadena[k] == index){
        if(valor.indexOf("$")>=0){
            valor = valor.substring(valor.indexOf("$")+1,valor.length());
        }
        dataSplit[lenghtGlobal] = valor;
        lenghtGlobal++;
        valor = "";
    }
    else
      valor += charCadena[k];
  }

  if(valor.length() > 0){
    if(valor.indexOf("&")>=0){
        valor = valor.substring(0,valor.indexOf("&"));
        dataSplit[lenghtGlobal] = valor;
    }else{
      dataSplit[lenghtGlobal] = valor;
    }
  }
}
