#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "bitmap.h"
#include "bitmap.c"
#include "block_store.h"
#include <string.h>
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
    // Allocate memory for our block store
    block_store_t* new_block_storage = (block_store_t*)malloc(sizeof(block_store_t));
    
    // Check to see if our memory allocation failed! If not, continue
    if (new_block_storage == NULL) {
        return NULL;
    }
    
    // Initializes our block store's bitmap!
    new_block_storage->bitmap = bitmap_create(BITMAP_SIZE_BYTES);
    
    //new_block_storage->bitmap = &new_block_storage -> block_data[BITMAP_SIZE_BYTES - 1];
    
    return new_block_storage;
}

void block_store_destroy(block_store_t *const bs)
{
    // Checks to see if our parameters we are passed are invalid! In this case,
    // simply return so we don't do anything else
    if (bs == NULL || bs->bitmap == NULL) {
        return;
    }
    
    // Invokes the bitmap_destory on our block store's bitmap
    bitmap_destroy(bs->bitmap);
    free(bs);
    return;
}

///
/// Searches for a free block, marks it as in use, and returns the block's id
/// \param bs BS device
/// \return Allocated block's id, SIZE_MAX on error
///
size_t block_store_allocate(block_store_t *const bs)
{
    // Checking all parameters to ensure all are valid
    if (bs != NULL)
    {
        if (bs->bitmap!=NULL)
        {
            // Finding the first zero so we can allocate an unused block.
            size_t toAllocate = bitmap_ffz(bs->bitmap);
            // Making sure there is a zero.
            if (toAllocate != SIZE_MAX)
            {
                // Flipping the bit in the bitmap to reflect it being in use.
                bitmap_flip(bs->bitmap, toAllocate);
                // Allocating the memory to the array in the appropriate block.
                bs->block_data[toAllocate][0] = malloc(BLOCK_SIZE_BITS);
                // Ensuring it doesn't return NULL (have an error in allocation)
                if (bs->block_data[toAllocate][0] != NULL)
                {
                    // Return this indicating a success
                    return toAllocate;
                }
                else
                {
                    // If it does fail, we should flip this back as we are not using it.
                    bitmap_flip(bs->bitmap, toAllocate);
                }
            }
        }
    }
    // Return this indicating a failure.
    return SIZE_MAX;
}

///
/// Attempts to allocate the requested block id
/// \param bs the block store object
/// \block_id the requested block identifier
/// \return boolean indicating succes of operation
///
bool block_store_request(block_store_t *const bs, const size_t block_id)
{
     // Checking all parameters to ensure all are valid
    if (bs != NULL)
    {
        if (block_id > 0 && block_id < BLOCK_STORE_NUM_BLOCKS)
        {
            // Checking in the bitmap to see if the requested block is in use.
            if(!bitmap_test(bs->bitmap, block_id))
            {
                // Flipping the bitmap as it is now in use.
                bitmap_flip(bs->bitmap, block_id);
                // Allocating the appropriate block.
                bs->block_data[block_id][0] = malloc(BLOCK_SIZE_BITS);
                // Making sure the allocation succeeds.
                if (bs->block_data[block_id][0] != NULL)
                {
                    // Returning this indicating a success.
                    return true;
                }
                else
                {
                    // If the allocation fails, flipping this back as we are not using it.
                    bitmap_flip(bs->bitmap, block_id);
                }
            }
        }
    }
    // Returning this indicating a failure.
    return false;
}

///
/// Frees the specified block
/// \param bs BS device
/// \param block_id The block to free
///
void block_store_release(block_store_t *const bs, const size_t block_id)
{
    // Checking all parameters to ensure all are valid
    if(bs != NULL)
    {
        if (block_id > 0 && block_id < BLOCK_STORE_NUM_BLOCKS && bs->bitmap != NULL && bs->block_data != NULL)
        {
            // Checking if the bitmap was already flipped.
            if (bitmap_test(bs->bitmap, block_id))
            {
                // If it was, flip it.
                bitmap_flip(bs->bitmap, block_id);
            }
            // Freeing the block.
            free(bs->block_data[block_id]);
        }
    }
}

size_t block_store_get_used_blocks(const block_store_t *const bs)
{
    // Check to see if our parameter is faulty; if it is, per the header file,
    // return SIZE_MAX!
    if (bs == NULL) {
        return SIZE_MAX;
    }
    
    // If everything is good, use an existing bitmap function to return the 
    // used blocks
    return bitmap_total_set(bs->bitmap);
}

size_t block_store_get_free_blocks(const block_store_t *const bs)
{
    // Check to see if our parameter is faulty; if it is, per the header file,
    // return SIZE_MAX!
    if (bs == NULL) {
        return SIZE_MAX;
    }
    
    // If everything is good, we can simply subtract the total number of set
    // bits from the total number of block bits to get the ones that aren't set!
    return BLOCK_SIZE_BITS - bitmap_total_set(bs->bitmap);
}

///
/// Returns the total number of user-addressable blocks
///  (since this is constant, you don't even need the bs object)
/// \return Total blocks
///
size_t block_store_get_total_blocks()
{
    // The total blocks should always be the value in BLOCK_STORE_NUM_BLOCKS.
    return BLOCK_STORE_NUM_BLOCKS;
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