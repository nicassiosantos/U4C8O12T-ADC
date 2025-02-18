#include <stdio.h> // Biblioteca padrão da linguagem C
#include "pico/stdlib.h" // Subconjunto central de bibliotecas do SDK Pico
#include "hardware/pwm.h" // Biblioteca para controlar o hardware de PWM
#include "hardware/adc.h" // Biblioteca para controle do ADC (Conversor Analógico-Digital)
#include "hardware/clocks.h" // Biblioteca para controle de clocks
#include "pico/bootrom.h" // Biblioteca para controle do bootrom
#include "hardware/i2c.h" // Biblioteca para controle do I2C
#include "inc/ssd1306.h" // Biblioteca para controle do display OLED SSD1306



// Definições dos pinos utilizados
#define VRY 26 // Pino da entrada analógica do eixo Y do joystick
#define VRX 27 // Pino da entrada analógica do eixo X do joystick
#define LED_B 12 // Pino do LED Azul
#define LED_R 13 // Pino do LED Vermelho
#define LED_G 11 // Pino do LED Verde
#define button_A 5 // Pino do botão A
#define button_joystick 22 // Pino do botão do joystick
#define I2C_PORT i2c1 // Porta I2C utilizada
#define I2C_SDA 14 // Pino SDA do I2C
#define I2C_SCL 15 // Pino SCL do I2C
#define endereco 0x3C // Endereço do display OLED


// Variáveis globais e flags
static volatile uint32_t last_time = 0; // Variável para controle do tempo (anti-bounce)
uint pwm_slice_1, pwm_slice_2; // Variáveis para manipular os slices do PWM
volatile bool pwm_on = true; // Flag para controle do PWM (ON/OFF)
volatile bool led_g_on = false; // Flag para controle do LED Verde
ssd1306_t ssd; // Estrutura do display OLED
uint pwm_wrap = 4096; // Valor máximo (wrap) do contador PWM
volatile int x, y; // Coordenadas do quadrado no display

// Função responsável por inicializar o PWM
uint pwm_init_gpio(uint gpio, uint wrap) {
    gpio_set_function(gpio, GPIO_FUNC_PWM); // Define o pino como saída PWM

    uint slice_num = pwm_gpio_to_slice_num(gpio); // Obtém o slice do PWM
    pwm_set_wrap(slice_num, wrap); // Define o valor de wrap para o slice
    
    pwm_set_enabled(slice_num, pwm_on);  // Ativa o PWM conforme a flag
    return slice_num;  
}

//Função para tratar a interrupção do botão A e do joystick
void handler(uint gpio, uint32_t events){
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    
    // Verifica se o botão A foi pressionado (com debounce)
    if (gpio == button_A && (current_time - last_time > 200000)) {
        last_time = current_time;
        if(pwm_on){ // Desativa o PWM e os LEDs Azul e Vermelho
            pwm_on = false;
            pwm_set_enabled(pwm_slice_1, false);
            pwm_set_enabled(pwm_slice_2, false);
            // Redefine os pinos como saída digital e apaga os LEDs
            gpio_set_function(LED_B, GPIO_FUNC_SIO);
            gpio_set_function(LED_R, GPIO_FUNC_SIO);
            gpio_set_dir(LED_B, GPIO_OUT);
            gpio_set_dir(LED_R, GPIO_OUT);
            gpio_put(LED_B, false);
            gpio_put(LED_R, false);
        }else{ // Ativa o PWM e os LEDs Azul e Vermelho
            pwm_on = true;
            pwm_slice_1 = pwm_init_gpio(LED_B, pwm_wrap);  
            pwm_slice_2 = pwm_init_gpio(LED_R, pwm_wrap);
            pwm_set_enabled(pwm_slice_1, true);
            pwm_set_enabled(pwm_slice_2, true);
        }
        
    }
    // Verifica se o botão do joystick foi pressionado
    else if (gpio == button_joystick && (current_time - last_time > 200000)) {
        last_time = current_time;
        if(led_g_on){ // Desativa o LED Verde e limpa o display
            led_g_on = false;
            gpio_put(LED_G,led_g_on);
            ssd1306_fill(&ssd, false);
            ssd1306_send_data(&ssd);
        }else{ // Ativa o LED Verde e desenha a borda na tela
            led_g_on = true;
            gpio_put(LED_G,led_g_on);
            ssd1306_rect(&ssd, 0, 0, 127, 63, true, false);
            ssd1306_send_data(&ssd);
        }
        
    }
}

// Função de configuração inicial
void initial_setup(){
    stdio_init_all(); // Inicializa a comunicação serial

    adc_init(); // Inicializa o ADC
    adc_gpio_init(VRY); // Configura pino do eixo Y do joystick
    adc_gpio_init(VRX); // Configura pino do eixo X do joystick

    // Configuração do botão A
    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN); 
    gpio_pull_up(button_A); 

    // Configuração do botão do joystick
    gpio_init(button_joystick);
    gpio_set_dir(button_joystick, GPIO_IN); 
    gpio_pull_up(button_joystick); 

    // Configuração do LED Verde
    gpio_init(LED_G);
    gpio_set_dir(LED_G, GPIO_OUT); 
    gpio_put(LED_G, led_g_on);

    // Configuração do I2C e do display OLED
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Função para desenhar o quadrado no display OLED
void draw_square(float duty_cycle_1, float duty_cycle_2){

    // Apaga o quadrado anterior
    ssd1306_delete_square(&ssd,  x,  y);
    ssd1306_send_data(&ssd); 
    
    // Determina as coordenadas Y com base no eixo Y do joystick
    if(duty_cycle_1 >= 0 && duty_cycle_1 < 16.6){
        y = 54; 
    }else if(duty_cycle_1 >= 16.6 && duty_cycle_1 < 33.2){
        y = 44;
    }else if(duty_cycle_1 >= 33.2 && duty_cycle_1 < 49.8){
        y = 34;
    }else if(duty_cycle_1 >= 49.8 && duty_cycle_1 < 66.4){
        y = 24;
    }else if(duty_cycle_1 >= 66.4  && duty_cycle_1 < 83){
        y = 14;
    }else if(duty_cycle_1 >= 83 && duty_cycle_1 < 100){
        y = 1;
    }

    // Determina as coordenadas X com base no eixo X do joystick
    if(duty_cycle_2 >= 0 && duty_cycle_2 < 10){
        x = 1; 
    }else if(duty_cycle_2 >= 10 && duty_cycle_2 < 20){
        x = 14;
    }else if(duty_cycle_2 >= 20 && duty_cycle_2 < 30){
        x = 27;
    }else if(duty_cycle_2 >= 30 && duty_cycle_2 < 40){
        x = 40;
    }else if(duty_cycle_2 >= 40 && duty_cycle_2 < 50){
        x = 53;
    }else if(duty_cycle_2 >= 50 && duty_cycle_2 < 60){
        x = 66;
    }else if(duty_cycle_2 >= 60 && duty_cycle_2 < 70){
        x = 79;
    }else if(duty_cycle_2 >= 70 && duty_cycle_2 < 80){
        x = 91;
    }else if(duty_cycle_2 >= 90 && duty_cycle_2 < 100){
        x = 118;
    }

    // Desenha o novo quadrado
    ssd1306_draw_square(&ssd, x, y);
    ssd1306_send_data(&ssd);
    
}

//Função responsável por obter os valores da convertidos digitalmente e controlar o duty-cycle
void setting_pwm_levels(float *duty_cycle_1,float *duty_cycle_2,uint16_t *vry_value, uint16_t *vrx_value){

    //Realizando conversão A/D
    adc_select_input(0);  
    *vry_value = adc_read();
    adc_select_input(1);  
    *vrx_value = adc_read(); 

    //Calculando porcentagem do duty-cycle em relação ao Wrap
    *duty_cycle_1 = (*vry_value / 4095.0) * 100;  
    *duty_cycle_2 = (*vrx_value / 4095.0) * 100;

    //Controle do duty cycle do pwm

    if(*duty_cycle_1 >= 46 &&*duty_cycle_1 <= 54){
        pwm_set_gpio_level(LED_B, 0);
    }else if(*vry_value > 2048){
        pwm_set_gpio_level(LED_B, *vry_value);
    }else{
        uint16_t level = (2048 - *vry_value) + 2048;
        pwm_set_gpio_level(LED_B, level);
    }
    
    if(*duty_cycle_2 >= 46 &&*duty_cycle_2 <= 54){
        pwm_set_gpio_level(LED_R, 0);
    }else if(*vrx_value > 2048){
        pwm_set_gpio_level(LED_R, *vrx_value);
    }else{
        uint16_t level = (2048 - *vrx_value) + 2048;
        pwm_set_gpio_level(LED_R, level);
    }

    //Função para desenhar o quadrado
    draw_square(*duty_cycle_1, *duty_cycle_2);


}

int main() {
    
    // Configuração inicial do sistema
    initial_setup();

    // Inicialização dos slices PWM para os LEDs Azul e Vermelho
    pwm_slice_1 = pwm_init_gpio(LED_B, pwm_wrap);  
    pwm_slice_2 = pwm_init_gpio(LED_R, pwm_wrap); 

    float duty_cycle_1, duty_cycle_2; // Variáveis para armazenar os ciclos de trabalho (duty cycle) do PWM
    uint16_t vry_value, vrx_value; // Variáveis para armazenar os valores analógicos dos eixos do joystick
    
    // Configuração das interrupções para o botão A e o botão do joystick
    gpio_set_irq_enabled_with_callback(button_A, GPIO_IRQ_EDGE_FALL, true, &handler);
    gpio_set_irq_enabled_with_callback(button_joystick, GPIO_IRQ_EDGE_FALL, true, &handler);

    uint32_t last_print_time = 0; // Variável para controle do tempo de impressão no console

    while (true) {
        // Configura os níveis do PWM com base nas leituras do joystick
        setting_pwm_levels(&duty_cycle_1,&duty_cycle_2, &vry_value, &vrx_value);
        
        uint32_t current_time = to_ms_since_boot(get_absolute_time());  

        last_print_time = current_time;  
        

        sleep_ms(100);  
    }

    return 0;  
}
