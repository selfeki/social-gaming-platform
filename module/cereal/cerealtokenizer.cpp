#include "cerealtokenizer.h"

CerealTokenizer::CerealTokenizer(char * delims) {
    delimiters = delims;
}

void CerealTokenizer::tokenize(char msg[]) {
    char * pch;
    pch = strtok(msg, delimiters);
    while (pch != NULL) {
        tokens.push_back(pch);
        pch = strtok(NULL, delimiters);
    }
}