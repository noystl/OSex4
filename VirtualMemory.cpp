#include <cmath>
#include <iterator>
#include "VirtualMemory.h"
#include "PhysicalMemory.h"

//----------------------------Private Function

#define MAX(X, Y)  ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))
#define ABS(X)  ((X) >= 0 ? (X) : (-X))


void clearTable(uint64_t frameIndex) {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(frameIndex * PAGE_SIZE + i, 0);
    }
}


bool findFrameHelper(word_t currFrameIdx, unsigned int hierLevel, word_t currParentIdx, uint64_t pathToFrame,
                     word_t &emptyFrameIdx, word_t &unusedFrameIdx, word_t &frameIdxWithMaxCyclicScore,
                     uint64_t &maxCyclicScore, word_t &parentIdx,
                     const uint64_t &pageIndex, const uint64_t (&doNotDelete)[TABLES_DEPTH])
{
    bool isFirstFulfilled = true;
    word_t tableEntry;

    // Check if 1st criteria is fulfilled:
    for (unsigned int i = 0; i < PAGE_SIZE; ++i) //stop condition: empty frame. TODO: check bounds
    {
        PMread(currFrameIdx * PAGE_SIZE + i, &tableEntry);
        if (tableEntry != 0)
        {
            isFirstFulfilled = false;
            unusedFrameIdx = MAX(unusedFrameIdx,tableEntry); //maintain 2nd criteria. TODO: what if we're in a leaf?
        }
    }

    // Check that it's not a "false alarm"- the frame belongs to path:
    if (isFirstFulfilled)
    {
        if (std::find(std::begin(doNotDelete), std::end(doNotDelete), currFrameIdx) == std::end(doNotDelete))
        {
            //prepare for successful return on 1st criteria:
            parentIdx = currParentIdx;
            emptyFrameIdx = currFrameIdx;
        }
        else{
            isFirstFulfilled = false;
        }
    }

    // Respond accordingly:
    if (!isFirstFulfilled)
    {
        // stop condition: leaf.
        if (hierLevel == TABLES_DEPTH) //TODO: check bounds
        {
            uint64_t absDist = ABS(pageIdx - pathToFrame);
            uint64_t pageDist = MIN(NUM_PAGES - absDist, absDist);
            if(pageDist > maxCyclicScore)
            {
                maxCyclicScore = pageDist; //maintain 3rd criteria

                //prepare for successful return on 3rd criteria:
                frameIdxWithMaxCyclicScore = currFrameIdx;
                parentIdx = currParentIdx;
            }
        }
        else
        {
            //recursion:
            for (unsigned int i = 0; i < PAGE_SIZE; ++i) //TODO: check bounds
            {
                PMread(currFrameIdx * PAGE_SIZE + i, &tableEntry);
                if (tableEntry != 0)
                {
                    return findFrameHelper(tableEntry, hierLevel + 1, currFrameIdx, pathToFrame, //TODO: operate func on pathToFrame
                                    emptyFrameIdx, unusedFrameIdx, frameIdxWithMaxCyclicScore,
                                    maxCyclicScore, parentIdx, pageIndex, doNotDelete);
                }
            }
        }
    }

    //return indicator suggesting to which return value to address:
    return isFirstFulfilled;
}


word_t findFrame(const uint64_t &pageIndex, const uint64_t &offset,
                 const uint64_t (&doNotDelete)[TABLES_DEPTH])
{
    //will contain the frames that answer thw 1st, 2nd and 3rd criteria accordingly:
    word_t emptyFrameIdx = 0, unusedFrameIdx = 0, frameIdxWithMaxCyclicScore = 0;
    uint64_t maxCyclicScore = 0;
    word_t parentIdx = 0;

    bool isFirstFulfilled = findFrameHelper(0, 0, NULL, NULL, emptyFrameIdx, unusedFrameIdx,
                                            frameIdxWithMaxCyclicScore, maxCyclicScore,
                                            parentIdx, pageIndex, doNotDelete);
    if (isFirstFulfilled)
    {
        //dereference from parent:
        //TODO
        return emptyFrameIdx;
    }
    if (unusedFrameIdx + 1 < NUM_FRAMES)
    {
        return unusedFrameIdx + 1;
    }
    //dereference from parent:
    //TODO
    return frameIdxWithMaxCyclicScore;
}



/**
 * Returns a "substring" of the address according to the mask, and moves it left according to the shifts.
 * Example: If address = 1000101, mask = 1110000 and shifts = 4, getSubAddress() returns 0000100.
 * @param mask
 * @param shifts
 * @return
 */
uint64_t getSubAddress(uint64_t  address, uint64_t mask, int shifts){
    // Apply mask:
    uint64_t temp = address & mask;

    // Shift as needed:
    temp = temp >> shifts;

    return temp;
}

/**
 * Creates a mask for subsequent bits according to length and position.
 * Examples:
 * If length = 3 and position = 2 the mask will be 11100.
 * If length = 2 and position = 5 the mask will be 1100000.
 * @param length
 * @param position
 * @return
 */
uint64_t createMask(int length, int position){
    uint64_t mask = 0;
    uint64_t oneBit = 1;                // in binary 0000000.....0001

    // Turn on the correct number of bits:
    for(int i = 0; i < length; i++){
        mask += oneBit;
        oneBit = oneBit << 1;
    }

    // Move the bits to the correct place:
    mask = mask << position;

    return mask;
}


/**
 * This function initializes offset, pageIndex and searchRows.
 * @param virtualAddress
 * @param offset
 * @param searchRows
 */
void translatePreprocces(uint64_t virtualAddress, uint64_t& offset, uint64_t& pageIndex,
                         uint64_t (&searchRows)[TABLES_DEPTH]){

    // Init offset:
    uint64_t offsetMask = createMask(OFFSET_WIDTH, 0);
    offset = getSubAddress(virtualAddress, offsetMask, 0);

    // Init pageIndex:
    int bitsForPageIndex = VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH;
    uint64_t pageIndexMask = createMask(bitsForPageIndex, OFFSET_WIDTH);
    pageIndex = getSubAddress(virtualAddress, pageIndexMask, OFFSET_WIDTH);

    // Init searchRows:
    int bitsForRow = std::ceil( (float)bitsForPageIndex / (float)TABLES_DEPTH);
    uint64_t currRowMask = createMask(bitsForRow, 0);

    for(int i = 0; i < TABLES_DEPTH; i++){
        searchRows[(TABLES_DEPTH - 1) - i] = getSubAddress(pageIndex, currRowMask, i*bitsForRow);
        currRowMask = currRowMask << bitsForRow;
    }
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
    for(int i = 0; i < TABLES_DEPTH - 1; i++){
        // If the requested table is not in memory (Page Fault):
        if(nextFrameIndex == 0){
            // Find and init a new table:
            nextFrameIndex = findFrame(pageIndex, offset, doNotEvict);
            clearTable(nextFrameIndex);
            PMwrite(currFrameIndex * PAGE_SIZE + searchRows[i], nextFrameIndex);

            // Verify it won't be deleted next time we'll need an empty frame:
            doNotEvict[usedTablesNum] = nextFrameIndex;
            usedTablesNum++;
        }

        currFrameIndex = nextFrameIndex;
        PMread(currFrameIndex * PAGE_SIZE + searchRows[i + 1], &nextFrameIndex);
    }


    // If the requested page is not in memory (Page Fault):
    if(nextFrameIndex == 0){
        nextFrameIndex = findFrame(pageIndex, offset, doNotEvict);
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
    uint64_t physicalAddress = translate(virtualAddress);
    PMwrite(physicalAddress, value);
    return 1;
}