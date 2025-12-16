//
// Created by 우지범 on 2025. 12. 16..
//

#ifndef QUEKKA_QUEKKA_ERROR_H
#define QUEKKA_QUEKKA_ERROR_H

#include <stdio.h>

#define QUECODE_LIST \
    X(QUECODE_SUCCESS,          0000, "Sucess") \
    X(QUECODE_FAILURE,          0001, "Failure") \
    X(QUECODE_INVALID_INPUT,    0002, "Invalid Input")


typedef enum {
    #define X(code, value, string) code = value,
    QUECODE_LIST
    #undef X
    QUECODE_LAST
} Quecode;

void Quecode_set_code(Quecode code);

Quecode Quecode_get_code();

const char *Quecode_string_code(Quecode code);

#endif //QUEKKA_QUEKKA_ERROR_H