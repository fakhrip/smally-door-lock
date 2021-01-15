#include <stdarg.h>
#include "LiquidCrystal.h"  //lcd libary  
                                     
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);   //LCD object Parameters: (rs, enable, d4, d5, d6, d7)
#define echoPin 13
#define trigPin 12
#define soundPin 11 
#define solenoid 10
#define SERIAL_PRINTF_MAX_BUFF      256
#define F_PRECISION                   6
void serialPrintf(const char *fmt, ...);

int distance;
int getDistance();
long duration;
unsigned long lastEvent = 0;

int key[] = {-1,-1,-1,-1};
int pass[] = {15,10,10,5};
int keyState[] = {-1, 1};

/**
 * --------------------------------------------------------------
 * Perform simple printing of formatted data
 * Supported conversion specifiers: 
 *      d, i     signed int
 *      u        unsigned int
 *      ld, li   signed long
 *      lu       unsigned long
 *      f        double
 *      c        char
 *      s        string
 *      %        '%'
 * Usage: %[conversion specifier]
 * Note: This function does not support these format specifiers: 
 *      [flag][min width][precision][length modifier]
 * --------------------------------------------------------------
 */
void serialPrintf(const char *fmt, ...) {
  /* buffer for storing the formatted data */
  char buf[SERIAL_PRINTF_MAX_BUFF];
  char *pbuf = buf;  /* pointer to the variable arguments list */
  va_list pargs;
  
  /* Initialise pargs to point to the first optional argument */
  va_start(pargs, fmt);  /* Iterate through the formatted string to replace all 
  conversion specifiers with the respective values */
  while(*fmt) {
    if(*fmt == '%') {
      switch(*(++fmt)) {
        case 'd': 
        case 'i': 
          pbuf += sprintf(pbuf, "%d", va_arg(pargs, int));
          break;        case 'u': 
          pbuf += sprintf(pbuf, "%u", va_arg(pargs, unsigned int));
          break;        case 'l': 
          switch(*(++fmt)) {
            case 'd': 
            case 'i': 
              pbuf += sprintf(pbuf, "%ld", va_arg(pargs, long));
              break;            case 'u': 
              pbuf += sprintf( pbuf, "%lu", 
                               va_arg(pargs, unsigned long));
              break;
          }
          break;        case 'f': 
          pbuf += strlen(dtostrf( va_arg(pargs, double), 
                                  1, F_PRECISION, pbuf));
          break;
        
        case 'c':
          *(pbuf++) = (char)va_arg(pargs, int);
          break;        case 's': 
          pbuf += sprintf(pbuf, "%s", va_arg(pargs, char *));
          break;        case '%':
          *(pbuf++) = '%';
          break;        default:
          break;
      }
    } else {
      *(pbuf++) = *fmt;
    }
    
    fmt++;
  }
  
  *pbuf = '\0';
  
  va_end(pargs);
  lcd.print(buf);
}

int getDistance() {
  
  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)

  return distance;
}

void updateKeyState() {
  for (byte i = 0; i < (sizeof(key) / sizeof(key[0])); i++) {
    if (key[i] == -1) {
      
      keyState[0] = i;
      keyState[1] = 0;
      return;
    }
  }
  
  keyState[0] = -1;
  keyState[1] = 1;
}

void checkPass(int distance){

  int keyLen = (sizeof(key) / sizeof(key[0]));

  updateKeyState();
  int pos = keyState[0];
  int isFull = keyState[1];

  //  Reinitialize (or kind of) the key arr
  if (isFull && pos == -1) 
    for (byte i = 0; i < keyLen; i++)
      key[i] = -1;

  //  Capture key sequence
  key[pos] = distance;
  lcd.setCursor(0,1);
  serialPrintf("Pass=");
  for (byte i = 0; i < keyLen; i++) {
    serialPrintf("%d", key[i]);

    if (i < keyLen - 1) serialPrintf(",");
  }
  lcd.setCursor(0,1);

  //  Start checking key and pass if key is ready to be checked (full)
  updateKeyState();
  if (keyState[1]){
    for (byte i = 0; i < keyLen; i++) {
      if (key[i] != pass[i]) {
        delay(500);
        lcd.clear();
        serialPrintf("wrong password");
        digitalWrite(solenoid, HIGH);
        return;
      }
    }
    delay(500);
    lcd.clear();
    serialPrintf("password correct");
    digitalWrite(solenoid, LOW);
    delay(5000);
    digitalWrite(solenoid, HIGH);
  }
}

void setup() {
  lcd.begin(16,2);   
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(soundPin, INPUT);
  pinMode(solenoid, OUTPUT);
  digitalWrite(solenoid, HIGH);
  Serial.begin(9600);
}

void loop() {
  getDistance();
  lcd.setCursor(0,0);
  lcd.print("Distance:");
  lcd.print(distance);
  lcd.print(" Cm   ");
  int sensorData = digitalRead(soundPin);

  if (sensorData == HIGH) {
    
    // If 25ms have passed since last LOW state, it means that
    // the clap is detected and not due to any spurious sounds
    if (millis() - lastEvent > 25) {
      lcd.clear();
      serialPrintf("Clap detected!");
      lcd.setCursor(0,1);
      checkPass(getDistance());
      delay(500);
    }
    
    // Remember when last event happened
    lastEvent = millis();
  }
}
