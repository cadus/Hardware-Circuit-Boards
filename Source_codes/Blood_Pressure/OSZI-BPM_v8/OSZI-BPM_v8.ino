///Variablen Presure and Oszilometry Signal/////////////////////////////////////////////////////////////////////
volatile float P0Sensor = 75.3437;
volatile float PSensor = 0.0;
volatile float mSensor = 0.4626;
volatile uint16_t OSZsig = 0;
volatile float P0correction = 0;

volatile int16_t OSZMax = 0;
volatile uint16_t OSZDBPCrit = 0;
volatile uint16_t OSZSBPCrit = 0;
volatile uint16_t OSZDBPBottom = 0;

volatile float MAP = 0.0;
volatile float DBP = 0.0;
volatile float SBP = 0.0;

boolean BPMesure = false;

boolean FirstEnvelopePeak = true;
volatile uint32_t TimeMillis = 0;
volatile uint32_t TimeBeatDetected = 0;
volatile uint32_t BeatToBeatTime = 0;
volatile uint32_t n = 0;
volatile uint16_t BPM = 0;
volatile uint16_t BPM_Values[5];
volatile uint32_t EnvelopeBufferTime = 0;
volatile uint16_t OSZEnvelope = 0;
uint16_t OSZEnvelopeThreshold = 425;
volatile uint16_t OSZEnvelopeMax = 0;
boolean EnvelopeDetection = false;
boolean EnvelopePeakDetected = false;
volatile float POSZEnvelope = 0;
volatile uint16_t OSZValues[300];
volatile uint16_t PValues[300];
volatile uint16_t NoPeakTime = 0;
volatile uint16_t PresureMesureLevel = 150;


boolean CancelMesure = false;
boolean BeatDetected = false;
boolean PumpUp = false;
boolean displayVal = false;

char SerialCommand;

///Function ADCread//////////////////////////////////////////////////////////////////////////////////////////////
uint16_t ADCread(uint8_t adcpin){
  uint8_t ADCLval = 0;
  uint8_t ADCHval = 0; 
  ADMUX  &= (0xF0);
  ADMUX  |= (adcpin & 0x0F);
  ADCSRA |= (1 << ADSC);
  while(!(ADCSRA & (1 << ADIF)));
  ADCLval = ADCL;
  ADCHval = ADCH;
  ADCSRA |= (1 << ADIF);
  return (ADCHval<<2)|(ADCLval>>6);
}

///Timer2 Interupt Service Routine (4ms)/////////////////////////////////////////////////////////////////////////////////
ISR(TIMER2_COMPA_vect){
  EnvelopeBufferTime=EnvelopeBufferTime+4;
  NoPeakTime=NoPeakTime+4;
  OSZsig  = (0.6859*OSZsig+0.3141*ADCread(1));
  PSensor = 0.6859*PSensor+0.3141*((mSensor*ADCread(0))-P0Sensor); ///Presur Signal Sensor Filterd Lowpass 15Hz
}

///Setup//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {

  cli(); // Disable Interupts

///Setup ADCread////////////////////////////////////////////////////////////////////////////////////////////////////////
  ADCSRA  = 0x00;
  ADCSRB  = 0x00;
  ADMUX  |= (1 << REFS0); 
  ADMUX  |= (1 << ADLAR);    
  ADCSRA |= (1 << ADPS0)|(1 << ADPS2) ; 
  ADCSRA |= (1 << ADEN); 

///Setup Timer2//////////////////////////////////////////////////////////////////////////////////////////////////////////
  TCCR2A = 0x00;
  TCCR2B = 0x00;
  TCNT2  = 0x00;
  TCCR2A |=  (1<<WGM21);
  TCCR2B |=  (1<<CS21)|(1<<CS22);
  OCR2A   =   250-1;  
  TIMSK2 |=  (1<<OCIE2A); 
  
  sei(); // Enable Interupts

///Setup Digital Pins///////////////////////////////////////////////////////////////////////////////////////////////////
  pinMode(12,INPUT);  //BUTTON
  pinMode(11,OUTPUT);  //PUMP
  pinMode(10, OUTPUT); //VALVE 1
  pinMode(9, OUTPUT); //VALVE 2
  digitalWrite(11,LOW);
  digitalWrite(10,LOW);
  digitalWrite(9,LOW);

///Serial Setup (9600 Baud)//////////////////////////////////////////////////////////////////////////////////////////// 
  Serial.begin(9600);
}

void loop() {

//Abfrage des Seriellen Buffers wenn ein 'R' gesendet wurde Startet die Messung wenn ein 'S' gesendet wurde Stopt die Messung
  if(Serial.available()){
    SerialCommand = Serial.read();
    if(SerialCommand=='R'){
      PumpUp = true;
      for(uint8_t i=0 ; i<5 ; i++){
        P0correction = P0correction + PSensor;
        delay(5);
      }
      P0correction = P0correction/5;
      P0Sensor = P0Sensor+P0correction;
      P0correction=0;
      Serial.flush();
    }
    else if(SerialCommand=='S'){
      BPMesure = false;
      PumpUp = false;
      EnvelopeBufferTime = 0;
      OSZEnvelope = OSZEnvelopeThreshold;
      OSZMax = 0;
      SBP = 0;
      MAP = 0;
      DBP = 0;
      digitalWrite(10,LOW);
      digitalWrite(9,LOW);
      Serial.flush();
      for(uint16_t i = 0 ; i < 300 ; i++){
        OSZValues[i] = 0;
        PValues[i] = 0;
      }
    }
    else{
      Serial.flush();
    }
  }

//Wenn der Taster gedrückt wurde Startet eine messung
  if(digitalRead(12)==HIGH){
    PumpUp = true;
    for(uint8_t i=0 ; i<5 ; i++){
      P0correction = P0correction + PSensor;
      delay(5);
    }
    P0correction = P0correction/5;
    P0Sensor = P0Sensor+P0correction;
    P0correction=0;
  }

//Das Aufpumpen der Manschette Startet
  while(PumpUp==true){
    digitalWrite(10,HIGH);
    digitalWrite(9,HIGH);
    digitalWrite(11,HIGH);

    if(Serial.available()){
      SerialCommand = Serial.read();
      if(SerialCommand=='S'){
        BPMesure = false;
        PumpUp = false;
        EnvelopeBufferTime = 0;
        OSZEnvelope = OSZEnvelopeThreshold;
        OSZMax = 0;
        BPM = 0;
        SBP = 0;
        MAP = 0;
        DBP = 0;
        digitalWrite(11,LOW);
        digitalWrite(10,LOW);
        digitalWrite(9,LOW);
        Serial.flush();
      }
      else{
        Serial.flush();
      }
    }
     
    if(PSensor > 220){
      digitalWrite(11,LOW);
      delay(1000);
      PumpUp = false;
      BPMesure = true;
      OSZEnvelope = OSZEnvelopeThreshold;
      EnvelopeBufferTime = 0;
      OSZMax = 0;
      SBP = 0;
      MAP = 0;
      DBP = 0;
      NoPeakTime = 0;
      PresureMesureLevel = PSensor;
    }
  }

  if((BPMesure==true)&&(PSensor > 20.0)){

///Envelope Detection/////////////////////////////////////////////////////////////////////////////    
    if(OSZsig > OSZEnvelope){
      EnvelopeBufferTime = 0;
      OSZEnvelope = OSZsig;
      POSZEnvelope = PSensor;
      EnvelopeDetection = true;
    }
    
    if((EnvelopeBufferTime > 150)&&(EnvelopeDetection==true)){
      OSZEnvelopeMax = OSZEnvelope;
      
      if(OSZEnvelopeMax > 970){
        OSZEnvelope = OSZEnvelopeThreshold;
        EnvelopeBufferTime = 0;
        EnvelopeDetection = false;
      }
      
      if(FirstEnvelopePeak == false){
        
        for(uint16_t i = 0 ; i < 299 ; i++){
          OSZValues[i] = OSZValues[i+1];
          PValues[i] = PValues[i+1];
        }
        
          OSZValues[299] = OSZEnvelopeMax-425;
          PValues[299] = POSZEnvelope;
          OSZEnvelope = OSZEnvelopeThreshold;
          EnvelopePeakDetected = true;
          
          if(OSZEnvelopeMax > OSZMax){
            OSZMax = OSZEnvelopeMax;
            MAP = POSZEnvelope;
            OSZDBPCrit = (OSZMax-425)*0.81;
            OSZSBPCrit = (OSZMax-425)*0.45;
          }
          
      }

//Verringerung des Druckes zur Aufnahme eines neuem Messwertes
      if(EnvelopePeakDetected==true){
        PresureMesureLevel = PresureMesureLevel-5;
        while(PSensor > PresureMesureLevel){
          digitalWrite(9,LOW);
        }
        digitalWrite(9,HIGH);
        delay(1000);
        FirstEnvelopePeak=true;
        NoPeakTime = 0;
      }
      EnvelopePeakDetected = false;
      FirstEnvelopePeak = false;
      EnvelopeDetection = false;
    }

//Wenn für 2sec keine Oszillation detektiert wurde wird der Druck in der Manschette verringert
    if(NoPeakTime > 2000){
      PresureMesureLevel = PresureMesureLevel-5;
      while(PSensor > PresureMesureLevel){
        digitalWrite(9,LOW);
      }
      digitalWrite(9,HIGH);
      delay(1000);
      FirstEnvelopePeak=true;
      NoPeakTime = 0;
    }

///SBP-DBP Detection//////////////////////////////////////////////////////////////////////////////////
    if((OSZValues[296] > OSZValues[297])&&(OSZValues[297] <= (OSZDBPCrit))&&(OSZValues[296] > (OSZDBPCrit))&&(OSZValues[297] > OSZValues[298])&&(OSZValues[297] > OSZValues[299])){
      DBP = PValues[297];
      for(uint16_t i = 0 ; i < 300 ; i++){
        if((OSZValues[i] < OSZSBPCrit)&&(OSZValues[i+1] <= OSZSBPCrit)&&(OSZValues[i+2] > OSZSBPCrit)){
          SBP=PValues[i+1];
          break;
        }
      }
      BPMesure = false;
      if(SBP > 0){
      displayVal = true;
      }
      else{
        Serial.print(String("BD")+00);
        Serial.println(String("b")+00);
      }
      for(uint16_t i = 0 ; i < 300 ; i++){
        OSZValues[i] = 0;
        PValues[i] = 0;
      }
    }
  }
  else{
    BPMesure = false;
    digitalWrite(10,LOW);
    digitalWrite(9,LOW);
  }


//Übertragung der Blutdruckwerte.
  if(displayVal == true){    
//    Serial.print(PSensor);
//    Serial.print('\t');
//    Serial.print(OSZsig);
//    Serial.print('\t');       
    Serial.print(String("BD")+int(SBP));
//    Serial.print('\t');
//    Serial.print(String("MAP: ")+MAP);
//    Serial.print('\t');
    Serial.println(String("b")+int(DBP));
//    Serial.println();
    displayVal = false;
  }
}
