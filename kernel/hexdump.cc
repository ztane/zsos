#include <stdio.h>
#include <stdlib.h>
#include <printk.h>

// ripped from net, todo write better
void hexdump(const void *pAddressIn, uint32_t size)
{
        char outputBuf[100];
        long indent = 1;
        long outlen, index, index2, outlen2;
        long relpos;
        struct { char *pData; unsigned long size; } buf;
        unsigned char *pTmp,ucTmp;
        unsigned char *pAddress = (unsigned char *)pAddressIn;

        buf.pData   = (char *)pAddress;
        buf.size   = size;

        while (buf.size > 0)
        {
            pTmp     = (unsigned char *)buf.pData;
            outlen  = (int)buf.size;
            if (outlen > 16)
                outlen = 16;

            // create a 64-character formatted output line:
            snprintf(outputBuf, 100, " >                            "
                           "                      "
                           "    %08lX", pTmp-pAddress);
            outlen2 = outlen;

            for(index = 1+indent, index2 = 53-15+indent, relpos = 0;
                outlen2;
                outlen2--, index += 2, index2++
               )
            {
               ucTmp = *pTmp++;

               snprintf(outputBuf + index, 10, "%02X ", (unsigned short)ucTmp);
               if(ucTmp < 32 || (ucTmp >= 128 && ucTmp < 160))  ucTmp = '.'; // nonprintable char
               outputBuf[index2] = ucTmp;

               if (!(++relpos & 3))     // extra blank after 4 bytes
               {  index++; outputBuf[index+2] = ' '; }
            }

            if (!(relpos & 3)) index--;

            outputBuf[index  ]   = '<';
            outputBuf[index+1]   = ' ';

            printk("%s\n", outputBuf);

            buf.pData   += outlen;
            buf.size   -= outlen;
         }
}

