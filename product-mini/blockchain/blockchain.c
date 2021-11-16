/*********************************************************************
* Filename:   sha256.c
* Author:     Brad Conte (brad AT bradconte.com)
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Performs known-answer tests on the corresponding SHA1
	          implementation. These tests do not encompass the full
	          range of available test vectors, however, if the tests
	          pass it is very, very likely that the code is correct
	          and was compiled properly. This code also serves as
	          example usage of the functions.
*********************************************************************/

/*************************** HEADER FILES ***************************/
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include "sha256.h"

int main()
{
	BYTE pattern[2] = {0x00,0x00};
	BYTE prev[SHA256_BLOCK_SIZE] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
																	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	BYTE buf[SHA256_BLOCK_SIZE];
	SHA256_CTX ctx;

	long int block = 0;

	while(1) {
		for (int i = 0; i < 500000; i++) {
			BYTE data[SHA256_BLOCK_SIZE*2+6];
			sprintf(data, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%d",
							prev[0],prev[1],prev[2],prev[3],prev[4],prev[5],prev[6],prev[7],prev[8],
							prev[9],prev[10],prev[11],prev[12],prev[13],prev[14],prev[15],prev[16],
							prev[17],prev[18],prev[19],prev[20],prev[21],prev[22],prev[23],prev[24],
							prev[25],prev[26],prev[27],prev[28],prev[29],prev[30],prev[31],i);

			sha256_init(&ctx);
			sha256_update(&ctx, data, strlen(data));
			sha256_final(&ctx, buf);

			if (!memcmp(pattern, buf, 2)) {
				printf("Block %ld:\n", block);
				printf("         Nonce: %d\n", i);
				printf("         Prev: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
								prev[0],prev[1],prev[2],prev[3],prev[4],prev[5],prev[6],prev[7],prev[8],
								prev[9],prev[10],prev[11],prev[12],prev[13],prev[14],prev[15],prev[16],
								prev[17],prev[18],prev[19],prev[20],prev[21],prev[22],prev[23],prev[24],
								prev[25],prev[26],prev[27],prev[28],prev[29],prev[30],prev[31]);
				printf("         Hash: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n\n",
								buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8],
								buf[9],buf[10],buf[11],buf[12],buf[13],buf[14],buf[15],buf[16],
								buf[17],buf[18],buf[19],buf[20],buf[21],buf[22],buf[23],buf[24],
								buf[25],buf[26],buf[27],buf[28],buf[29],buf[30],buf[31]);
				memcpy(prev, buf, SHA256_BLOCK_SIZE*sizeof(BYTE));
				block++;
				break;
			}
		}
	}
	return(0);
}
