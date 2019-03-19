#ifndef __GENESIS_PARSER__
#define __GENESIS_PARSER__

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <fstream>
#include <libgenesis13/util/StringProcessing.h>

using namespace std;

class Parser : public StringProcessing {
    public:
        Parser();
        virtual ~Parser();
        bool open(string, int,bool);
        bool parse(string*, map<string, string>*);

    private:

        int rank;
        istringstream input;
        bool fillMap(string*, map<string, string>*);

};

#endif
