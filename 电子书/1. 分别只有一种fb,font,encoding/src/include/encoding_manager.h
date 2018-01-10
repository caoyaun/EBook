#ifndef __ENCODINGMANGER_H
#define __ENCODINGMANGER_H

typedef struct enconding{
	char  *name;
	int file_head;
	int (*isSupportThisEncoding)(unsigned char *pcFileHeadPos);
	int (*getEncoding)(unsigned int *pdwCode, unsigned char *pcFileCurPos, unsigned char *pcFileEndPos);
	struct enconding *next_encoding;
}T_ENCODING, *PT_ENCODING;

int register_encoding(PT_ENCODING ptEncodingOpr);
PT_ENCODING find_support_encoding(unsigned char *pcFileHeadPos);
int utf8_init(void);
void encoding_init(void);

#endif
