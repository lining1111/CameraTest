#include <iostream>
#include "OpenCVTest.h"
#include "UvcTest.h"
#include "Uvc2Fb.h"
#include "MyVideoCapture.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

//    OpenCVTest();
//    UvcTest();
    Uvc2Fb();
    return 0;
}
