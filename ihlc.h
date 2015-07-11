
#define NUM_CHANNELS     16
#define FADE_DELAY_US    3500


// Values should be between 0 and 100
void setChannel(int channel, uint8_t value);
uint8_t getChannel(int channel);
void setAllChannels(uint8_t value);
