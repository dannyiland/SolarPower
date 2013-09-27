  #include <Wire.h>
  #include <Adafruit_INA219.h>
  #include <SPIFlash.h>
  #include <SPI.h>
  #include <avr/wdt.h>
  
  Adafruit_INA219 ina219; // Power measurement board
  const int buttonPin = 14; // AKA A0, pin with on/off button for load
  const int TIP120pin = 5; //TIP120 Base pin, set high to control 12v output. Voltage Drop 0.7V
  int buttonState = 0;
  boolean on;
  char input = 0;
  double offset = 0;
  SPIFlash flash(8, 0xEF30);
  union float2bytes { float f; byte b[4]; };
   
  float2bytes current_f2b;
  float2bytes voltage_f2b;
  
  void setup(void) 
  {
    pinMode(buttonPin, INPUT);    

    pinMode(TIP120pin, OUTPUT); // Set pin for output to control TIP120 Base pin
    on = true; // Default to ON! Button press turns load off
    analogWrite(TIP120pin, 255); // By changing values from 0 to 255 you can output power
    Serial.begin(115200);
    Serial.println("Measuring and logging voltage and current ...");
    ina219.begin();
    if (flash.initialize())
      Serial.println("Flash memory OK!");
    else
      Serial.println("Flash memory init FAIL!");
     Serial.println("Erasing Flash chip"); 
     flash.chipErase(); // must do this, or overwriting flash will not work
     while(flash.busy());
     Serial.println("Logging current and voltage every 10 seconds");
     offset = 0; // Flash memory position
  }
  
  void loop(void) 
  {
    if (Serial.available() > 0) {
      input = Serial.read();
      if (input == 'r') //r= raw flash area
      {
      
        Serial.println("Flash content:");
        int c = 0;
  
        while(c<=offset){
          Serial.print(flash.readByte(c++), HEX
          );
          Serial.print('.');
        }
        
        Serial.println();
      }
      else if (input == 'd') //d=dump flash area
      {
        Serial.println("Logged readings:");
        int counter = 0;
        while(counter<=offset-8){
          for ( int i=0; i < sizeof(float); i++ ) {
            voltage_f2b.b[i] = flash.readByte(counter+i);
           }
           counter = counter + 4;
           for ( int i=0; i < sizeof(float); i++ ) {
            current_f2b.b[i] = flash.readByte(counter+i);
           }
           counter = counter + 4;
           // TODO: Use a real time clock to give actual times
           Serial.print("Time: ");
           Serial.print(counter/8 * 10);
           Serial.println("seconds after start");
           // Output the voltage and current over Serial
           Serial.print("Load Voltage:  ");
	   Serial.print(voltage_f2b.f, 2);
	   Serial.println(" V");
           Serial.print("Current:       "); 
	   Serial.print(current_f2b.f, 2);
	   Serial.println(" mA");
        }
      Serial.println();
      }
      else if (input == 'e')
      {
        Serial.print("Erasing Flash chip ... ");
        flash.chipErase();
        while(flash.busy());
        Serial.println("DONE");
        offset = 0;
      }
    }
      
    buttonState = digitalRead(buttonPin);
    
    if (buttonState == HIGH && on == true) {    
      Serial.print("Turning Output power off!");
      analogWrite(TIP120pin, 0); // By changing values from 0 to 255 you can control motor speed
      on = false;
      delay(2000); // stop a long hold from turning it on and off again
    } 
  
    if (buttonState == HIGH && on == false) {    
      Serial.print("Turning Output power on!");
      analogWrite(TIP120pin, 255); // By changing values from 0 to 255 you can control motor speed
      on = true;
    }

    float shuntvoltage = 0;
    float busvoltage = 0;
    float current_mA = 0;
    float loadvoltage = 0;
  
    shuntvoltage = ina219.getShuntVoltage_mV();
    busvoltage = ina219.getBusVoltage_V();
    current_mA = ina219.getCurrent_mA();
    loadvoltage = busvoltage + (shuntvoltage / 1000);
    
    Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
    Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
    Serial.print("Load Voltage:  ");
    Serial.print(loadvoltage); Serial.println(" V");
    Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
    Serial.println("");
    
    voltage_f2b.f = loadvoltage;
    current_f2b.f = current_mA;
    for ( int i=0; i < sizeof(float); i++ ) {
       flash.writeByte(offset+i, voltage_f2b.b[i]);
    }
    offset = offset + 4;
    for ( int i=0; i < sizeof(float); i++ ) {
      flash.writeByte(offset+i, current_f2b.b[i]);
    }
    offset = offset + 4;
    delay(10000);
  }
