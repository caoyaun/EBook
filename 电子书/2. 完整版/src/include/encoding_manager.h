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
void encoding_init(void);
int utf8_init(void);
int ascii_init(void);
int utf16Le_init(void);
int utf16Be_init(void);

#endif
