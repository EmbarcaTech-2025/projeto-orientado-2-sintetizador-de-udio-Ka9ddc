    #include <stdio.h> // Biblioteca padrão
    #include <math.h> // Biblioteca de matemática (função "round" foi utilizada)

    #include "pico/stdlib.h" // Biblioteca padrão pico
    #include "hardware/gpio.h" // Biblioteca de GPIOs
    #include "hardware/adc.h" // Biblioteca do ADC
    #include "hardware/pwm.h" // Biblioteca do PWM
    #include "include/ssd1306/ssd1306.h" // Biblioteca do display OLED

    #define SAMPLING_RATE 8000      // 8 KHz
    #define RECORD_SECONDS 5        // Duração da gravação em segundos
    #define BUFFER_SIZE (SAMPLING_RATE * RECORD_SECONDS) // Tamanho do buffer da gravação

    #define MIC_PIN 28
    #define ADC_CHANNEL 2
    #define BUZZER_1_PIN 10
    #define BUZZER_2_PIN 21
    #define BUTTON_A_PIN 5
    #define BUTTON_B_PIN 6
    #define LED_RED_PIN 13
    #define LED_GREEN_PIN 11

    uint8_t audio_buffer[BUFFER_SIZE]; // 
    volatile uint32_t buffer_position = 0;
    volatile bool is_recording = false;
    volatile bool is_playing = false;
    volatile bool buffer_is_full = false;


    // Configuração display OLED
    ssd1306_t disp;

    void setup_display()
    {
        i2c_init(i2c1, 400000);
        gpio_set_function(14, GPIO_FUNC_I2C);
        gpio_set_function(15, GPIO_FUNC_I2C);
        gpio_pull_up(14);
        gpio_pull_up(15);

        disp.external_vcc = false;
        ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
        ssd1306_clear(&disp);
        ssd1306_draw_string(&disp, 0, 0, 1, "Sintetizador de Audio");
        ssd1306_draw_string(&disp, 0, 16, 1, "BitDogLab");
        ssd1306_show(&disp);
        sleep_ms(2000);
        ssd1306_clear(&disp);
        ssd1306_show(&disp);
    }

    struct repeating_timer timer;

    // Função chamada pelo timer para gravar uma amostra
    bool record_sample(struct repeating_timer *t) {
        if (buffer_position < BUFFER_SIZE) {
            // Lê o valor do ADC (12 bits) e o converte para 8 bits (0-255)
            uint16_t adc_value = adc_read();
            audio_buffer[buffer_position] = (uint8_t)round((float)adc_value / 16.0f);
            buffer_position++;
        } else {
            // Gravação concluída
            is_recording = false;
            buffer_is_full = true;
            gpio_put(LED_RED_PIN, 0); // Desliga LED vermelho 

            ssd1306_clear(&disp);
            ssd1306_show(&disp);

            cancel_repeating_timer(&timer);
        }
        return true;
    }

    void start_recording() {
        if (is_recording || is_playing) return; // Evita iniciar se já estiver ocupado

        ssd1306_clear(&disp);
        ssd1306_draw_string(&disp, 24, 28, 1, "Gravando...");
        ssd1306_show(&disp);

        printf("Iniciando gravacao...\n");
        is_recording = true;
        buffer_is_full = false;
        buffer_position = 0;
        
        gpio_put(LED_RED_PIN, 1); // Acende LED vermelho para indicar gravação 

        // Configura o timer para chamar record_sample na taxa de amostragem definida
        add_repeating_timer_us(-(1000000 / SAMPLING_RATE), record_sample, NULL, &timer);
    }

    void play_audio() {
        if (is_recording || is_playing || !buffer_is_full) return;

        int delay_samples = 20;

        printf("Verificando dados do buffer antes de reproduzir...\n");
        // Imprime as primeiras 200 amostras para verificação
        for (int i = 0; i < 200; i++) {
            printf("%d ", audio_buffer[i]);
        }
        printf("\n\nReproduzindo audio...\n");

        is_playing = true;
        gpio_put(LED_GREEN_PIN, 1);

        gpio_set_function(BUZZER_1_PIN, GPIO_FUNC_PWM);
        uint slice1_num = pwm_gpio_to_slice_num(BUZZER_1_PIN);
        uint chan1_num = pwm_gpio_to_channel(BUZZER_1_PIN);
        pwm_set_wrap(slice1_num, 255);
        pwm_set_enabled(slice1_num, true);

        gpio_set_function(BUZZER_2_PIN, GPIO_FUNC_PWM);
        uint slice2_num = pwm_gpio_to_slice_num(BUZZER_2_PIN);
        uint chan2_num = pwm_gpio_to_channel(BUZZER_2_PIN);
        pwm_set_wrap(slice2_num, 255);
        pwm_set_enabled(slice2_num, true);

        // Loop de Reprodução
        for (int i = 0; i < BUFFER_SIZE; i++) {
            uint8_t sample = audio_buffer[i];

            pwm_set_chan_level(slice1_num, chan1_num, audio_buffer[i]);

            if (i >= delay_samples) {
                pwm_set_chan_level(slice2_num, chan2_num, audio_buffer[i - delay_samples]);
            }

            sleep_us(1000000 / SAMPLING_RATE);
        }

        pwm_set_enabled(slice1_num, false);
        pwm_set_enabled(slice2_num, false);

        gpio_put(LED_GREEN_PIN, 0);
        is_playing = false;
        printf("Reproducao finalizada.\n");
    }

    int main() {
        stdio_init_all();
        setup_display();
        printf("Sintetizador de Audio BitDogLab - Pressione A para gravar, B para reproduzir.\n");

        // ADC para o microfone
        adc_init();
        adc_gpio_init(MIC_PIN);
        adc_select_input(ADC_CHANNEL);

        // LEDs para feedback visual 
        gpio_init(LED_RED_PIN);
        gpio_set_dir(LED_RED_PIN, GPIO_OUT);
        gpio_init(LED_GREEN_PIN);
        gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

        // Botões para controle 
        gpio_init(BUTTON_A_PIN);
        gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
        gpio_pull_up(BUTTON_A_PIN);

        gpio_init(BUTTON_B_PIN);
        gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
        gpio_pull_up(BUTTON_B_PIN);

        // Loop principal
        while (1) {
            // Verifica se o Botão A foi pressionado, configurado para pull-up
            if (!gpio_get(BUTTON_A_PIN)) {
                start_recording();
                // Debounce
                while(!gpio_get(BUTTON_A_PIN)); // Espera soltar o botão
                sleep_ms(50);
            }

            // Verifica se o Botão B foi pressionado
            if (!gpio_get(BUTTON_B_PIN)) {
                play_audio();
                while(!gpio_get(BUTTON_B_PIN)); // Espera soltar o botão
                sleep_ms(50);
            }
        }
    }