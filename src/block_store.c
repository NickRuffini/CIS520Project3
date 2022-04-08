#include <stdio.h>
#include <stdint.h>
#include "bitmap.h"
#include "block_store.h"
// include more if you need

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

// Declaring the struct but not implementing in the header allows us to prevent users
// from using the object directly and monkeying with the contents
// They can only create pointers to the struct, which must be given out by us
// This enforces a black box device, but it can be restricting
typedef struct block_store {
    bitmap_t *bitmap;
    // available blocks and size in bytes
    char * block_data[BLOCK_STORE_AVAIL_BLOCKS][BLOCK_SIZE_BYTES];
} block_store_t;

block_store_t *block_store_create()
{
    return NULL;
}

void block_store_destroy(block_store_t *const bs)
{
    UNUSED(bs);
}
size_t block_store_allocate(block_store_t *const bs)
{
    UNUSED(bs);
    return 0;
}

bool block_store_request(block_store_t *const bs, const size_t block_id)
{
    UNUSED(bs);
    UNUSED(block_id);
    return false;
}

void block_store_release(block_store_t *const bs, const size_t block_id)
{
    UNUSED(bs);
    UNUSED(block_id);
}

size_t block_store_get_used_blocks(const block_store_t *const bs)
{
    UNUSED(bs);
    return 0;
}

size_t block_store_get_free_blocks(const block_store_t *const bs)
{
    UNUSED(bs);
    return 0;
}

size_t block_store_get_total_blocks()
{
    return 0;
}

size_t block_store_read(const block_store_t *const bs, const size_t block_id, void *buffer)
{
    // checks parameter errors
    if(bs == NULL || buffer == NULL || block_id > BLOCK_STORE_NUM_BYTES){
        return 0;
    }

    // copys data into buffer
    if(!memcpy(buffer, bs->block_data[block_id], BLOCK_SIZE_BYTES)){
        return 0;
    }

    // returns bytes that were read
    return BLOCK_SIZE_BYTES;
}

size_t block_store_write(block_store_t *const bs, const size_t block_id, const void *buffer)
{
    // checks parameter errors
    if(bs == NULL || buffer == NULL || block_id >= BLOCK_STORE_NUM_BYTES){
        return 0;
    }

    // copy buffer data into the block data
    memcpy(bs->block_data[block_id], buffer, BLOCK_SIZE_BYTES);

    // return bytes that were written
    return BLOCK_SIZE_BYTES;
} 

block_store_t *block_store_deserialize(const char *const filename)
{
    UNUSED(filename);
    return NULL;
}

size_t block_store_serialize(const block_store_t *const bs, const char *const filename)
{
    UNUSED(bs);
    UNUSED(filename);
    return 0;
}