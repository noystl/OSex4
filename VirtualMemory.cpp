#include "VirtualMemory.h"
#include "PhysicalMemory.h"

//----------------------------Private Function

void clearTable(uint64_t frameIndex) {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(frameIndex * PAGE_SIZE + i, 0);
    }
}

uint64_t findFrame(const uint64_t (&doNotDelete)[TABLES_DEPTH]){ // Gets an array of frames it shouldn't evict.
    uint64_t phisicalAddr;
    // Max cyclic score
    // Max frame with max cyclic score
    // Max frame index
    // TODO: Do.
    return phisicalAddr;
}

/**
 * Returns a "substring" of the address according to the mask, and moves it around according to the shifts.
 * Example: If the address is 1000101, the mask is 1110000
 * @param mask
 * @param shifts
 * @return
 */
uint64_t getSubAddress(uint64_t  Address, uint64_t mask, uint64_t shifts){

}


/**
 * This function initializes offset, pageIndex and searchRows.
 * @param virtualAddress
 * @param offset
 * @param searchRows
 */
void translatePreprocces(uint64_t virtualAddress, uint64_t& offset, uint64_t& pageIndex,
        uint64_t (&searchRows)[TABLES_DEPTH]){

}

/**
 * Translates a virtual address to physical address.
 * @param toTrans: The virtual address to translate.
 * @return: The translation to physical address.
 */
uint64_t translate(uint64_t toTrans){
    uint64_t doNotEvict[TABLES_DEPTH];
    uint64_t usedTablesNum = 0 ;        // This is also the number of elements stored in doNotEvict.
    uint64_t searchRows[TABLES_DEPTH];  // Holds indexes of table entries relevant to the search.
    uint64_t offset;
    uint64_t pageIndex;
    word_t currFrameIndex = 0;          // We always start the search from frame 0.
    word_t nextFrameIndex;

    // Split the virtual address into parts and init relevant variables:
    translatePreprocces(toTrans, offset, pageIndex, searchRows);

    // Init nextFrameIndex for the first time (it's important to do it outside the loop in case we have just one level):
    PMread(currFrameIndex * PAGE_SIZE + searchRows[0], &nextFrameIndex);

    // Find the frame for the requested page:
    for(int i = 1; i < (TABLES_DEPTH - 1); i++){
        // If the requested table is not in memory (Page Fault):
        if(nextFrameIndex == 0){
            // Find and init a new table:
            nextFrameIndex = (word_t)findFrame(doNotEvict);
            clearTable(nextFrameIndex);
            PMwrite(currFrameIndex * PAGE_SIZE + searchRows[i], nextFrameIndex);

            // Verify it won't be deleted next time we'll need an empty frame:
            doNotEvict[usedTablesNum] = nextFrameIndex;
            usedTablesNum++;
        }

        currFrameIndex = nextFrameIndex;
        PMread(currFrameIndex * PAGE_SIZE + searchRows[0], &nextFrameIndex);
    }


    // If the requested page is not in memory (Page Fault):
    if(nextFrameIndex == 0){
        nextFrameIndex = (word_t)findFrame(doNotEvict);
        PMrestore(nextFrameIndex, pageIndex);
        PMwrite(currFrameIndex * PAGE_SIZE + searchRows[TABLES_DEPTH - 1], nextFrameIndex);
    }

    return nextFrameIndex * PAGE_SIZE + offset;
}



//----------------------------- Public Functions

void VMinitialize() {
    clearTable(0);
}

int VMread(uint64_t virtualAddress, word_t* value) {
    return 1;
}


int VMwrite(uint64_t virtualAddress, word_t value) {
    uint64_t phisicalAddress = translate(virtualAddress);
    PMwrite(phisicalAddress, value);
    return 1;
}