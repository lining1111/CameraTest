//
// Created by lining on 1/13/22.
//

#include <iostream>
#include "RTSPH264.h"

using namespace std;

int main(int argc, char **argv) {

    RTSPH264 *rtsph264 = new RTSPH264();

    rtsph264->Open();
    rtsph264->Run();
//    rtsph264->Close();
//    delete rtsph264;
    while (true) {
        sleep(1);
    }
}