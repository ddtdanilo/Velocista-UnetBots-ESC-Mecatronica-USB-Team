#include <PID_v1.h>

byte Kp = 2;
byte Ki = 5;
byte Kd = 1;
int Duty;

double Setpoint = 0;                     //Variable de referencia para el control
double Input;                            //Variable que se quiere controlar
double Output;                           //Variable que será ajustada por el PID

PID PIDController(&Input, &Output, &Setpoint,Kp,Ki,Kd, DIRECT); // Se define el controlador

/*
    SetPoint ----->O---->|Gp|---->|Gc|-----------> Output
                   ^                       |
            Input  |                       |
                   +---------|K|-----------+
*/

//  DEFINIR PUERTOS DIGITALES A UTILIZAR PARA LOS SENSORES 

//Entrada de sensores infrarrojos
byte Pin0 = 2;
byte Pin1 = 4;
byte Pin2 = 7;
byte Pin3 = 8;
byte Pin4 = 12;
byte Pin5 = 13;
byte Pin6 = A0;
byte Pin7 = A1;

//Salida de pwm para motores
byte PinPWM1 = 5;
byte PinPWM2 = 6;

//Entradas y salidas de ultrasónicos
byte PinTriggerF = 3;
byte PinEchoF = 9;
byte PinTriggerL = 10;
byte PinEchoL= 11;


byte Rango[] = {-4,-3,-2,-1,1,2,3,4};         //Rangos definidos para el control
byte Bits_Rango[7];                       //Variable que almacena cada bit por su rango: Bits_Rango = [b0*R0 b1*R1 b2*R2 ... bn*Rn]
byte Ponderado = 0;                         //Variable que almacena la suma de cada componente del arreglo Bits_Rango
  
void setup(){
  

  pinMode(Pin0,INPUT);
  pinMode(Pin1,INPUT);
  pinMode(Pin2,INPUT);
  pinMode(Pin3,INPUT);
  pinMode(Pin4,INPUT);
  pinMode(Pin5,INPUT);
  pinMode(Pin6,INPUT);
  pinMode(Pin7,INPUT);
  
  PIDController.SetMode(AUTOMATIC);
  
}

void loop(){
 //***************************************** Paso 1: Leer los bits del sensor *******************************
 //b0 b1 b2 b3 b4 b5 b6 b7 --> 8 Pines digitales de entrada
 
byte b0 = digitalRead(Pin0);
byte b1 = digitalRead(Pin1);
byte b2 = digitalRead(Pin2);
byte b3 = digitalRead(Pin3);
byte b4 = digitalRead(Pin4);
byte b5 = digitalRead(Pin5);
byte b6 = digitalRead(Pin6);
byte b7 = digitalRead(Pin7);

byte Bits[] = {b0,b1,b2,b3,b4,b5,b6,b7};  //Arreglo de unos y ceros

 //***************************************** Paso 2: Pasar los bits a rango -4:4 *******************************
 //8 bits --> -4 -3 -2 -1 1 2 3 4
 
for(byte i=0;i<8;i++){
  Bits_Rango[i] = Bits[i]*Rango[i];
}

 //***************************************** Paso 3: Hacer la ponderación de los rangos *******************************
 //Se suman todos los rangos correspondientes a los bits activos

for(byte i=0; i<8 ; i++){
  Ponderado = Ponderado + Bits_Rango[i];  // Numero entre -10 y 10
}
/*
Ponderado (P)
Si P=0 --> El robot esta centrado en la línea
Si P>0 --> El robot se desvió a la izquierda
Si P<0 --> El robot se desvió a la derecha
Si |P| = 9 ó |P| = 10 --> El robot esta en un cruce de 90 grados.
*/

Input = Ponderado;                        //Variable que se desea controlar (Se quiere que Input = SetPoint = 0 para mantener centrado el robot)

 //***************************************** Paso 4: Aplicar control PID *******************************
 
 PIDController.Compute();

 //***************************************** Paso 5: Cambiar rangos de -10:10 a 0:255 para controlar el Duty Cycle *******************************
 
 Duty = map(Output,-10,10,0,255);  //Duty de 0-255
 
  //***************************************** Paso 6: Enviar el Duty al PWM de un motor y (1-Duty) al segundo motor *******************************
  
  //Pines PWM para motores: 5 y 6   PWM1 -> Derecha   PWM2 -> Izquierda
  
  
 if(Output == 0){
 
   analogWrite(PinPWM1,Duty);
   analogWrite(PinPWM1,Duty);
 
 }
 
if(Output != 0){
 
   analogWrite(PinPWM1,255-Duty);
   analogWrite(PinPWM2,Duty);
 }
 
}
