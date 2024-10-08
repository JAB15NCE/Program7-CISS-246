/*
Name: Jon Bennett & Garrett S. 
Date:12/03/2023
Description: This program runs a Fully-Associative Memory cache mapper on the first
 4 memoryblocks while implementing a First in first out (FIFO) Policy.   
*/

#include <stdio.h>
#include "Cache.h"
#include "PrintingFormats.h"


//PRIVATE FUNCTION PROTOTYPES
uint8_t GetCacheSetIndex(CachePtr cachePtr, uint8_t memoryAddress);

void CacheInitialize(CachePtr cachePtr, MainMemoryPtr mainMemoryPtr)
{
    for (int index = 0; index < CACHE_SETS; index++)
    {
        cachePtr->cacheSets[index].validBit = false;
        cachePtr->cacheSets[index].tag = 0;
    }

    cachePtr->mainMemoryPtr = mainMemoryPtr;
}

uint8_t CacheReadWord(CachePtr cachePtr, uint8_t memoryAddress)
{
    uint8_t cacheSetIndex = GetCacheSetIndex(cachePtr, memoryAddress);

    //Read the word from the cache set
    uint8_t cacheBlockOffset = (uint8_t)(memoryAddress & CACHE_BLOCK_OFFSET_BITMASK);
    uint8_t word = cachePtr->cacheSets[cacheSetIndex].blockValues[cacheBlockOffset];

    return word;
}

void CacheWriteWord(CachePtr cachePtr, uint8_t mainMemoryAddress, uint8_t word)
{
    int cacheSetIndex = GetCacheSetIndex(cachePtr, mainMemoryAddress);

    //Write the word to the cache set
    uint8_t cacheBlockOffset = (uint8_t)(mainMemoryAddress & CACHE_BLOCK_OFFSET_BITMASK);
    cachePtr->cacheSets[cacheSetIndex].blockValues[cacheBlockOffset] = word;
}

uint8_t GetCacheSetIndex(CachePtr cachePtr, uint8_t mainMemoryAddress)
{
    //this initializes the index.
    uint8_t i = 0;

    //Determine the address tag fields
    uint8_t addressTagField = mainMemoryAddress & CACHE_SET_TAG_BITMASK;
    addressTagField = addressTagField >> (CACHE_TAG_BITS + CACHE_BLOCK_OFFSET_BITS);

    //Reads the address and checks to make sure that there is a matching tag
    for ( i = 0; i < CACHE_SETS; i++){
    bool isCacheSetValid = cachePtr->cacheSets[i].validBit;
    bool tagFieldsMatch = addressTagField == cachePtr->cacheSets[i].tag;
        if (isCacheSetValid && tagFieldsMatch){
        return i;
        }
    }
    //This performs the FIFO policy and replaces the oldesBlockIndex to forfill the Fully-Associative Cache 
    static uint8_t oldestBlockIndex = 0;
    oldestBlockIndex ++;
    oldestBlockIndex %= 4; //++, if 4, set to zero     
    
    
    
        //The address of the memory block is determined by the cache set index bits and 
        //the cache set tag bits.
        uint8_t memoryBlockAddress = mainMemoryAddress & MAIN_MEMORY_BLOCK_ADDDRESS_BITMASK;

        //Read the block from memory
        cachePtr->cacheSets[oldestBlockIndex].blockValues[0] = cachePtr->mainMemoryPtr->values[memoryBlockAddress];
        cachePtr->cacheSets[oldestBlockIndex].blockValues[1] = cachePtr->mainMemoryPtr->values[memoryBlockAddress + 1];
        cachePtr->cacheSets[oldestBlockIndex].blockValues[2] = cachePtr->mainMemoryPtr->values[memoryBlockAddress + 2];
        cachePtr->cacheSets[oldestBlockIndex].blockValues[3] = cachePtr->mainMemoryPtr->values[memoryBlockAddress + 3];

        //Update the cache set tag field and valid bit
        cachePtr->cacheSets[oldestBlockIndex].tag = addressTagField;
        cachePtr->cacheSets[oldestBlockIndex].validBit = true;
    

    return oldestBlockIndex;
}

void CacheViewCache(CachePtr cachePtr)
{
    printf("****************************** CACHE CONTENTS *****************************\n\n");
    
    for (int cacheSetIndex = 0; cacheSetIndex < CACHE_SETS; cacheSetIndex++)
    {
        bool validBit = cachePtr->cacheSets[cacheSetIndex].validBit;
        uint8_t tag = cachePtr->cacheSets[cacheSetIndex].tag;

        printf("Cache Set Binary Address: ");
        printf("["TW0_BITS_TO_BINARY_PATTERN"]", TWO_BITS_TO_BINARY(tag));
        printf("["TW0_BITS_TO_BINARY_PATTERN"]", TWO_BITS_TO_BINARY(cacheSetIndex));
        printf("["TW0_BITS_TO_BINARY_PATTERN"]", TWO_BITS_TO_BINARY(0));
        printf("\n\n");

        printf("Cache Set Valid Bit: %d\n", validBit);
        printf("Cache Set Tag:       ");
        printf("%d    ", tag);
        printf("["TW0_BITS_TO_BINARY_PATTERN"]", TWO_BITS_TO_BINARY(tag));
        printf("\n");
        printf("Cache Set Index:     ");
        printf("%d      ", cacheSetIndex);
        printf("["TW0_BITS_TO_BINARY_PATTERN"]", TWO_BITS_TO_BINARY(cacheSetIndex));
        printf("\n");

        for (int blockValuesIndex = 0; blockValuesIndex < CACHE_BLOCK_SIZE; blockValuesIndex++)
        {
            uint8_t word = cachePtr->cacheSets[cacheSetIndex].blockValues[blockValuesIndex];

            printf("Cache Word Binary Address: ");
            printf(TW0_BITS_TO_BINARY_PATTERN, TWO_BITS_TO_BINARY(tag));
            printf(TW0_BITS_TO_BINARY_PATTERN, TWO_BITS_TO_BINARY(cacheSetIndex));
            printf("["TW0_BITS_TO_BINARY_PATTERN"]", TWO_BITS_TO_BINARY(blockValuesIndex));
            printf(" Word: %d ", word);
            printf("\n");
        }

        printf("\n\n");
    }

    printf("**************************** END CACHE CONTENTS ***************************\n\n");
}