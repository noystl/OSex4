#include <iostream>
#include "VirtualMemory.h"
#include "PhysicalMemory.h"

//----------------------------Private Function

uint64_t translate(uint64_t toTrans){
    uint64_t doNotEvict[TABLES_DEPTH];  //TODO: Verify that this is the right array length
    uint64_t physicalAddr;
    // TODO: Do.
    return physicalAddr;
}

int findFrameHelper(word_t& currTableIdx, int currHierLevel, const uint64_t &pageIndex,
                         uint64_t& maxFrameIdx, uint64_t& maxCyclicScore,
                         uint64_t& frameWithMaxCyclicScore)
{
    //check this table for 1st criteria:
    word_t tableEntry;
    bool isTableEmpty = true;

    for (int i = 0; i < PAGE_SIZE; ++i)
    {
        PMread(currTableIdx * PAGE_SIZE + i, &tableEntry);
        if (tableEntry != 0)
        {
            isTableEmpty = false;
            break;
        }
    }

    if (isTableEmpty)
    {
        //TODO: de-reference from parent
        //TODO: update physical address???????????????????
        return 0;
    }

    //update maxFrameIdx:
    maxFrameIdx = (maxFrameIdx < currTableIdx)? currTableIdx : maxFrameIdx;

    //update maxCyclicScore, frameWithMaxCyclicScore:
    if(currHierLevel == TABLES_DEPTH)
    {
        uint64_t frameWithCyclicScore = currTableIdx ;
        uint64_t pageWithCyclicScore = ;
        uint64_t absDist = ((pageIndex - pageWithCyclicScore) > 0) ?
                           (pageIndex - pageWithCyclicScore) : (pageWithCyclicScore - pageIndex); //abs
        uint64_t cyclicScore = ((NUM_PAGES - absDist) < absDist)  ? (NUM_PAGES - absDist) : absDist; //min
        if (cyclicScore > maxCyclicScore) //max
        {
            maxCyclicScore = cyclicScore;
            frameWithMaxCyclicScore = frameWithCyclicScore;
        }
    }

    //recursion:
    for (int i = 0; i < PAGE_SIZE; ++i)
    {
        PMread(currTableIdx * PAGE_SIZE + i, &tableEntry);
        if (tableEntry != 0)
        {
            return findFrameHelper(tableEntry, ++currHierLevel, pageIndex, maxFrameIdx,
                            maxCyclicScore, frameWithMaxCyclicScore);
        }
    }
}


uint64_t findFrame(const uint64_t& pageIndex,const uint64_t& doNotDelete){ // Gets an array of frames it shouldn't evict.
    // for 2nd criteria:
    uint64_t maxFrameIdx;
    // for 3rd criteria:
    uint64_t maxCyclicScore;
    uint64_t frameWithMaxCyclicScore;
    uint64_t pageWithMaxCyclicScore;

    word_t currTableIdx = 0;
    if (findFrameHelper(currTableIdx, 0 ,pageIndex, maxFrameIdx, maxCyclicScore,
                        frameWithMaxCyclicScore)) // couldn't answer 1st criteria
    {
        // answered 2nd criteria:
        if (maxFrameIdx + 1 < NUM_FRAMES)
        {
            return maxFrameIdx + 1;
        }
        // answered 3rd criteria:

    }

}

//----------------------------- Public Functions
void clearTable(uint64_t frameIndex) {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(frameIndex * PAGE_SIZE + i, 0);
    }
}

void VMinitialize() {
    clearTable(0);
}

int VMread(uint64_t virtualAddress, word_t* value) {
    return 1;
}


int VMwrite(uint64_t virtualAddress, word_t value) {
    return 1;
}