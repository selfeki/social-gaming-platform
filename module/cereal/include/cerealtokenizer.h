#include <string>
#include <vector>
#include "cereal.h"

class CerealTokenizer {
    public:
        CerealTokenizer(char*);
        void tokenize(char*);
        std::vector<std::string> tokens;

    private:
        char * delimiters;
};