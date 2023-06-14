#include <Wire.h>

#define SLAVE_ADDRESS 8
#define DATA_SIZE 99

#define BUZZER_PIN 2

#define GREEN_LED_PIN 3
#define RED_LED_PIN 4
#define YELLOW_LED_PIN 5
#define BLUE_LED_PIN 6
#define ORANGE_LED_PIN 7

#define GREEN_BUTTON_PIN 8
#define RED_BUTTON_PIN 9
#define YELLOW_BUTTON_PIN 10
#define BLUE_BUTTON_PIN 11
#define ORANGE_BUTTON_PIN 12

#define FRECUENCY_DO 262
#define FRECUENCY_RE 293
#define FRECUENCY_MI 330
#define FRECUENCY_FA 349
#define FRECUENCY_SOL 392

int frequencies[5] = {FRECUENCY_DO, FRECUENCY_RE, FRECUENCY_MI, FRECUENCY_FA, FRECUENCY_SOL};
int inputButton[5] = {GREEN_BUTTON_PIN, RED_BUTTON_PIN, YELLOW_BUTTON_PIN, BLUE_BUTTON_PIN, ORANGE_BUTTON_PIN};
int leds[5] = {GREEN_LED_PIN, RED_LED_PIN, YELLOW_LED_PIN, BLUE_LED_PIN, ORANGE_LED_PIN};
bool areButtonsBlocked = true;

void setup()
{
    Serial.begin(9600);
    Wire.begin(8);
    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestFunc);
    pinMode(BUZZER_PIN, OUTPUT);
    for (int pin = 0; pin < 5; pin++)
    {
        pinMode(leds[pin], OUTPUT);
    }
    for (int pin = 0; pin < 5; pin++)
    {
        pinMode(inputButton[pin], INPUT);
    }
}

void loop()
{
}

void turnOffLeds()
{
    for (int pin = 0; pin < 5; pin++)
    {
        digitalWrite(leds[pin], LOW);
    }
}

void receiveEvent(int byteCount)
{
    int current = 0;
    while (Wire.available())
    {
        int current = Wire.read();
        Serial.println("current: ");

        Serial.println(current);
        if (current == 9)
        {
            turnOffLeds();
            noTone(BUZZER_PIN);
        }
        else if (current >= 0 && current <= 4)
        {
            digitalWrite(leds[current], HIGH);
            tone(BUZZER_PIN, frequencies[current]);
        }
    }
}

void requestFunc()
{
    bool flag = false;
    while (true)
    {
        for (int pin = 0; pin < 5; pin++)
        {
            if (digitalRead(inputButton[pin]))
            {
                digitalWrite(leds[pin], HIGH);
                tone(BUZZER_PIN, frequencies[pin]);
                Wire.write(pin);
                return;
            }
        }
        Wire.write(9);
        return;
    }
}