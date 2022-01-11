//
// Created by lining on 2021/2/28.
//

#ifndef _MYMATH_H
#define _MYMATH_H

#include <stdint.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
using namespace std;
namespace myMath {
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

    unsigned char XorCheck(unsigned char *in, unsigned int lenOfIn);

    uint16_t CRC16_1(uint8_t *pchMsg, uint16_t wDataLen);

    void base64_encode(unsigned char *input, unsigned int input_length, unsigned char *output);

    void base64_decode(unsigned char *input, unsigned int input_length, unsigned char *output);

    int GbkToUtf8TRANSLATE(char *in, unsigned int *lenOfIn, char *out, unsigned int *lenOfOut);

    int Utf8ToGbkTRANSLATE(char *in, unsigned int *lenOfIn, char *out, unsigned int *lenOfOut);

    void
    GetFormatStr(char *in, unsigned int *lenOfIn, char *out, unsigned int *lenOfOut, string formatIn, string formatOut);

    int GetUtf8VoiceFromInteger(unsigned int value, char *out, unsigned int *lenOfOut);

    int GetUtf8VoiceFromDecimal(double value, char *out, unsigned int *lenOfOut);

    int GetUtf8VoiceFromDouble(double value, char *out, unsigned int *lenOfOut);

    void str_replace(char *str1, char *str2, char *str3);

    bool IsLocalProcessRun(const char *name);

}


#ifdef __cplusplus
}
#endif

#endif //_MYMATH_H
