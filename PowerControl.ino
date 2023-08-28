#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <LayadCircuits_SalengGSM.h>
#include <Button.h>

#define buttonPin 3
#define ledPin 2
#define relayPin 4

SoftwareSerial gsmSerial(8, 7); //SIM800L RxD(4) & TxD(5) is connected to Arduino #5 & #6
LayadCircuits_SalengGSM salengGSM = LayadCircuits_SalengGSM(&gsmSerial);
LiquidCrystal_I2C lcd(0x27, 16, 2); // Start the lcd screen
Button button(buttonPin); //Declare the button

//--Button States and Declare--//
int electricityState = 0; // 0 for state OFF and 1 for state ON
bool cancelpwr = false; // Variable for the cancel case in powering off
int powerOffTime = 299; // Variable for time to power off. 179 - > 3min | 299 - > 5min
const unsigned long lcdTurnOff = 420000; // Variable for idle time to no backlight. Now is 7 minutes
unsigned long lastmillis = millis()+1;
bool lcdbacklight = true;
String SMSNum, SMSNumM = "+306977264630";
char* SMSBody;

void setup() {
  button.begin();
  pinMode(ledPin, OUTPUT); // Initialize the LED as an output
  pinMode(relayPin, INPUT_PULLUP); // Initialize the Relay as Input_Pullup to prevent the swich on and off at esp power on
  pinMode(relayPin, OUTPUT); // Initialize the Relay as an output

  salengGSM.begin(19200);
  delay(10000); // allow 10 seconds for modem to boot up and register
  salengGSM.initSalengGSM();
    
  lcd.begin(16, 2); // start lcd screen
  lcd.init();
  lcd.backlight();

  lcdCenterText("Home Electricity", 0); //Default values
  lcdCenterText("IS OFF", 1);
  digitalWrite(ledPin, HIGH);
  digitalWrite(relayPin, HIGH);

// Send SMS after power loss 
  salengGSM.sendSMS(SMSNumM.c_str(), "Perachora's Electricity is OFF after Power Loss");
}

void loop() {
  salengGSM.smsMachine(); // Τhis allows for non-blocking SMS transmission
  if(salengGSM.isSMSavailable()) { // Check for incoming messages
     salengGSM.readSMS(); // Read SMS
     SMSBody = salengGSM.smsRxMsg; // SMS Body to variable
     to_upper(SMSBody); // Uppercase the Received Message
     SMSNum = salengGSM.smsSender; // Save the sender's number to variable  
     handle_sim800_response();
  }

  salengGSM.smsMachine();
  if(button.pressed()) {
     lcd.backlight(); // turn on backlight.
     lastmillis = millis();
     lcdbacklight = true;
     if(electricityState == 0) { // turning ON
        PowerON();
        salengGSM.smsMachine();
        salengGSM.sendSMS(SMSNumM.c_str(), "Perachora's Electricity turned ON from button");
      } else { // turning OFF         // electricityState == 1)
        lcdCenterText("Press to CANCEL", 1);
        digitalWrite(ledPin, HIGH);
        for(int i = powerOffTime; i >= 0; i--) {
            if(!button.pressed()) {
               String lcdMinutes = String(i / 60);
               String lcdSeconds = String(i % 60);
               if((i % 60) < 10) lcdSeconds = "0" + lcdSeconds; 
               lcdCenterText("Turn OFF in " + lcdMinutes + ":" + lcdSeconds, 0);
               ledBlink(i / 60);
               delay(1000);
             } else {
               cancelpwr = true;
               break;
            }
        }
        if(!cancelpwr) { //Cancel Case
           PowerOFF();
           salengGSM.smsMachine();
           salengGSM.sendSMS(SMSNumM.c_str(),"Perachora's Electricity turned OFF from button");
         } else {
           lcdCenterText("Cancelling State", 0);
           lcdCenterText("Power OFF", 1);
           delay(2000);
           cancelpwr = false;
           lcdCenterText("Home Electricity", 0);
           lcdCenterText("IS ON", 1);
        }
     }
  }
     
  salengGSM.smsMachine();
  if(millis() - lastmillis >= lcdTurnOff && lcdbacklight == true) {
     lcd.noBacklight();
     lcdbacklight = false;
  }
}

void handle_sim800_response() {
     lcd.backlight(); // turn on backlight.
     lastmillis = millis();
     lcdbacklight = true;
     if (strstr(SMSBody,"ON")){ // Case : SMS response has "ON" inside.
         if (electricityState == 0){
            PowerON();
            salengGSM.smsMachine();
            salengGSM.sendSMS(SMSNum.c_str(),"Now Perachora's Electricity is ON");
            if(SMSNum != SMSNumM) { // If the sender is different from the default
              while (!salengGSM.isFreeToSend()) {
                   salengGSM.smsMachine();
                   delay(1000);
              }
              salengGSM.smsMachine();
              char SMSinfo[100];
              strcpy(SMSinfo,"Perachora's Electricity turned ON by ");
              strcat(SMSinfo,SMSNum.c_str());
              salengGSM.sendSMS(SMSNumM.c_str(),SMSinfo);
            }
          } else {
            salengGSM.smsMachine();
            salengGSM.sendSMS(SMSNum.c_str(),"Perachora's Electricity is already ON");
         }    
     } else if (strstr(SMSBody,"OFF")){  // Case : SMS response has "OFF" inside.
         if (electricityState == 1){
            PowerOFF();
            salengGSM.smsMachine();
            salengGSM.sendSMS(SMSNum.c_str(),"Now Perachora's Electricity is OFF");
            if(SMSNum != SMSNumM) {
              while (!salengGSM.isFreeToSend()) {
                   salengGSM.smsMachine();
                   delay(1000);
              }
              salengGSM.smsMachine();
              char SMSinfo1[100];
              strcpy(SMSinfo1,"Perachora's Electricity turned OFF by ");
              strcat(SMSinfo1,SMSNum.c_str());
              salengGSM.sendSMS(SMSNumM.c_str(),SMSinfo1);
            }
          } else {
            salengGSM.sendSMS(SMSNum.c_str(),"Perachora's Electricity is already OFF");
         }
      } else if (strstr(SMSBody,"STATUS") || strstr(SMSBody,"STATE")){ // Case : SMS response has "STATUS" or "STATE" inside.
        if(electricityState == 1) {
          salengGSM.sendSMS(SMSNum.c_str(),"Perachora's Electricity is ON");
         } else {
          salengGSM.sendSMS(SMSNum.c_str(),"Perachora's Electricity is OFF");
         }   
        } else {
        salengGSM.sendSMS(SMSNumM.c_str(),SMSBody);
       }
      salengGSM.smsMachine();
      SMSBody = "";
}

void PowerON() { 
      digitalWrite(relayPin, LOW);
      digitalWrite(ledPin, LOW);
      lcdCenterText("Home Electricity", 0);
      lcdCenterText("IS ON", 1);
      electricityState = 1;
      delay(100);
}

void PowerOFF() {
        digitalWrite(relayPin, HIGH);
        digitalWrite(ledPin, HIGH);
        lcdCenterText("Home Electricity", 0);
        lcdCenterText("IS OFF", 1);
        electricityState = 0;
        delay(100);
}

void lcdCenterText(String text_msg, int line) { // Centers any given text to LCD
  lcd.setCursor(0, line);
  lcd.print("                ");
  lcd.setCursor((16 - text_msg.length()) / 2, line);
  lcd.print(text_msg);
}

void ledBlink(int timer) { // Led blinks faster as the time comes to end
  switch (timer) {
    case 0 :
      digitalWrite(ledPin, (millis() / 1000) % 2);
      break;
    default :
      digitalWrite(ledPin, (millis() / 1000) % 5);
      break;
  }
}

void to_upper(char* string) { // Convert lowercase string to uppercase
    const char OFFSET = 'a' - 'A';
    while (*string) {
        *string = (*string >= 'a' && *string <= 'z') ? *string -= OFFSET : *string;
        string++;
    }
}
