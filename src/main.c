
#include<math.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


//! IO specification

// Define filas (ánodos)
#define ROW_1 22
#define ROW_2 23
#define ROW_3 21
#define ROW_4 19
#define ROW_5 32
#define ROW_6 33
#define ROW_7 25
#define ROW_8 26

//rojos
#define COL_1 15
#define COL_2 0
#define COL_3 16
#define COL_4 5

#define COL_8 27
#define COL_7 14
#define COL_6 12
#define COL_5 13

//verdes
#define COL_4_VER 2
#define COL_6_VER 4
#define COL_3_VER 17
#define COL_5_VER 18

// Define botones (entradas)

#define GPIO_UP 35
#define GPIO_DOWN 34
#define GPIO_LEFT 39
#define GPIO_RIGHT 36


// Arreglos para recorrer filas y columnas fácilmente
const int rows[] = {ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8};
const int cols[] = {COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8};

const int cols_ver[] = {COL_4_VER, COL_6_VER, COL_3_VER, COL_5_VER};

#define ROW_MASK ((1ULL<<ROW_1)|(1ULL<<ROW_2)|(1ULL<<ROW_3)|(1ULL<<ROW_4)|\
                  (1ULL<<ROW_5)|(1ULL<<ROW_6)|(1ULL<<ROW_7)|(1ULL<<ROW_8))
#define COL_MASK ((1ULL<<COL_1)|(1ULL<<COL_2)|(1ULL<<COL_3)|(1ULL<<COL_4)|\
                  (1ULL<<COL_5)|(1ULL<<COL_6)|(1ULL<<COL_7)|(1ULL<<COL_8))|\
                  (1ULL<<COL_4_VER)|(1ULL<<COL_6_VER)|(1ULL<<COL_3_VER)|(1ULL<<COL_5_VER)

//!




typedef struct enemy {
    int position[8][8];
} enemy_t;

typedef struct player {
    int position[8][8];
} player_t;

typedef struct obstacles{
    int position[8][8];
} obstacles_t;


typedef struct tablero {
    int field[8][8][4];
    player_t player;
    enemy_t enemy1;
    enemy_t enemy2;
    obstacles_t obstacles;
} tablero_t;


//! Variables globales
tablero_t tablero;
int led_state[8][8] = {0}; // Estado de los LEDs (0: apagado, 1: encendido, 2: parpadeando)
bool led_blink_aux = 0;
uint32_t led_blink_iterations_aux = 1000; // Tiempo de parpadeo en ms (1 segundo)
int playerposition[2]; // Posición del jugador (x, y)
bool runnning = true; // Variable para controlar el bucle de ejecución
//!

void clear_tablero() {
    // Limpia el tablero y las posiciones de los enemigos y el jugador
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            tablero.field[i][j][0] = 0; // Jugador
            tablero.field[i][j][1] = 0; // Enemigo 1
            tablero.field[i][j][2] = 0; // Enemigo 2
            tablero.field[i][j][3] = 0; // Obstáculos
        }
    }
}

void init_tablero() {
    // Inicializa el tablero y las posiciones de los enemigos y el jugador
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            tablero.field[i][j][0] = 0; // Jugador
            tablero.field[i][j][1] = 0; // Enemigo 1
            tablero.field[i][j][2] = 0; // Enemigo 2
            tablero.field[i][j][3] = 0; // Obstáculos
        }
    }

    // Inicializa la posición del jugador en el centro del tablero
    tablero.player.position[7][7] = 10;
    // Inicializa la posición de los enemigos en posiciones aleatorias
    if(runnning){
        tablero.enemy1.position[1][1] = 1;
        tablero.enemy2.position[1][1] = 1;

            // for (int j = 3; j < 6; j++) {
            //     int col_height = rand() % 4; // Inicializa la altura de la columna
            //     int start_index = rand() % 2; // Inicializa el ancho de la columna
            //     for (int h = 0; h < col_height; h++) {
            //         tablero.obstacles.position[1+h+start_index][j] = 1; // Inicializa la posición de los enemigos
            //         tablero.field[1+h+start_index][j][3] = 1; // Inicializa la posición de los enemigos
            //         gpio_set_level(rows[1+h+start_index], 1); // Enciende la fila del jugador
            //         gpio_set_level(cols_ver[j], 1); // Enciende la columna del jugador
            //     }
            // }

    }

    playerposition[0] = 7; // Posición inicial del jugador
    playerposition[1] = 7; // Posición inicial del jugador

    // printf("Tablero inicializado\n");
    // printf("Posicion Jugador: %d\n", tablero.player.position[3][3]);    
    // printf("Posicion Enemigo 1: %d\n", tablero.enemy1.position[1][1]);
    // printf("Posicion Enemigo 2: %d\n", tablero.enemy2.position[6][6]);
    tablero.player.position[7][7] = 10; // Actualiza la posición del jugador
    runnning = true; // Inicia el bucle de ejecución
}

void creature_random_motion(){
    int x_index;
    int y_index;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if(tablero.enemy1.position[i][j] == 1) {
                
                tablero.enemy1.position[i][j] = 0; // Limpia la posición anterior del jugador

                x_index = i + (rand() % 3) - 1; // Movimiento aleatorio en x
                y_index = j + (rand() % 3) - 1; // Movimiento aleatorio en y

                x_index = round((playerposition[0] - x_index)*0.5  + x_index); // Movimiento aleatorio en x
                y_index = round((playerposition[1] - y_index)*0.5  + y_index);  // Movimiento aleatorio en y
                
                
                if(y_index < 0) y_index = j; // Limita el movimiento a la matriz
                if(y_index > 7) y_index = j; // Limita el movimiento a la matriz
                if(x_index < 0) x_index = i; // Limita el movimiento a la matriz
                if(x_index > 7) x_index = i; // Limita el movimiento a la matriz
            }
        }
    }
    tablero.enemy1.position[x_index][y_index] = 1;


    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if(tablero.enemy2.position[i][j] == 1) {
                
                tablero.enemy2.position[i][j] = 0; // Limpia la posición anterior del jugador

                x_index = i + (rand() % 3) - 1; // Movimiento aleatorio en x
                y_index = j + (rand() % 3) - 1; // Movimiento aleatorio en y

                x_index = round((playerposition[0] - x_index)*0.17  + x_index); // Movimiento aleatorio en x
                y_index = round((playerposition[1] - y_index)*0.17  + y_index);  // Movimiento aleatorio en y
                
                if (abs(x_index) + abs(y_index) == 2) {
                    x_index = i; // Limita el movimiento a la matriz
                    y_index = j; // Limita el movimiento a la matriz
                }

                if (isnan(x_index*0.1) || isnan(x_index*0.1)) {
                    x_index = i; // Limita el movimiento a la matriz
                    y_index = j; // Limita el movimiento a la matriz
                }

                if(y_index < 0) y_index = j; // Limita el movimiento a la matriz
                if(y_index > 7) y_index = j; // Limita el movimiento a la matriz
                if(x_index < 0) x_index = i; // Limita el movimiento a la matriz
                if(x_index > 7) x_index = i; // Limita el movimiento a la matriz
            }
        }
    }
    tablero.enemy2.position[x_index][y_index] = 1;
    vTaskDelay(pdMS_TO_TICKS(100)); // Espera 100 ms antes de la siguiente iteración
}

void update_tablero_values(){
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            tablero.field[i][j][0] = tablero.player.position[i][j]; // Jugador
            tablero.field[i][j][1] = tablero.enemy1.position[i][j]; // Enemigo 1
            tablero.field[i][j][2] = tablero.enemy2.position[i][j]; // Enemigo 2

            int creature_sum = tablero.field[i][j][0] + tablero.field[i][j][1] + tablero.field[i][j][2];
            if (creature_sum > 0 && creature_sum < 10) {
                led_state[i][j] = 1; // Encender LED si hay una criatura
            }else if (creature_sum == 0) {
                led_state[i][j] = 0; // apagar LED si no hay criaturas
            }else if (creature_sum == 10) {
                led_state[i][j] = 2; // Parpadear LED si hay jugador
            }else if (creature_sum > 10) {
                runnning = false; // Parpadear LED si hay jugador
                init_tablero(); // Parpadear LED si hay jugador
               
        }
    }
    }
}

void update_player_movement(){
    int y = 0;
    int x = 0;
    if(gpio_get_level(GPIO_UP) == 1) {
        y = 1; // Mover hacia arriba
    }else if(gpio_get_level(GPIO_DOWN) == 1) {  
        y = -1; // Mover hacia abajo
    }else if(gpio_get_level(GPIO_LEFT) == 1) {
        x = -1; // Mover hacia la izquierda
    }else if(gpio_get_level(GPIO_RIGHT) == 1) {
        x = 1; // Mover hacia la derecha
    }

    int x_index = 1;
    int y_index = 1;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if(tablero.player.position[i][j] == 10) {
       
                tablero.player.position[i][j] = 0; // Limpia la posición anterior del jugador
                
                x_index = i + x; // Movimiento aleatorio en x
                y_index = j + y; // Movimiento aleatorio en y
                
                if(y_index < 0) y_index = j; // Limita el movimiento a la matriz
                if(y_index > 7) y_index = j; // Limita el movimiento a la matriz
                if(x_index < 0) x_index = i; // Limita el movimiento a la matriz
                if(x_index > 7) x_index = i; // Limita el movimiento a la matriz
            }
        }
    }
    if (runnning) {
        playerposition[0] = x_index; // Actualiza la posición del jugador
        playerposition[1] = y_index; // Actualiza la posición del jugador 
        tablero.player.position[x_index][y_index] = 10; // Actualiza la posición del jugador
    }
    }


//! --------------------------------------------------------------------------------------------


void update_board_task_fn(){
    int iterations = 0;
    while (1 && runnning)
    {
        if (iterations % 4 == 0) {
            creature_random_motion();
        }
        update_player_movement(); // Actualiza la posición del jugador según el movimiento
        update_tablero_values(); // Actualiza el estado de los LEDs según el tablero
        iterations += 1; // Incrementa el contador de iteraciones
        vTaskDelay(pdMS_TO_TICKS(200)); // Espera 100 ms antes de la siguiente iteración
    }
}

void light_up_matrix_task_fn() {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (led_state[row][col] == 1) {
                gpio_set_level(rows[row], 1); 
                gpio_set_level(cols[col], 1); 
                vTaskDelay(pdMS_TO_TICKS(15)); // Espera 1 ms para encender el LED
                gpio_set_level(rows[row], 0); 
                gpio_set_level(cols[col], 0); 
            }else if(led_state[row][col] == 2 && led_blink_aux) {
                gpio_set_level(rows[row], 0);
                gpio_set_level(cols[col], 0);
                vTaskDelay(pdMS_TO_TICKS(15)); // Espera 1 ms para encender el LED
            }else if(led_state[row][col] == 2 && !led_blink_aux) {
                    gpio_set_level(rows[row], 1);
                    gpio_set_level(cols[col], 1);
                    vTaskDelay(pdMS_TO_TICKS(10)); // Espera 1 ms para encender el LED
                    gpio_set_level(rows[row], 0); 
                    gpio_set_level(cols[col], 0);
            }else{
                gpio_set_level(rows[row], 0); 
                gpio_set_level(cols[col], 0);
            }
        }
    }
    if(led_blink_iterations_aux % 5 == 0) {
        led_blink_aux = !led_blink_aux; // Cambia el estado de parpadeo
    }
}


void init_hw(){
    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;

    // Configura filas
    io_conf.pin_bit_mask = ROW_MASK;
    gpio_config(&io_conf);

    // Configura columnas
    io_conf.pin_bit_mask = COL_MASK;
    gpio_config(&io_conf);

    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0; // Habilita resistencias pull-up para los botones
    io_conf.pin_bit_mask = (1ULL << GPIO_UP) | (1ULL << GPIO_DOWN) | (1ULL << GPIO_LEFT) | (1ULL << GPIO_RIGHT);
    gpio_config(&io_conf);
}

void app_main() {
    init_hw();
    vTaskDelay(pdMS_TO_TICKS(200));
    init_tablero(); // Inicializa el tablero y las posiciones de los enemigos y el jugador
    xTaskCreatePinnedToCore(update_board_task_fn, "update_board_task", 4096, NULL, 1, NULL, 0);

    while(1)
    {
        led_blink_iterations_aux += 1;
        light_up_matrix_task_fn(); // Enciende la matriz de LEDs según el estado
    }
    

}