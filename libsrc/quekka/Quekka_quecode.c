//
// Created by 우지범 on 2025. 12. 16..
//

#include "Quekka_quecode.h"

static _Thread_local Quecode quecode = QUECODE_SUCCESS;

static const char *quecode_value[] = {
    #define X(code, value, string) string,
    QUECODE_LIST
    #undef X
};

void Quecode_set_code(Quecode code) {
    quecode = code;
}

Quecode Quecode_get_code() {
    return quecode;
}

const char *Quecode_string_code(Quecode code) {
    if (0 > code || code >= QUECODE_LAST) {
        return "Unknown error";
    }
    return quecode_value[code];
}
