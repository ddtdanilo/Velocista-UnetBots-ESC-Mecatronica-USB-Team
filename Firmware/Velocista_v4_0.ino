#include <NewPing.h>



byte Duty = 255/6;                     //Ciclo de trabajo de las PWM --> Inicialmente en 50%
byte Duty1 = Duty;
byte Duty2 = Duty;
int time = 0;


// ADSIGNACION DE PINES DEL ATMEGA
byte Pin0 = 13;
byte Pin1 = 12;
byte Pin2 = 4;
byte Pin3 = 8;
byte Pin4 = 7;
byte Pin_Led = A2;
  
byte PinPWM1 = 5;                    //Pines para las 2 PWM del motor -> Derecha
byte PinPWM2 = 6;                    //Izquierda

//Entradas y salidas de ultrasónicos

  byte PinTriggerF = 3;
  byte PinEchoF = 9;
  byte PinTriggerL = 10;
  byte PinEchoL= 11;
  int MaxDist = 300;
  // Distancia
  NewPing sonarF(PinTriggerF,PinEchoF,MaxDist);
  NewPing sonarL(PinTriggerL,PinEchoL,MaxDist);

//DEFINICION DE ARREGLOS Y VARIABLES AUXILIARES PARA EL CONTROL

int Rango[5] = {-5, -1, 0, 1, 5};        //Rangos definidos para el control
int Bits_Rango[5];                       //Variable que almacena cada bit por su rango: Bits_Rango = [b0*R0 b1*R1 b2*R2 ... bn*Rn]
int Ponderado = 0;                       //Variable que almacena la suma de cada componente del arreglo Bits_Rango 

void setup(){
   
//  DEFINICION DE PUERTOS DIGITALES A UTILIZAR PARA LOS SENSORES 

  pinMode(Pin0,INPUT);
  pinMode(Pin1,INPUT);
  pinMode(Pin2,INPUT);
  pinMode(Pin3,INPUT);
  pinMode(Pin4,INPUT);


Serial.begin(115200);
 
}

void loop(){
 //***************************************** Paso 1: Leer los bits del sensor *********************************
 
  byte b0 = digitalRead(Pin0);
  byte b1 = digitalRead(Pin1);
  byte b2 = digitalRead(Pin2);
  byte b3 = digitalRead(Pin3);
  byte b4 = digitalRead(Pin4);

  byte Bits[5] = {b0, b1, b2, b3, b4};  //Arreglo de unos y ceros

 //***************************************** Paso 2: Pasar los bits a rango -2:2 *******************************
 Serial.print("\nBits:  ");
  for(byte i=0;i<5;i++){
    Bits_Rango[i] = Bits[i]*Rango[i];
    Serial.print(Bits[i]);
  }
  Serial.print("   ");
  /*
  BIT    ESTADO RANGO 
  b0:     0|1    -2  
  b1:     0|1    -1 
  b2:     0|1     0  
  b3:     0|1     1  
  b4:     0|1     2  
  */
  

 //***************************************** Paso 3: Hacer la ponderación de los rangos *******************************
 
 //Se suman todos los rangos correspondientes a los bits activos
  Serial.print("Bits_Rango");
  Serial.print("   ");
  for(byte i=0;i<5;i++){
    Ponderado = Ponderado + Bits_Rango[i];  // Numero entre -3 y 3
    Serial.print(Bits_Rango[i]);
  }
  Serial.print("   ");
  Serial.print(Ponderado);
  Serial.print("   ");
    /*
    CASOS POSIBLES: MAXIMO DOS SENSORES CONSECUTIVOS PUEDEN ESTAR ACTIVOS A LA VEZ
    00000   // Carro fuera de linea -> Ponderado = 0
    10000   // Max desvio derecha   -> Ponderado = -2
    11000   // + Desvio derecha     -> Ponderado = -3
    01000   // Desvio derecha       -> Ponderado = -1
    01100   // Min desvio derecha   -> Ponderado = -1
    00100   // Centrado en línea    -> Ponderado = 0
    00110   // Min desvio izquierda -> Ponderado = 1
    00010   // Desvio izquierda     -> Ponderado = 1
    00011   // + Desvio izquierda   -> Ponderado = 3
    00001   // Max desvio izquierda -> Ponderado = 2
    
    11111   //Cruce de teipe en la pista  -> Ponderado = 0
    01111   //Cruce 90 grados en la pista -> Ponderado = 2 
    00111   //Cruce 90 grados en la pista -> Ponderado = 3 
    11110   //Cruce 90 grados en la pista -> Ponderado = 2
    11100   //Cruce 90 grados en la pista -> Ponderado = 3  
   
  */
  
/*
Ponderado (P)
Si P=0      --> El robot esta centrado en la línea
Si P>0      --> El robot se desvió a la izquierda
Si P<0      --> El robot se desvió a la derecha
Si |P| = 3  --> El robot esta en un cruce de 90 grados.
*/

 //***************************************** Paso 4: Cambiar rangos de -3:3 a 0:255 para controlar el Duty Cycle *******************************
 
  int Error = Ponderado;               //Valor entre -3:3
  byte Error_abs = abs(Ponderado);      //Valor entre 0:3
  byte Delta_Duty_down = map(Error_abs,0,6,0,Duty);    //Variacion del Duty para correccion 
  byte Delta_Duty_up = map(Error_abs,0,20,0,255-Duty);  //Variacion del Duty para correccion
  
 
  //***************************************** Paso 6: Enviar el Duty al PWM de un motor y (1-Duty) al segundo motor *******************************
 // Serial.print("Delta_Duty");
  //Serial.print("   ");
  //Serial.print(Delta_Duty);
  //Serial.print("   ");
  
  //Pines PWM para motores: 5 y 6   PWM1 -> Derecha   PWM2 -> Izquierda
  
  if(Error == 0){                 //El vehiculo esta centrado
   if(b2 == HIGH){
    analogWrite(PinPWM1,Duty);     //Motores a 50%
    analogWrite(PinPWM2,Duty);
   }
   else{
     analogWrite(PinPWM1,Duty1);     //Motores a 50%
     analogWrite(PinPWM2,Duty2);
   }
 }
 
  if(Error > 0){ //El vehiculo se desvio a la izquierda
    if(Error >= 5)
    {
    Duty1 = Duty-Delta_Duty_down;
    Duty2 = Duty;
    analogWrite(PinPWM1,Duty1);
    analogWrite(PinPWM2,Duty2);
    }
    else{
    Duty1 = Duty-Delta_Duty_down;
    Duty2 = Duty+Delta_Duty_up;
    analogWrite(PinPWM1,Duty1);
    analogWrite(PinPWM2,Duty2);
      }
    
 }
 
  if(Error < 0){    //El vehiculo se desvio a la derecha
   if(Error <= -5)
    {
     Duty1 = Duty; //Duty anterior
     Duty2 = Duty-Delta_Duty_down;
     analogWrite(PinPWM1,Duty1);
     analogWrite(PinPWM2,Duty2);
    }
    else{
     Duty1 = Duty+Delta_Duty_up; //Duty anterior
     Duty2 = Duty-Delta_Duty_down;
     analogWrite(PinPWM1,Duty1);
     analogWrite(PinPWM2,Duty2);
    }
     
    
 }
 Serial.print("Duty1");
 Serial.print("   ");
 Serial.print(Duty1);
 Serial.print("   ");
 Serial.print("Duty2");
 Serial.print("   ");
 Serial.print(Duty2);
 
 Ponderado = 0;
 
  int uSF = sonarF.ping();
  Serial.print("PingF: ");
  Serial.print(uSF / US_ROUNDTRIP_CM);
  Serial.print(" cm");
  Serial.print("   ");
  
  int uSL = sonarL.ping();
  Serial.print("PingL: ");
  Serial.print(uSL / US_ROUNDTRIP_CM);
  Serial.print(" cm");
  Serial.print("   ");
  Serial.print("\n");
  delay(10);
 
}
