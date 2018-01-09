#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>

const String PROTOCOL_VERSION = "1";
const String DEVICE_TYPE = "1"; // LLave de agua
const String ACTION_WRITE = "POST";
const String ACTION_READ = "GET";
const String MESSAGE_DIVIDER = "|";
const byte CONTROLLER_ID[6] = "00000";
const byte DEVICE_ID[6] = "11111";

const int pinCE = 9;
const int pinCSN = 10;
RF24 radio(pinCE, pinCSN);

int counter = 1;
int limit = 10000;
boolean open = false;
 
void setup(void) {
    Serial.begin(9600);
    radio.begin();
    radio.openWritingPipe(CONTROLLER_ID);
    radio.openReadingPipe(2, DEVICE_ID);

    Serial.println(F("TLALOC INICIADO"));
}

void loop() {
    //getAPIState();
    readResponse();
    //delay(5000);
}

void getAPIState() {
    // Parse the device id into a String
    String device = String((char*) DEVICE_ID);

    // Message composition
    // "|" + PROTOCOL VERSION + "|" + DEVICE TYPE + "|" + DEVICE ID + "|" + ACTION + "|"
    String mensaje = MESSAGE_DIVIDER +
        PROTOCOL_VERSION + MESSAGE_DIVIDER +
        DEVICE_TYPE + MESSAGE_DIVIDER +
        device + MESSAGE_DIVIDER +
        ACTION_READ + MESSAGE_DIVIDER;

    char mensajeArray[(mensaje.length() + 1)];
    mensaje.toCharArray(mensajeArray, (mensaje.length() + 1));

    Serial.print(F("ENVIANDO: "));
    Serial.print(mensajeArray);
    Serial.print(F("  LONGITUD: "));
    Serial.println(strlen(mensajeArray));

    if (radio.write(mensajeArray, strlen(mensajeArray))) {
        Serial.println(F("MENSAJE ENVIADO CORRECTAMENTE"));
        // Read reasponse from controller
        readResponse();
    } else {
        Serial.println(F("* * ERROR ENVIANDO EL MENSAJE, SIN CONFIRMACION DE RECEPCION"));
        readResponse();
    }
}

void readResponse() {
    radio.openReadingPipe(1, DEVICE_ID);
    Serial.println(F("ESPERANDO RESPUESTA"));
    radio.startListening();
    uint32_t timer = millis() + 10000;
    int counter = 0;
    while(timer > millis()) {
        if (radio.available()) {
            int size = radio.getPayloadSize();

            char data[size];
            radio.read(data, size); 
            radio.writeAckPayload (DEVICE_ID, "OK", 2);

            Serial.print(F("MENSAJE RECIBIDO: "));
            Serial.print(data);
            Serial.print(F("  TAMANO: "));
            Serial.println(size);

            radio.stopListening ();
            return;
        } /*else {
            if (radio.available()) {
                Serial.println("There are bites to read");
            } else {
                Serial.println("No bites to read");
            }*//*
            if (counter > 110) {
                Serial.println(F("."));
                counter = 0;
            } else {
                Serial.print(F("."));
                counter ++;
            }
        }*/
    }
    Serial.println("");
    Serial.println(F("* * ERROR: SIN RESPUESTA"));
    radio.stopListening ();
    radio.openWritingPipe(CONTROLLER_ID);
}

/*
void readSerial() {
    if (Serial.available() > 0) {
        String message = Serial.readString();
  
        if (findText(F("OPEN"), message)) {
            Serial.println(F("STORING OPEN STATE"));
            open = true;
        }
    }
}
*/
/*
void writeToDevice() {
    radio.stopListening ();

    String param = "OPEN";
    char paramArray[(param.length() + 1)];
    param.toCharArray(paramArray, (param.length() + 1));

    Serial.print(F("SENDING: "));
    Serial.print(paramArray);
    Serial.print(F("  LENGTH: "));
    Serial.println(param.length());

    radio.write(paramArray, param.length());

    radio.startListening();
}


// ************************************************* EMISOR
void readDevices() {
    if (radio.available()) {
        int size = radio.getPayloadSize();

        char data[size];
        radio.read(data, size); 
        radio.writeAckPayload (TLALOC, "OK", 2);

        Serial.print(F("RECEIVING: "));
        Serial.print(data);
        Serial.print(F("  SIZE: "));
        Serial.println(size);

        if (open) {
            Serial.println(F("WRITTING OPEN STATE TO DEVICE"));
            open = false;
            //writeToDevice();
        }
    }
}
*/

int findText(String subStr, String str) {
  int foundpos = -1;

  if (str.length() < 1 || subStr.length() < 1 || str.length() < subStr.length()) {
    return false;
  }

  int index = str.indexOf(subStr.charAt(0));
  
  if (index < 0) {
    return false;
  }

  for (int i = index; i <= str.length() - subStr.length(); i++) {
    if (str.substring(i,subStr.length()+i) == subStr) {
      foundpos = i;
    }
  }

  return foundpos >= 0;
}
