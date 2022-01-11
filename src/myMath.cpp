//
// Created by root on 2021/2/28.
//

#include <stdio.h>
#include <iconv.h>
#include <string.h>

#include "myMath.h"

namespace myMath {
    unsigned char XorCheck(unsigned char *in, unsigned int lenOfIn) {
        unsigned char ret;
        int i = 0;
        ret = in[0];
        for (i = 1; i < lenOfIn; i++) {
            ret = (ret ^ in[i]);
        }
        return ret;
    }

//CRC 高位字节值表
    const uint8_t chCRCHTable[] =
            {0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
             0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
             0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
             0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
             0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
             0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
             0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
             0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
             0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
             0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
             0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
             0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
             0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
             0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
             0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
             0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40};
    const uint8_t chCRCLTable[] =                                 // CRC 低位字节值表
            {0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
             0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
             0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
             0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
             0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
             0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
             0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
             0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
             0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
             0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
             0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
             0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
             0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
             0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
             0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
             0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40};

//CRC16计算方法
    uint16_t CRC16_1(uint8_t *pchMsg, uint16_t wDataLen) {
        uint8_t chCRCHi = 0xFF; // 高CRC字节
        uint8_t chCRCLo = 0xFF; // 低
        uint16_t wIndex;            // CRC循环中的索引
        while (wDataLen--) {
            // 计算CRC
            wIndex = (chCRCLo ^ *pchMsg++) & 0xff;
            chCRCLo = chCRCHi ^ chCRCHTable[wIndex];
            chCRCHi = chCRCLTable[wIndex];
        }

        return ((chCRCHi << 8) | chCRCLo) & 0xffff;
    }

    const char encodeCharacterTable[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const signed char decodeCharacterTable[256] = {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
            -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
            -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    };

    void base64_encode(unsigned char *input, unsigned int input_length, unsigned char *output) {
        char buff1[3];
        char buff2[4];
        unsigned char i = 0, j;
        unsigned int input_cnt = 0;
        unsigned int output_cnt = 0;

        while (input_cnt < input_length) {
            buff1[i++] = input[input_cnt++];
            if (i == 3) {
                output[output_cnt++] = encodeCharacterTable[(buff1[0] & 0xfc) >> 2];
                output[output_cnt++] = encodeCharacterTable[((buff1[0] & 0x03) << 4) + ((buff1[1] & 0xf0) >> 4)];
                output[output_cnt++] = encodeCharacterTable[((buff1[1] & 0x0f) << 2) + ((buff1[2] & 0xc0) >> 6)];
                output[output_cnt++] = encodeCharacterTable[buff1[2] & 0x3f];
                i = 0;
            }
        }
        if (i) {
            for (j = i; j < 3; j++) {
                buff1[j] = '\0';
            }
            buff2[0] = (buff1[0] & 0xfc) >> 2;
            buff2[1] = ((buff1[0] & 0x03) << 4) + ((buff1[1] & 0xf0) >> 4);
            buff2[2] = ((buff1[1] & 0x0f) << 2) + ((buff1[2] & 0xc0) >> 6);
            buff2[3] = buff1[2] & 0x3f;
            for (j = 0; j < (i + 1); j++) {
                output[output_cnt++] = encodeCharacterTable[buff2[j]];
            }
            while (i++ < 3) {
                output[output_cnt++] = '=';
            }
        }
        output[output_cnt] = 0;
    }

    void base64_decode(unsigned char *input, unsigned int input_length, unsigned char *output) {
        char buff1[4];
        char buff2[4];
        unsigned char i = 0, j;
        unsigned int input_cnt = 0;
        unsigned int output_cnt = 0;

        while (input_cnt < input_length) {
            buff2[i] = input[input_cnt++];
            if (buff2[i] == '=') {
                break;
            }
            if (++i == 4) {
                for (i = 0; i != 4; i++) {
                    buff2[i] = decodeCharacterTable[buff2[i]];
                }
                output[output_cnt++] = (char) ((buff2[0] << 2) + ((buff2[1] & 0x30) >> 4));
                output[output_cnt++] = (char) (((buff2[1] & 0xf) << 4) + ((buff2[2] & 0x3c) >> 2));
                output[output_cnt++] = (char) (((buff2[2] & 0x3) << 6) + buff2[3]);
                i = 0;
            }
        }
        if (i) {
            for (j = i; j < 4; j++) {
                buff2[j] = '\0';
            }
            for (j = 0; j < 4; j++) {
                buff2[j] = decodeCharacterTable[buff2[j]];
            }
            buff1[0] = (buff2[0] << 2) + ((buff2[1] & 0x30) >> 4);
            buff1[1] = ((buff2[1] & 0xf) << 4) + ((buff2[2] & 0x3c) >> 2);
            buff1[2] = ((buff2[2] & 0x3) << 6) + buff2[3];
            for (j = 0; j < (i - 1); j++) {
                output[output_cnt++] = (char) buff1[j];
            }
        }
        output[output_cnt] = 0;
    }

    int GbkToUtf8TRANSLATE(char *in, unsigned int *lenOfIn, char *out, unsigned int *lenOfOut) {
        //
        /* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
         *           IGNORE ：遇到无法转换字符跳过*/
        int bRes = -1;
        const char *const encTo = "UTF-8//TRANSLIT"; // TRANSLIT //IGNORE
        const char *const encFrom = "GBK"; //UNICODE GBK
        iconv_t cd = iconv_open(encTo, encFrom);
        if (cd == (iconv_t) -1) {
            perror("iconv_open");
            iconv_close(cd);
            return -1;
        }
        /* 由于iconv()函数会修改指针，所以要保存源指针 */

        /* 需要转换的字符串 */
        char bufIn[1024];
        size_t lenOfBufIn;
        char *pBufIn = bufIn;
        memset(bufIn, 0, sizeof(bufIn));
        snprintf(bufIn, sizeof(bufIn) - 1, "%s", in);
        lenOfBufIn = *lenOfIn;

        /* 存放转换后的字符串 */
        char bufOut[1024];
        size_t lenOfBufOut = sizeof(bufOut);
        char *pBufOut = bufOut;
        memset(bufOut, 0, sizeof(bufOut));

        /* 进行转换
         * *@param cd iconv_open()产生的句柄
         * *@param bufIn 需要转换的字符串
         * *@param lenOfBufIn存放还有多少字符没有转换
         * *@param bufOut 存放转换后的字符串
         *	*@param outlen 存放转换后,tempoutbuf剩余的空间
         *  */
        size_t ret = iconv(cd, &pBufIn, &lenOfBufIn, &pBufOut, &lenOfBufOut);
        if (ret == -1) {
            perror("iconv");
            iconv_close(cd);
            return -1;
        }
        memcpy(out, bufOut, strlen(bufOut));
        *lenOfOut = strlen(bufOut);
        /* 关闭句柄 */
        //
        iconv_close(cd);
        return bRes;

    }

    int Utf8ToGbkTRANSLATE(char *in, unsigned int *lenOfIn, char *out, unsigned int *lenOfOut) {
        /*
         *  目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
         *      	IGNORE  ：遇到无法转换字符跳过
         */
        char *encTo = "GBK//TRANSLIT";
        /* 源编码 */
        char *encFrom = "UTF-8";

        /*
         * 获得转换句柄
         * *@param encTo 目标编码方式
         * *@param encFrom 源编码方式
         */
        iconv_t cd = iconv_open(encTo, encFrom);
        if (cd == (iconv_t) -1) {
            perror("iconv_open");
            iconv_close(cd);
            return -1;
        }

        /* 需要转换的字符串 */
        char bufIn[1024];
        size_t lenOfBufIn;
        char *pBufIn = bufIn;
        memset(bufIn, 0, sizeof(bufIn));
        snprintf(bufIn, sizeof(bufIn) - 1, "%s", in);
        lenOfBufIn = *lenOfIn;

        /* 存放转换后的字符串 */
        char bufOut[1024];
        size_t lenOfBufOut = sizeof(bufOut);
        char *pBufOut = bufOut;
        memset(bufOut, 0, sizeof(bufOut));
        /* 由于iconv()函数会修改指针，所以要保存源指针 */

        /* 进行转换
         * *@param cd iconv_open()产生的句柄
         * *@param bufIn 需要转换的字符串
         * *@param lenOfBufIn存放还有多少字符没有转换
         * *@param bufOut 存放转换后的字符串
         *	*@param outlen 存放转换后,tempoutbuf剩余的空间
         *  */
        size_t ret = iconv(cd, &pBufIn, &lenOfBufIn, &pBufOut, &lenOfBufOut);
        if (ret == -1) {
            perror("iconv");
            iconv_close(cd);
            return -1;
        }

        memcpy(out, bufOut, strlen(bufOut));
        *lenOfOut = strlen(bufOut);
        /* 关闭句柄 */
        iconv_close(cd);
        return 0;
    }

    void GetFormatStr(char *in, unsigned int *lenOfIn, char *out, unsigned int *lenOfOut, string formatIn,
                      string formatOut) {
        if (in == NULL || out == NULL || *lenOfIn == 0 || formatIn.empty() || formatOut.empty()) {
            return;
        }

        if (formatIn == formatOut) {
            //copy
            strcpy(out, in);
            *lenOfOut = *lenOfIn;
        } else if (formatIn.compare("UTF8") == 0 && formatOut.compare("GBK") == 0) {
            Utf8ToGbkTRANSLATE(in, lenOfIn, out, lenOfOut);
        } else if (formatIn.compare("GBK") == 0 && formatOut.compare("UTF8") == 0) {
            GbkToUtf8TRANSLATE(in, lenOfIn, out, lenOfOut);
        } else {
            printf("not GBK or UTF8");
        }
    }

    int GetUtf8VoiceFromInteger(unsigned int value, char *out, unsigned int *lenOfOut) {
        char *uint[5] = {"", "十", "百", "千", "万"};
        unsigned int uintIndex = 0;
        char *number[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
        unsigned int numberIndex = 0;
        char utf8[512] = {0};
        unsigned int utf8Len = 0;
        char strValue[6] = {0};
        unsigned int strValueLen = 0;
        unsigned int strValueIndex = 0;

        if (value == 0) {
            utf8[0] = '0';
            utf8[1] = '\0';
            utf8Len = strlen(utf8);
            memcpy(out, utf8, utf8Len);
            *lenOfOut = utf8Len;
            return *lenOfOut;
        }

        if (value > 99999) {
            printf("value > 99999");
            return -1;
        }

        //1.获取整型数字的字符串
        sprintf(strValue, "%u", value);
        strValueLen = strlen(strValue);
        if (strValueLen > (sizeof(strValue) - 1)) {
            strValueLen = (sizeof(strValue) - 1);
            strValue[strValueLen] = '\0';
        }

        //2.转换字符
        strValueIndex = 0;
        memset(utf8, 0, sizeof(utf8));
        utf8Len = 0;
        do {
            //2.1获取数字的位置
            numberIndex = strValue[strValueIndex] - 0x30;
            strcat(utf8, number[numberIndex]);
            //2.2获取权值的位置
            if (numberIndex != 0) {
                uintIndex = strValueLen - 1 - strValueIndex;
                strcat(utf8, uint[uintIndex]);
            }
            strValueIndex++;
        } while (strValueIndex < strValueLen);
        //3.把字符中连续的0变为一个0
        while (strstr(utf8, "00") != NULL) {
            str_replace(utf8, "00", "0");
        }
        //4.把最后一个0去掉
        utf8Len = strlen(utf8);
        while (utf8[utf8Len - 1] == '0') {
            utf8[utf8Len - 1] = '\0';
            utf8Len--;
        }

        utf8Len = strlen(utf8);
        memcpy(out, utf8, utf8Len);
        *lenOfOut = utf8Len;
        return *lenOfOut;
    }

    int GetUtf8VoiceFromDecimal(double value, char *out, unsigned int *lenOfOut) {
        char *number[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
        unsigned int numberIndex = 0;
        char utf8[512] = {0};
        unsigned int utf8Len = 0;
        char strValue[6] = {0};
        unsigned int strValueLen = 0;
        unsigned int strValueIndex = 0;

        if (value == 0) {
            printf("value = 0");
            return -1;
        }

        if (value >= 1) {
            printf("value >=1");
            return -1;
        }
        //1.获取小数数字的字符串
        sprintf(strValue, "%.3f", value);
        strValueLen = strlen(strValue);
        if (strValueLen > (sizeof(strValue) - 1)) {
            strValueLen = (sizeof(strValue) - 1);
            strValue[strValueLen] = '\0';
        }
        //1.1 去掉小数尾巴上的0
        while (strValue[strValueLen - 1] == '0') {
            strValue[strValueLen - 1] = '\0';
            strValueLen--;
        }

        //2转换字符
        strValueIndex = 0;
        memset(utf8, 0, sizeof(utf8));
        utf8Len = 0;
        strValueIndex = 2;
        do {
            //2.1获取数字的位置
            numberIndex = strValue[strValueIndex] - 0x30;
            strcat(utf8, number[numberIndex]);
            strValueIndex++;
        } while (strValueIndex < strValueLen);

        utf8Len = strlen(utf8);
        memcpy(out, utf8, utf8Len);
        *lenOfOut = utf8Len;
        return *lenOfOut;
    }

/**
 *
 * @param value 浮点型数字
 * @param out 浮点型数字的语音字符串
 * @param lenOfOut 浮点型数字的语音字符串长度
 * @return 浮点型数字的语音字符串长度 -1:error
 * @example value = 10010.5  out="1万01十点5"
 */
    int GetUtf8VoiceFromDouble(double value, char *out, unsigned int *lenOfOut) {
        unsigned int valueInteger;
        double valueDecimal;

        char strIntegers[512] = {0};
        unsigned int strIntegersLen = 0;
        char strDecimals[512] = {0};
        unsigned int strDecimalsLen = 0;

        char utf8[512];
        int utf8Len;

        char *uint_dian = "点";

        memset(utf8, 0, sizeof(utf8));
        // value ==0
        if (value == 0) {
            utf8[0] = '0';
            utf8[1] = '\0';
            utf8Len = strlen(utf8);
            memcpy(out, utf8, utf8Len);
            *lenOfOut = utf8Len;
            return *lenOfOut;
        }

        //1.算出整数部分和小数部分
        valueInteger = (unsigned int) value;
        valueDecimal = value - valueInteger;

        //2.转换整数部分
        if (valueInteger >= 0) {
            memset(strIntegers, 0, sizeof(strIntegers));
            GetUtf8VoiceFromInteger(valueInteger, strIntegers, &strIntegersLen);
            strcat(utf8, strIntegers);
        }
        if (valueDecimal == 0) {
            utf8Len = strlen(utf8);
            memcpy(out, utf8, utf8Len);
            *lenOfOut = utf8Len;
            return *lenOfOut;
        }

        //3.点
        strcat(utf8, uint_dian);
        //4.转换小数部分
        memset(strDecimals, 0, sizeof(strDecimals));
        GetUtf8VoiceFromDecimal(valueDecimal, strDecimals, &strDecimalsLen);

        strcat(utf8, strDecimals);

        utf8Len = strlen(utf8);
        memcpy(out, utf8, utf8Len);
        *lenOfOut = utf8Len;
        return *lenOfOut;
    }

    void str_replace(char *str1, char *str2, char *str3) {
        int i, j, k, done, count = 0, gap = 0;
        char temp[1024 * 512];
        for (i = 0; i < strlen(str1); i += gap) {
            if (str1[i] == str2[0]) {
                done = 0;
                for (j = i, k = 0; k < strlen(str2); j++, k++) {
                    if (str1[j] != str2[k]) {
                        done = 1;
                        gap = k;
                        break;
                    }
                }
                if (done == 0) { // 已找到待替换字符串并替换
                    for (j = i + strlen(str2), k = 0; j < strlen(str1); j++, k++) { // 保存原字符串中剩余的字符
                        temp[k] = str1[j];
                    }
                    temp[k] = '\0'; // 将字符数组变成字符串
                    for (j = i, k = 0; k < strlen(str3); j++, k++) { // 字符串替换
                        str1[j] = str3[k];
                        count++;
                    }
                    for (k = 0; k < strlen(temp); j++, k++) { // 剩余字符串回接
                        str1[j] = temp[k];
                    }
                    str1[j] = '\0'; // 将字符数组变成字符串
                    gap = strlen(str2);
                }
            } else {
                gap = 1;
            }
        }
        if (count == 0) {
//		LogInfo("Can't find the replaced string!");
        }
    }

    bool IsLocalProcessRun(const char *name) {
        char command[128] = {0};
        FILE *fp = NULL;
        char pid_buff[10] = {0};
        int pidnum = 0;
        char *pid = NULL;

        if (name == NULL) {
            return false;
        }
        snprintf(command, sizeof(command) - 1, "ps -ef | grep %s | grep -v grep | awk \'{print $2}\'", name);
        printf("command:%s\n", command);
        fp = popen(command, "r");
        pid = fgets(pid_buff, sizeof(pid_buff) - 1, fp);
        pidnum = atoi(pid_buff);
        printf("%s pid:%d\n", name,pidnum);
        if (!pid || pidnum == 0) {
            pclose(fp);
            return false;
        }
        pclose(fp);
        if (pidnum <= 0) {
            //程序未运行
            return false;
        }
        return true;
    }




}