#pragma once

#ifdef _MSC_VER
#define bswap16(i) *(i) = _byteswap_ushort(*(i))
#define bswap32(i) *(i) = _byteswap_ulong(*(i))
#define bswap64(i) *(i) = _byteswap_uint64(*(i))
#elif
#define bswap16(i) *(i) = ((*(i)&0xFF)<<8 | *(i)>>8&0xFF)
#define bswap32(i) *(i) = ((*(i)&0xFF)<<24 | (*(i)&0xFF00)<<8 | *(i)>>8&0xFF00 | *(i)>>24&0xFF)
#define bswap64(i) *(i) = ((*(i)&0xFF)<<56 | (*(i)&0xFF00)<<40 | (*(i)&0xFF0000)<<24 | (*(i)&0xFF000000)<<8 | *(i)>>8&0xFF000000 | *(i)>>24&0xFF0000 | *(i)i>>40&0xFF00 | *(i)>>56&0xFF)
#endif