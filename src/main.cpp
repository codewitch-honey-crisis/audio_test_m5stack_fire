#include <Arduino.h>
#include <driver/i2s.h>
#include <player.hpp>
#include <test.hpp>
// pin assignments
constexpr static const uint8_t spi_host = VSPI;
constexpr static const int8_t lcd_pin_bl = 32;
constexpr static const int8_t lcd_pin_dc = 27;
constexpr static const int8_t lcd_pin_rst = 33;
constexpr static const int8_t lcd_pin_cs = 14;
constexpr static const int8_t sd_pin_cs = 4;
constexpr static const int8_t speaker_pin = 25;
constexpr static const int8_t mic_pin = 34;
constexpr static const int8_t button_a_pin = 39;
constexpr static const int8_t button_b_pin = 38;
constexpr static const int8_t button_c_pin = 37;
constexpr static const int8_t led_pin = 15;
constexpr static const int8_t spi_pin_mosi = 23;
constexpr static const int8_t spi_pin_clk = 18;
constexpr static const int8_t spi_pin_miso = 19;
player sound(44100,1,8,512);

size_t test_len = sizeof(test_data);
size_t test_pos = 0;
int read_demo(void* state) {
    if(test_pos>=test_len) {
        return -1;
    }
    return test_data[test_pos++];
}
void seek_demo(unsigned long long position, void* state) {
    test_pos = position;
}
void setup() {
    Serial.begin(115200);    
    if(!sound.initialize()) {
        printf("Sound initialization failure.\n");    
    }
    i2s_config_t i2s_config;
    memset(&i2s_config,0,sizeof(i2s_config_t));
    i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN);
    i2s_config.sample_rate = 22050;
    i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_8BIT;
    i2s_config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
    i2s_config.communication_format = I2S_COMM_FORMAT_STAND_MSB;
    i2s_config.dma_buf_count = 2;
    i2s_config.dma_buf_len = player::frame_size(sound.channel_count(),sound.bit_depth())*player.frame_count();
    i2s_config.use_apll = true;
    i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL2;
    i2s_driver_install((i2s_port_t)I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin((i2s_port_t)0,nullptr);
    
    sound.on_flush([](const void* buffer,size_t buffer_size,void* state){
        size_t written;
        i2s_write(I2S_NUM_0,buffer,buffer_size,&written,portMAX_DELAY);
    });
    sound.on_sound_disable([](void* state) {
        i2s_zero_dma_buffer(I2S_NUM_0);
    });
    sound.wav(read_demo,nullptr,.5,true,seek_demo,nullptr);
    //sound.sin(440,.5);
}
void loop() {
    sound.update();
}