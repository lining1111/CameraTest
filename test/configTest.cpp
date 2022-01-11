//
// Created by lining on 1/5/22.
//

#include <string>
#include "config.h"

using namespace std;


int main() {
    string s = "15.45";
    double value = stod(s/*, nullptr*/);
    printf("value:%lf\n", value);
    double value1 = 0.45f;
    auto b = value1*2;
//    string b_s = to_string(b);
    char b_s[100];
    sprintf(b_s,"%f",b);
    printf("b:%lf\n", b);

    string dbPath = "./config/config.db";
    read_real_loc_info((char *) dbPath.c_str());
}