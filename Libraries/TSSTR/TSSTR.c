#include "TSSTR.h"

char* ustrfmt(char* Output, char* Input, ...) //unsafe string formatter
{
    va_list Arguments;
    va_start(Arguments,Input);
    size_t Size = strlen(Input)+1;
    int oi = 0;

    char Int8Buff[4+1] = "\0";
    char Uint8Buff[3+1] = "\0";
    char Int16Buff[6+1] = "\0";
    char Uint16Buff[5+1] = "\0";
    char Int32Buff[11+1] = "\0";
    char Uint32Buff[10+1] = "\0";
    char Int64Buff[21+1] = "\0";
    char Uint64Buff[20+1] = "\0";
    char CharBuff[1+1] = "\0";

    for(size_t i = 0; i < Size; i++)
    {
        if(Input[i] == '%')
        {
            if(i+1 < Size)
            {
                if(Input[i+1] == 'i')
                {
                    if(i+2 < Size)
                    {
                        if(Input[i+2] == '8')
                        {
                            snprintf(Uint8Buff,sizeof(Uint8Buff),"%hhu",va_arg(Arguments,unsigned int));
                            size_t BSize = strlen(Uint8Buff);
                            for(size_t j = 0; j < BSize; j++)
                            {
                                Output[oi] = Uint8Buff[j];
                                oi++;
                            }
                            i+=2;
                        }
                        else
                        {
                            if(i+3 < Size)
                            {
                                if(Input[i+2] == '1' && Input[i+3] == '6')
                                {
                                    snprintf(Uint16Buff,sizeof(Uint16Buff),"%hu",va_arg(Arguments,unsigned int));
                                    size_t BSize = strlen(Uint16Buff);
                                    for(size_t j = 0; j < BSize; j++)
                                    {
                                        Output[oi] = Uint16Buff[j];
                                        oi++;
                                    }
                                    i+=3;
                                }
                                if(Input[i+2] == '3' && Input[i+3] == '2')
                                {
                                    snprintf(Uint32Buff,sizeof(Uint32Buff),"%lu",va_arg(Arguments,unsigned long int));
                                    size_t BSize = strlen(Uint32Buff);
                                    for(size_t j = 0; j < BSize; j++)
                                    {
                                        Output[oi] = Uint32Buff[j];
                                        oi++;
                                    }
                                    i+=3;
                                }
                                if(Input[i+2] == '6' && Input[i+3] == '4')
                                {
                                    snprintf(Uint64Buff,sizeof(Uint64Buff),"%llu",va_arg(Arguments,unsigned long long int));
                                    size_t BSize = strlen(Uint64Buff);
                                    for(size_t j = 0; j < BSize; j++)
                                    {
                                        Output[oi] = Uint64Buff[j];
                                        oi++;
                                    }
                                    i+=3;
                                }
                            }
                        }
                    }
                }
                if(Input[i+1] == 'I')
                {
                    if(i+2 < Size)
                    {
                        if(Input[i+2] == '8')
                        {
                            snprintf(Int8Buff,sizeof(Int8Buff),"%hhd",va_arg(Arguments,signed int));
                            size_t BSize = strlen(Int8Buff);
                            for(size_t j = 0; j < BSize; j++)
                            {
                                Output[oi] = Int8Buff[j];
                                oi++;
                            }
                            i+=2;
                        }
                        else
                        {
                            if(i+3 < Size)
                            {
                                if(Input[i+2] == '1' && Input[i+3] == '6')
                                {
                                    snprintf(Int16Buff,sizeof(Int16Buff),"%hd",va_arg(Arguments,signed int));
                                    size_t BSize = strlen(Int16Buff);
                                    for(size_t j = 0; j < BSize; j++)
                                    {
                                        Output[oi] = Int16Buff[j];
                                        oi++;
                                    }
                                    i+=3;
                                }
                                if(Input[i+2] == '3' && Input[i+3] == '2')
                                {
                                    snprintf(Int32Buff,sizeof(Int32Buff),"%ld",va_arg(Arguments,signed long int));
                                    size_t BSize = strlen(Int32Buff);
                                    for(size_t j = 0; j < BSize; j++)
                                    {
                                        Output[oi] = Int32Buff[j];
                                        oi++;
                                    }
                                    i+=3;
                                }
                                if(Input[i+2] == '6' && Input[i+3] == '4')
                                {
                                    snprintf(Int64Buff,sizeof(Int64Buff),"%lld",va_arg(Arguments,signed long long int));
                                    size_t BSize = strlen(Int64Buff);
                                    for(size_t j = 0; j < BSize; j++)
                                    {
                                        Output[oi] = Int64Buff[j];
                                        oi++;
                                    }
                                    i+=3;
                                }
                            }
                        }
                    }
                }
                if(Input[i+1] == 'c')
                {
                    Output[oi] = (unsigned char)va_arg(Arguments,unsigned int);
                    oi++;
                    i+=1;
                }
                if(Input[i+1] == 's')
                {
                    unsigned char* S = va_arg(Arguments,unsigned char*);
                    size_t SSize = strlen(S);
                    for(size_t j = 0; j < SSize; j++)
                    {
                        Output[oi] = S[j];
                        oi++;
                    }
                    i+=1;
                }
            }
        }
        else
        {
            Output[oi] = Input[i];
            oi++;
        }
    }

    Output[oi] = '\0';
    va_end(Arguments);
    return(Output);
}