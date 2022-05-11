void calibrate();

bool isIdle(int);
void handleIdle();

int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue);
bool readSwitch(byte channelInput, bool defaultValue);