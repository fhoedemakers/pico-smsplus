#ifndef HASH_H
#define HASH_H

#define SHA1_DIGEST_SIZE 20

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

#ifdef WORDS_BIGENDIAN
#define blk0(i) block->l[i]
#else
#define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
	|(rol(block->l[i],8)&0x00FF00FF))
#endif
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
	^block->l[(i+2)&15]^block->l[i&15],1))

#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

typedef struct {
	uint32_t state[5];
	uint32_t count[2];
	uint8_t  buffer[64];
} sha1_context_t;

void sha1(uint8_t *digest, const void *buf, size_t size);
void sha1_init(sha1_context_t* context);
void sha1_transform(uint32_t state[5], const uint8_t buffer[64]);
void sha1_update(sha1_context_t* context, const uint8_t* data, const size_t len);
void sha1_final(sha1_context_t* context, uint8_t digest[SHA1_DIGEST_SIZE]);

uint32_t crc32(uint32_t crc, const void *buf, size_t size);

#endif
