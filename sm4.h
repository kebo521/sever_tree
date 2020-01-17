#ifndef __SM4_H_
#define __SM4_H_

// 加解密接口, 允许输入缓冲与输出缓冲复用
void SM4_ENC(u8 *Buff ,u8 *key ,u8 key_number);
void SM4_DEC(u8 *Buff ,u8 *key ,u8 key_number);

#endif
