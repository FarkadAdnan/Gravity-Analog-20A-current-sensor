/*
 By:Farkad Adnan
 E-mail: farkad.hpfa95@gmail.com
 inst : farkadadnan
 #farkadadnan , #farkad_adnan , فرقد عدنان#
 FaceBook: كتاب عالم الاردوينو
 inst : arduinobook
 #كتاب_عالم_الاردوينو  #كتاب_عالم_الآردوينو 
 */
const int currentSensorPin = A1; 
const int mVperAmp = 100;
float Vref  = 0; 
void setup(){
    Serial.begin(115200);
    Vref = readVref(); 
}
void loop(){
    float CurrentValue =  readDCCurrent(currentSensorPin);
    Serial.println(CurrentValue);
    delay(500);
}
float readDCCurrent(int Pin){
    int analogValueArray[31];
    for(int index=0;index<31;index  ){
      analogValueArray[index]=analogRead(Pin);
    }
    int i,j,tempValue;
    for (j = 0; j < 31 - 1; j   ){
        for (i = 0; i < 31 - 1 - j; i   ){
            if (analogValueArray[i] > analogValueArray[i   1]){
                tempValue = analogValueArray[i];
                analogValueArray[i] = analogValueArray[i   1];
                analogValueArray[i   1] = tempValue;
            }
        }
    }
    float medianValue = analogValueArray[(31 - 1) / 2];
    float DCCurrentValue = (medianValue / 1024.0 * Vref - Vref / 2.0) / mVperAmp;  
    return DCCurrentValue;
}
float readACCurrent(int Pin){
   int analogValue;            
   int maxValue = 0;         
   int minValue = 1024;         
   unsigned long start_time = millis();
   while((millis()-start_time) < 200) {
       analogValue = analogRead(Pin);
       if (analogValue > maxValue){
           maxValue = analogValue;
       }
       if (analogValue < minValue){
           minValue = analogValue;
       }
   }
   float Vpp = (maxValue - minValue) * Vref / 1024.0;
   float Vrms = Vpp / 2.0 * 0.707 / mVperAmp; //Vpp -> Vrms
   return Vrms;
}

long readVref(){
    long result;
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB1286__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    ADCSRB &= ~_BV(MUX5);   
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
#endif
#if defined(__AVR__)
    delay(2);                                      
    ADCSRA |= _BV(ADSC);                          
    while (bit_is_set(ADCSRA, ADSC));
    result = ADCL;
    result |= ADCH << 8;
    result = 1126400L / result; 
    return result;
#elif defined(__arm__)
    return (3300);                              
#else
    return (3300);                                 
#endif
}
