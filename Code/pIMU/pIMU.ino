/*
 * portable Inertial Measurement Unit
 * 
 * Instructions and more can be found at:
 * 
 * https://github.com/pepassaco/pIMU
 * 
 * by Pablo Alvarez Dominguez.
 */
 
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "MPU9250.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>


MPU9250 IMU(SPI,5);     //IMU MPU9250 por SPI, con CS en pin 5
int status;             //Estado de la IMU
double msg[3];          //Array donde se almacenan los valores del acelerometro

RF24 radio(9, 8);       // CE, CSN el emisor de radio NRF24L01     
const byte address[6] = "00001"; 

volatile int f_wdt=1;   //Flag para el WDT


ISR(WDT_vect)           //Rutina de atencion a la interrupcion del watchdog 
{
  if(f_wdt == 0)        //se limpia el flag
  {
    f_wdt=1;
  }
}





void imuConf(){                 //Configuracion inicial de la imu
  status = IMU.begin();
  if (status < 0) {             //Si no se inicia, no hagas nada

    while(1) {
    }
    
  }
}

void nrfConf(){                                 //Configuracion de la radio
  radio.begin();                                //Inicio
  radio.openWritingPipe(address);               //Se abre una tuberia de datos en la direccion especificada
  radio.setPALevel(RF24_PA_MIN);                //Nivel de potencia de tx al minimo 
  radio.stopListening();                        //Desactiva la escucha (solo tx)
}

void wdtConf(){                                 //Configuracion del WDT (registros del microcontrolador)
  
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);

  /* set new watchdog timeout prescaler value */
  WDTCSR = 0<<WDP0 | 0<<WDP1 | 0<<WDP2 | 0<<WDP3; /* 16 miliseconds */
  
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
}




void enterSleep(void){                    //Metodo que duerme al micro
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    //Selecciona como modo el apagado del micro
  
  sleep_mode();                           //Entra en el modo seleccionado
  
  ADCSRA = 0;                             //Desactiva el ADC
  power_all_disable();                    //Desactiva todos los modulos

  power_timer0_enable();                  //Reactiva el timer0
  power_spi_enable();                     //Reactiva el SPI
}




void manda() {
  
  IMU.readSensor();
  
  msg[0] = IMU.getAccelX_mss();
  msg[1] = IMU.getAccelY_mss();
  msg[2] = IMU.getAccelZ_mss();

  radio.powerUp();
  radio.write(&msg, sizeof(msg));
  radio.powerDown();  
  
}






void setup() {
  
  ADCSRA = 0;
  
  power_all_disable(); 

  power_timer0_enable();
  power_spi_enable();
  
  imuConf();
  nrfConf();
  wdtConf();  
  
}


void loop(){
  
  if(f_wdt == 1){
    manda();
    f_wdt = 0;      //limpiamos el flag
    enterSleep();
  }
  
}
