# pIMU: Unidad de medición inercial portatil

## Características

pIMU es una unidad de medición inercial (acelerómetro, giroscopio y magnetómetro) capaz de enviar los datos muestreados de forma inalámbrica. Sus principales características son:

· Dimensiones: 18x12mm
· Tensión de trabajo: 3,5V-1V
· Consumo: 9mA (consumo optimizado en el código)
· Transmisor y protcolo: NRF24L01
· Frecuencia de emisión: 2,4GHz

## Descripción

El sensor principal encargado de la recolección de datos es un MPU9250. Éste es controlado por un microcontrolador Atmega328p que, a su vez, envía los datos en bruto a un módulo NRF24L01 para su transmisión. El regulador de tensión será un TLV61225, capaz de elevar tensiones de hasta 0,7V a los 3,3V necesarios para el funcionamiento del resto de integrados.

Este módulo está pensado para su alimentación mediante un supercondensador de tensión nominal entre 2,5V y 3,5V. Dado su bajo consumo, el uso de un supercondensador permite tiempos de uso superiores a las 2h con tiempos de carga de apenas unos segundos. 

El Atmega328p será flasheado con un el firmware Optiboot, con el fin de poder utilizar lenguaje Arduino en su programación y permitir que cualquier persona sin conocimientos de este micro concreto pueda programarlo a su gusto. La PCB cuenta con pads de soldadura suficientemente grandes asociados a los pines TX, RX y RST del microcontrolador, para poder subir sketches de forma sencilla mediante un adaptador USB-UART.  

## Instrucciones de montaje

Una vez adquiridos todos los componentes (Bill Of Materials disponible en el proyecto de Kicad), los pasos a seguir para conseguir una pIMU funcionando serán:

1. Soldadura del TLV, Atmega, sus condensadores de desacoplo y la resistencia de pull-up para el reset

2. Conexión de la interfaz SPI al ordenador. Para ello, podemos tomar como puertos de SPI los pines correspondientes al módulo NRF24L01 aún sin soldar y conectarlos a un programador ISP. En caso de no disponer de dicho programador, es posible fabricarse uno con un Arduino (ENLACE).

3. Cambio de los fuses (¿fusibles?) del micro. Para ello se usará el programa avrdude. Buscamos optimizar el espacio reservado al bootloader, utilizar el reloj interno a 8MHz para un menos consumo y desactivar o rebajar la detección de Brown-Out. En nuestro caso, los valores elegidos son LowFuse: 0xFF; HighFuse: 0xDE; ExtendedFuse: 0xFD. 

4. Grabado del bootloader. Existen múltiples formas de realizar este paso. La opción recomendada es hacer uso del programa de Nick Gammon https://www.gammon.com.au/bootloader. Este programa permite además cambiar los fuses de forma directa, en caso de tener problemas con avrdude en el paso anterior. La opción Lilypad 8MHz incluye ya algunas de las opciones mencionadas, como el uso de reloj interno u optimización del espacio de arranque.

5. Comprobación del bootloader. Para ello, basta soldar tres cables a los pines TX, RX y RST del pIMU, conectarlos a un conversor de USB-UART y probar a subir un sketch de Arduino (seleccionando la placa Lilypad 8MHz en caso de haber optado por esa opción en el caso anteior)

6. Soldadura del resto de componentes.

7. Programación del Atmega con el código final. Este código está disponible en este repo, e incluye la adquisicion de datos y su envío de forma periódica mediante uso de la interrupción WDT y la configuración a modo dormido del micro entre mediciones.
