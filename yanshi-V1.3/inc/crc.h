#ifndef CRC_H
#define CRC_H
extern void init_crc_table(void);
extern unsigned int crc32(char *data,int size);
#endif // CRC_H
