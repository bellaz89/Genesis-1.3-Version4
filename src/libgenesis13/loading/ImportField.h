#ifndef __GENESIS_IMPORTFIELD__
#define __GENESIS_IMPORTFIELD__

#include <vector>
#include <string>
#include <map>
#include <libgenesis13/util/StringProcessing.h>
#include <libgenesis13/main/Setup.h>
#include <libgenesis13/main/Time.h>
#include <libgenesis13/core/Field.h>

using namespace std;

extern const double ce;

class ImportField : public StringProcessing {
public:
    ImportField();
    virtual ~ImportField();
    bool init(int, int, map<string, string>*, vector<Field*>*, Setup*, Time*);

private:
    void usage();
    string file;
    int harm;
    double offset;
    bool dotime;
};

#endif
