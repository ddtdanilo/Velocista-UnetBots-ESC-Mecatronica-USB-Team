#include <NewPing.h>

byte Duty = 255/6;                     //Ciclo de trabajo de las PWM
byte Duty1 = Duty;
byte Duty2 = Duty;
byte DutyObs = Duty;

/*
int Obs_l = 300;
int Obs_w = 500;
int Turn = 600;
*/

int Paso1 = 700;   //Primer giro (izquierda)
int Paso2 = 250;   //Primera recta
int Paso3 = 600;   //Segundo giro (derecha)
int Paso4 = 550;   //Segunda recta
int Paso5 = 600;   //Tercer giro (derecha)
int Paso6 = 300;   //Tercea recta
int Paso7 = 750;   //Ultimo giro (izquierda)


int time = 0;


// ASIGNACION DE PINES DEL ATMEGA
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
  int uSF;
  int uSL;
  int DistF;
  int DistL;
  
  // Distancia
  NewPing sonarF(PinTriggerF,PinEchoF,MaxDist);
  NewPing sonarL(PinTriggerL,PinEchoL,MaxDist);

//DEFINICION DE ARREGLOS Y VARIABLES AUXILIARES PARA EL CONTROL

byte b0;
byte b1;
byte b2;
byte b3;
byte b4;
byte Bits[5]; //Arreglo de unos y ceros

int Rango[5] = {-5, -1, 0, 1, 5};        //Rangos definidos para el control
int Bits_Rango[5];                       //Variable que almacena cada bit por su rango: Bits_Rango = [b0*R0 b1*R1 b2*R2 ... bn*Rn]
int Ponderado = 0;                       //Variable que almacena la suma de cada componente del arreglo Bits_Rango 

/************************** FUNCIONES ******************************************/


void readSensors(){
  
  b0 = digitalRead(Pin0);
  b1 = digitalRead(Pin1);
  b2 = digitalRead(Pin2);
  b3 = digitalRead(Pin3);
  b4 = digitalRead(Pin4);
	        
  Bits[0] = b0;
  Bits[1] = b1;
  Bits[2] = b2;
  Bits[3] = b3;
  Bits[4] = b4;
  
  for(byte i=0;i<5;i++)
 {
  Bits_Rango[i] = Bits[i]*Rango[i];
  }
  //*****************************************Hacer la ponderación de los rangos *******************************
  Ponderado = 0;
  //Se suman todos los rangos correspondientes a los bits activos
  for(byte i=0;i<5;i++)
  {
     Ponderado = Ponderado + Bits_Rango[i];  
  }
 
}

void stopWheels(int delayStop){
 analogWrite(PinPWM1,0); //Freno
 analogWrite(PinPWM2,0);
 delay(delayStop);
}
 
void turnRight(int delayTurn){
  analogWrite(PinPWM1,0);
  analogWrite(PinPWM2,DutyObs);
  delay(delayTurn);
}

void turnLeft(int delayTurn){
  analogWrite(PinPWM1,DutyObs);
  analogWrite(PinPWM2,0);
  delay(delayTurn);
}

void keepGoing(int delayForward){
  analogWrite(PinPWM1,DutyObs);
  analogWrite(PinPWM2,DutyObs);
  delay(delayForward);
}


void measureDistL()
{
	uSL = sonarL.ping();
	DistL = uSL / US_ROUNDTRIP_CM;
}

void measureDistF()
{
	uSF = sonarF.ping();
	DistF = uSF / US_ROUNDTRIP_CM;
}

/*********************************************************************************/
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

	
	Serial.print("PingFront: ");
	measureDistF();
	Serial.print(DistF);
	Serial.print(" cm");
	Serial.print("   ");
	
	
	Serial.print("PingLat: ");
	measureDistL();
	Serial.print(DistL);
	Serial.print(" cm");
	Serial.print("   ");
	Serial.print("\n");
        
        
        
    readSensors();

	

	if(DistF <= 14 && DistF != 0) //Caso de obstaculo
	{
      stopWheels(300);
 
 	while(Ponderado > 0) 
 	{
 	 readSensors();
 	 turnRight(0);
 	}
 	while(Ponderado < 0)
 	{
 	 readSensors();
 	 turnLeft(0);
 	}
 	
 		stopWheels(500);
 	 	//Paso 1: Girar izquierda
	        while(DistL == 0 || DistL > 20) 
                {
	 	turnLeft(0);
                
		measureDistL();
                delay(35);
                }
                 stopWheels(500);
	 	 //Paso 2:  Seguir recto
		while(DistL <= 15 && DistL != 0){
			keepGoing(0);
                        measureDistL();
                        delay(35);
		}
                stopWheels(500);


	 	 //Paso 3: Girar derecha
	
	 	 turnRight(Paso3);
                 stopWheels(500);
	
	 	 //Paso 4: Seguir recto bordeando el objeto
	
	 	// while(DistL <= 8 && DistL != 0){
		//	measureDistL();
			keepGoing(Paso4);
		//}
                 stopWheels(500);
	
	 	 //Paso 5: Girar derecha
	
	 	 turnRight(Paso5);
                 stopWheels(500);
	
	 	 //Paso 6: Seguir recto hasta la linea
	         measureDistL();
                 while(DistL == 0 || DistL > 20) 
                 {
                   keepGoing(0);
                   measureDistL();
                   //delay(35);
                 }
                 
                 stopWheels(500);
                 
	 	 while(DistL <= 20 && DistL != 0){
			measureDistL();
			keepGoing(0);//Paso6
                        //delay(35);
		}
                 stopWheels(500);
	
	
	 	 //Paso 7: Girar izquierda para estar sobre la linea
	
 	 	turnLeft(Paso7);
                stopWheels(500);
             
  		measureDistF();
  		measureDistL();

	}


	else
	{

 		readSensors();
  
		/*
		Ponderado (P)
		Si P=0      --> El robot esta centrado en la línea
		Si P>0      --> El robot se desvió a la izquierda
		Si P<0      --> El robot se desvió a la derecha
		Si |P| = 6  --> El robot esta en un cruce de 90 grados.
		*/

 		//***************************************** Paso 4: Cambiar rangos de -3:3 a 0:255 para controlar el Duty Cycle *******************************
 
  		int Error = Ponderado;               //Valor entre -6:6
  		byte Error_abs = abs(Ponderado);      //Valor entre 0:6
  		byte Delta_Duty_down = map(Error_abs,0,6,0,Duty);    //Variacion del Duty para correccion 
  		byte Delta_Duty_up = map(Error_abs,0,20,0,255-Duty);  //Variacion del Duty para correccion
  		
 		
 		//***************************************** Paso 6: Enviar el Duty al PWM de un motor y (1-Duty) al segundo motor *******************************
 		// Serial.print("Delta_Duty");
 		//Serial.print("   ");
 		//Serial.print(Delta_Duty);
 		//Serial.print("   ");
 		 		
  		//Pines PWM para motores: 5 y 6   PWM1 -> Derecha   PWM2 -> Izquierda
  
  		if(Error == 0)
  		{//El vehiculo esta centrado
  			if(b2 == HIGH)
  			{
  			 analogWrite(PinPWM1,Duty);     //Motores a 50%
  			 analogWrite(PinPWM2,Duty);
  			}
  			else
  			{
  			  analogWrite(PinPWM1,Duty1);     //Motores a 50%
  			  analogWrite(PinPWM2,Duty2);
  			}
 		}		
 		
 		if(Error > 0)
 		{ //El vehiculo se desvio a la izquierda
 		 	if(Error >= 5) //Curva
 		 	{
 		 	Duty1 = Duty-Delta_Duty_down;
 		 	Duty2 = Duty;
                        
 		 	analogWrite(PinPWM1,Duty1);
 		 	analogWrite(PinPWM2,Duty2);
 		 	}
 		 	else
 		 	{
 		 	Duty1 = Duty-Delta_Duty_down;
 		 	Duty2 = Duty+Delta_Duty_up;
 		 	analogWrite(PinPWM1,Duty1);
 		 	analogWrite(PinPWM2,Duty2);
 		 	}		  
 		}		
 		
 		if(Error < 0)
 		{    //El vehiculo se desvio a la derecha
 			if(Error <= -5) //Curva
 			 {
 			  Duty1 = Duty; 
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
 		Serial.print("   ");
 		Serial.print("\n");

 		Ponderado = 0;
 		
 }

  delay(10);
 
}



