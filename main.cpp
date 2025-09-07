/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

#include "TM1638.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>

// Define los pines que usarás para conectar el TM1638
#define STB_PIN D10  // Pin de STB (Strobe)
#define CLK_PIN D9  // Pin de CLK (Clock)
#define DIO_PIN D8  // Pin de DIO (Data In/Out)

//prototipos 
void leer_botones(void);
int decodeKey(uint32_t keys);

//variables 

int boton = 0;



//hilos y sistema operativo
Thread T_leer_botones (osPriorityNormal,4096, NULL, NULL);
Thread T_contador (osPriorityNormal, 4096, NULL, NULL);
TM1638 tm1638(STB_PIN, CLK_PIN, DIO_PIN);
//eventos
//EventFlags eventos;
int main() {
    // Inicializo el display y programa 
 
    tm1638.init();
    tm1638.setBrightness(7); // Ajusta el brillo al máximo
    printf("Arranque del programa\n\r");
    
    // Pruebo que funcione
    tm1638.displayNumber(12345678); // Muestra un número en el display
    wait_us(2e6);
    tm1638.clearDisplay();          // Limpia el display

    //Inicializo variables 
    bool state = 0;

    int conteo = 0;
    bool flag1 = 1;

    int ledVUM = 0;

    //Menu
    enum Modo 
    {
        IDLEmenu,
        CONTADORmenu,
        JUEGO,
        VUMETRO
    };
    int modoActual = IDLEmenu ;

    // Arranco hilos 
    T_leer_botones.start(leer_botones);
    

    while (true) 

         {
            switch (modoActual) {
            case IDLEmenu:
                tm1638.displayNumber(00000000);
                ThisThread::sleep_for(100ms);
                tm1638.clearDisplay();
                modoActual = boton;
                //eventos.wait_any(1);
                break;
            case CONTADORmenu:

                modoActual = CONTADORmenu;
                if (boton > 0) {
                state = !state;
                tm1638.setLed(1,state);
             }
                if(boton==4)
                {
                    flag1 = !flag1;
                    
                }

                if(flag1 == 1)
                {
                    tm1638.displayNumber(conteo++);
                }else if (flag1 == 0) {
                    
                    tm1638.displayNumber(conteo--);

                }
                ThisThread::sleep_for(100ms);
               // eventos.wait_any(1);
                break;
            case VUMETRO:
                modoActual = VUMETRO;
                
                if (boton == 8) {
                    ledVUM++;
                    tm1638.setLed(ledVUM, 1);
                    ThisThread::sleep_for(100ms);
                    tm1638.displayNumber(ledVUM);
                    if (ledVUM == 8) {
                        ThisThread::sleep_for(600ms);
                        tm1638.clearLeds();
                        ledVUM = 0;
                        tm1638.clearDisplay();
                    }

                }
                
               
                }    


             
             
        
             ThisThread::sleep_for(100ms);
        
        }
       
}


void leer_botones(void)
{
    int btnLOC = 0;
    while (true) {
        int numButton = 0;
        uint32_t keysDEB = 0;
        uint32_t keys = tm1638.readKeys();
        ThisThread::sleep_for(10ms);
        uint32_t keysCHECK = tm1638.readKeys();
        if (keys == keysCHECK){
            keysDEB = keys;     
        }else {
            keysDEB = 0;
        }

        boton = decodeKey(keysDEB);
          
        // Imprime la palabra completa y los bytes individuales
       /* uint8_t b0 = (keysDEB >> 0) & 0xFF;
        uint8_t b1 = (keysDEB >> 8) & 0xFF;
        uint8_t b2 = (keysDEB >> 16) & 0xFF;
        uint8_t b3 = (keysDEB >> 24) & 0xFF;*/

        //printf("KEYS: %08X  bytes: %02X %02X %02X %02X\n", keysDEB, b0, b1, b2, b3);
        
       switch (boton) {
            case 1:
                boton = 1;
                break;
            case 9:
                boton = 2;
                break;
            case 17:
                boton = 3;
                break;
            case 25:
                boton = 4;
                break;
            case 5:
                boton = 5;
                break;
            case 13:
                boton = 6;
                break;
            case 21:
                boton = 7;
                break;
            case 29:
                boton = 8;
                break;
       } 
        //eventos.set(1);
        ThisThread::sleep_for(50ms);
        
    }
}




int decodeKey(uint32_t keys) {
    if (keys == 0) return 0;  // ningún botón

    for (int i = 0; i < 32; i++) {
        if (keys & (1UL << i)) {
            return i + 1; // Botón encontrado (empieza en 1)
        }
    }
    return 0;
}
