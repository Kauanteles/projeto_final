#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "hardware/timer.h"

// Declaração de constantes
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define NUM_PIXELS 25 // Número de LEDs na matriz
#define WS2812_PIN 7
#define IS_RGBW 0
#define LED_PIN_BLUE 12
#define LED_PIN_GREEN 11
#define PIN_BUTTON_A 5
#define PIN_BUTTON_B 6
#define MAX_PASSWORD_LENGTH 6 // Tamanho máximo da senha

// Declaração de variáveis
static volatile uint32_t LAST_TIME_A = 0;
static volatile uint32_t LAST_TIME_B = 0;
static volatile bool button_a_pressed = false;
static volatile bool button_b_pressed = false;
int current_number = 0;
int count_a = 0;
int count_b = 0;
char password[MAX_PASSWORD_LENGTH + 1] = ""; // Armazena a senha
bool password_set = false; // Indica se a senha foi definida

// Declaração das funções
static inline void put_pixel(uint32_t pixel_grb);
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
void gpio_irq_handler(uint gpio, uint32_t events);
void initialize_pin_led();
void display_menu(ssd1306_t *ssd);
void create_password(ssd1306_t *ssd);
void enter_password(ssd1306_t *ssd);
void blink_led(bool success);
void read_password_from_terminal(char *buffer, int max_length);
void clear_input_buffer();

// Padrões de LEDs para "V" e "X"
const bool V_PATTERN[NUM_PIXELS] = {
    0, 0, 0, 1, 0,
    1, 0, 1, 0, 0,
    0, 1, 0, 0, 0,
    0, 0, 0, 0, 1,
    0, 0, 0, 0, 0
};

const bool X_PATTERN[NUM_PIXELS] = {
    1, 0, 0, 0, 1,
    0, 1, 0, 1, 0,
    0, 0, 1, 0, 0,
    0, 1, 0, 1, 0,
    1, 0, 0, 0, 1
};

int main() {
    // Inicialização da matriz de leds, display e led RGB
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);
    stdio_init_all();
    i2c_init(I2C_PORT, 400 * 1000);
    initialize_pin_led();
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Apaga todo o display
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configuração dos botões A e B
    gpio_init(PIN_BUTTON_A);
    gpio_set_dir(PIN_BUTTON_A, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_A);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Interrupção para o botão A

    gpio_init(PIN_BUTTON_B);
    gpio_set_dir(PIN_BUTTON_B, GPIO_IN);
    gpio_pull_up(PIN_BUTTON_B);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler); // Interrupção para o botão B

    while (true) {
        display_menu(&ssd);

        if (button_a_pressed) {
            button_a_pressed = false;
            create_password(&ssd);
        } else if (button_b_pressed) {
            button_b_pressed = false;
            enter_password(&ssd);
        }

        sleep_ms(10);
    }

    return 0;
}

// Função para inicializar os pinos dos leds verde e azul
void initialize_pin_led() {
    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);

    gpio_init(LED_PIN_BLUE);
    gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);
}

// Funções para configurar os leds da matriz de leds
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    // Reduz a intensidade para 10% (valores de 0 a 255 são escalados para 0 a 25)
    return ((uint32_t)(r / 10) << 8) | ((uint32_t)(g / 10) << 16) | (uint32_t)(b / 10);
}

// Função para trabalhar a interrupção dos botões A e B
void gpio_irq_handler(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (gpio == PIN_BUTTON_A && current_time - LAST_TIME_A > 200000) {
        LAST_TIME_A = current_time;
        button_a_pressed = true;
        count_a++;
    } else if(gpio == PIN_BUTTON_B && current_time - LAST_TIME_B > 200000) {
        LAST_TIME_B = current_time;
        button_b_pressed = true;
        count_b++;
    }
}

// Função para exibir o menu no display
void display_menu(ssd1306_t *ssd) {
    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, "1 criar senha", 10, 10);
    ssd1306_draw_string(ssd, "2 entrar", 10, 20);
    ssd1306_send_data(ssd);
}

// Função para criar uma nova senha
void create_password(ssd1306_t *ssd) {
    if (password_set) {
        // Se já existir uma senha, pede a senha antiga
        ssd1306_fill(ssd, false);
        ssd1306_draw_string(ssd, "Digite a senha", 10, 10);
        ssd1306_draw_string(ssd, "antiga", 10, 20);
        ssd1306_send_data(ssd);

        char old_password[MAX_PASSWORD_LENGTH + 1] = {0}; // Buffer para a senha antiga
        clear_input_buffer(); // Limpa o buffer de entrada
        read_password_from_terminal(old_password, MAX_PASSWORD_LENGTH);

        if (strcmp(old_password, password) != 0) {
            // Senha antiga incorreta
            ssd1306_fill(ssd, false);
            ssd1306_draw_string(ssd, "Senha antiga", 10, 10);
            ssd1306_draw_string(ssd, "incorreta", 10, 20);
            ssd1306_send_data(ssd);
            blink_led(false); // Exibe "X" vermelho na matriz de LEDs
            sleep_ms(2000); // Mostra a mensagem por 2 segundos
            return; // Volta ao menu principal
        }
    }

    // Permite a criação de uma nova senha
    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, "Digite nova", 10, 10);
    ssd1306_draw_string(ssd, "senha ", 10, 20);
    ssd1306_send_data(ssd);

    char new_password[MAX_PASSWORD_LENGTH + 1] = {0}; // Buffer para a nova senha
    clear_input_buffer(); // Limpa o buffer de entrada
    read_password_from_terminal(new_password, MAX_PASSWORD_LENGTH);

    snprintf(password, sizeof(password), "%s", new_password); // Armazena a nova senha
    password_set = true;

    blink_led(true); // Exibe "V" na matriz de LEDs (confirmação de sucesso)
}

// Função para inserir a senha
void enter_password(ssd1306_t *ssd) {
    if (!password_set) {
        ssd1306_fill(ssd, false);
        ssd1306_draw_string(ssd, "Nenhuma senha", 10, 10);
        ssd1306_draw_string(ssd, "existente", 10, 20);
        ssd1306_send_data(ssd);
        sleep_ms(2000);
        return;
    }

    ssd1306_fill(ssd, false);
    ssd1306_draw_string(ssd, "Digite a senha", 10, 10);
    ssd1306_send_data(ssd);

    char input[MAX_PASSWORD_LENGTH + 1] = {0}; // Limpa o buffer
    clear_input_buffer(); // Limpa o buffer de entrada antes de ler a senha
    read_password_from_terminal(input, MAX_PASSWORD_LENGTH);

    printf("Senha digitada: %s\n", input); // Log para depuração
    printf("Senha armazenada: %s\n", password); // Log para depuração

    if (strcmp(input, password) == 0) {
        blink_led(true); // Exibe "V" na matriz de LEDs
    } else {
        ssd1306_fill(ssd, false);
        ssd1306_draw_string(ssd, "Senha incorreta", 10, 10);
        ssd1306_send_data(ssd);
        blink_led(false); // Exibe "X" na matriz de LEDs
    }
    sleep_ms(2000); // Mostra o padrão por 2 segundos
}

// Função para exibir "V" ou "X" na matriz de LEDs
void blink_led(bool success) {
    const bool *pattern = success ? V_PATTERN : X_PATTERN; // Escolhe o padrão
    uint32_t color = success ? urgb_u32(0, 255, 0) : urgb_u32(255, 0, 0); // Verde para "V", vermelho para "X"

    for (int i = 0; i < NUM_PIXELS; i++) {
        if (pattern[i]) {
            put_pixel(color); // Acende o LED com a cor correspondente
        } else {
            put_pixel(0); // Apaga o LED
        }
    }
    sleep_ms(2000); // Mantém os LEDs acesos por 2 segundos
    for (int i = 0; i < NUM_PIXELS; i++) {
        put_pixel(0); // Apaga todos os LEDs após 2 segundos
    }
}

// Função para ler a senha do terminal com feedback visual de *
void read_password_from_terminal(char *buffer, int max_length) {
    memset(buffer, 0, max_length + 1); // Limpa o buffer antes de cada leitura
    int i = 0;
    printf("Digite a senha: ");
    while (i < max_length) {
        int c = getchar_timeout_us(1000);
        if (c != PICO_ERROR_TIMEOUT) {
            if (c == '\n' || c == '\r') { // Finaliza a entrada ao pressionar Enter
                break;
            }
            buffer[i] = (char)c;
            i++;
            printf("*"); // Exibe um asterisco para cada caractere digitado
            fflush(stdout); // Garante que o * seja exibido imediatamente
        }
    }
    printf("\n"); // Nova linha após a senha ser digitada
    buffer[i] = '\0'; // Termina a string
}

// Função para limpar o buffer de entrada
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Limpa todos os caracteres no buffer
}