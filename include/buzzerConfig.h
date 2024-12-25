#ifndef BUZZER_CONFIG_H
#define BUZZER_CONFIG_H

class Buzzer
{
public:
    // Constructor untuk inisialisasi pin
    Buzzer(int pin)
    {
        _pin = pin;
        pinMode(_pin, OUTPUT); // Atur pin sebagai output
    }

    // Fungsi untuk membunyikan buzzer
    void beep(int times, int duration)
    {
        for (int i = 0; i < times; i++)
        {
            digitalWrite(_pin, LOW);  // Menyalakan buzzer
            delay(duration);          // Tunggu selama durasi yang ditentukan
            digitalWrite(_pin, HIGH); // Mematikan buzzer
            delay(duration);          // Tunggu sebelum bunyi berikutnya
        }
    }

private:
    int _pin; // Pin tempat buzzer terhubung
};

#endif
