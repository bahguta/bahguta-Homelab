#include "DateUtils.h"

static const char* weekdays[] =
{
    "Domingo",
    "Lunes",
    "Martes",
    "Miércoles",
    "Jueves",
    "Viernes",
    "Sábado"
};

static const char* months[] =
{
    "",
    "ENE",
    "FEB",
    "MAR",
    "ABR",
    "MAY",
    "JUN",
    "JUL",
    "AGO",
    "SEP",
    "OCT",
    "NOV",
    "DIC"
};

const char* DateUtils::weekday(uint8_t day)
{
    return weekdays[day];
}

uint8_t DateUtils::weekdayFromDate(int year, int month, int day)
{
    // Algoritmo de Sakamoto. Devuelve 0=Domingo ... 6=Sabado,
    // igual que el indice que espera weekday().

    static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

    if (month < 3)
    {
        year -= 1;
    }

    int result =
        (year + year / 4 - year / 100 + year / 400 +
         t[month - 1] + day) % 7;

    return static_cast<uint8_t>(result);
}

const char* DateUtils::month(uint8_t month)
{
    return months[month];
}

void DateUtils::formatTime(
    const ClockInfo& clock,
    char* buffer)
{
    sprintf(
        buffer,
        "%02d:%02d",
        clock.hour,
        clock.minute);
}

void DateUtils::formatDate(
    const ClockInfo& clock,
    char* buffer)
{
    sprintf(
        buffer,
        "%02d %s",
        clock.day,
        month(clock.month));
}