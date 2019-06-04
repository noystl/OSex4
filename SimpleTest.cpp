#include "VirtualMemory.h"
//#include <cstdio>
//#include <cassert>
//
//int main(int argc, char **argv) {
//    VMinitialize();
//    for (uint64_t i = 0; i < 2; ++i) {
//        printf("writing to %llu: %d\n", (long long int) i, (word_t)i);
//        VMwrite(5 * i * PAGE_SIZE, (word_t)i);
//    }
//
//    for (uint64_t i = 0; i < 2; ++i) {
//        word_t value;
//        VMread(5 * i * PAGE_SIZE, &value);
//        printf("reading from %llu: %d\n", (long long int) i, value);
//        assert(uint64_t(value) == i);
//    }
//    printf("success\n");
//
//    return 0;
//}



/// N's tests

/// GetSubAddress() Test:


/// input is: address = 1001000 (=72 in decimal), mask = 1110000 (=112 in decimal), shifts = 4.
/// output should be: 0000100 (=4 in decimal)

//#include <iostream>
//
//uint64_t getSubAddress(uint64_t  address, uint64_t mask, int shifts){
//    // Apply mask:
//    uint64_t temp = address & mask;
//
//    // Shift as needed:
//    temp = temp >> shifts;
//
//    return temp;
//}
//
//int main(){
//    std::cout << getSubAddress(72, 112, 4) << std::endl;
//    return 0;
//}


/// CreateMask() Test
/// Input is: length = 2, pos = 2. The output should be 12 (1100).

//#include <cstdint>
//#include <iostream>
//
//uint64_t createMask(int length, int position){
//    uint64_t mask = 0;
//    uint64_t oneBit = 1;                // in binary 0000000.....0001
//
//    // Turn on the correct number of bits:
//    for(int i = 0; i < length; i++){
//        mask += oneBit;
//        oneBit = oneBit << 1;
//    }
//
//    // Move the bits to the correct place:
//    mask = mask << position;
//
//    return mask;
//}
//
//
//int main(){
//    std::cout << createMask(64, 0)  << std::endl;
//    return 0;
//}




/// translatePreprocess() Test:
//// Tested with consts: OFFSET_WIDTH 4, PHYSICAL_ADDRESS_WIDTH 10, VIRTUAL_ADDRESS_WIDTH 11
//// The result should be: offset = 6 (=0110), pageIndex = 81 (=1010001), row0 = 5 (=101), row1 = 1 (=0001).
//
//#include <cstdint>
//#include <cmath>
//#include <iostream>
//#include "MemoryConstants.h"
//
//
//uint64_t getSubAddress(uint64_t  address, uint64_t mask, int shifts){
//    // Apply mask:
//    uint64_t temp = address & mask;
//
//    // Shift as needed:
//    temp = temp >> shifts;
//
//    return temp;
//}
//
//
//uint64_t createMask(int length, int position){
//    uint64_t mask = 0;
//    uint64_t oneBit = 1;                // in binary 0000000.....0001
//
//    // Turn on the correct number of bits:
//    for(int i = 0; i < length; i++){
//        mask += oneBit;
//        oneBit = oneBit << 1;
//    }
//
//    // Move the bits to the correct place:
//    mask = mask << position;
//
//    return mask;
//}
//
//
//void translatePreprocces(uint64_t virtualAddress, uint64_t& offset, uint64_t& pageIndex,
//                         uint64_t (&searchRows)[TABLES_DEPTH]){
//
//    // Init offset:
//    uint64_t offsetMask = createMask(OFFSET_WIDTH, 0);
//    offset = getSubAddress(virtualAddress, offsetMask, 0);
//
//    // Init pageIndex:
//    int bitsForPageIndex = VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH;
//    uint64_t pageIndexMask = createMask(bitsForPageIndex, OFFSET_WIDTH);
//    pageIndex = getSubAddress(virtualAddress, pageIndexMask, OFFSET_WIDTH);
//
//    // Init searchRows:
//    int bitsForRow = std::ceil( (float)bitsForPageIndex / (float)TABLES_DEPTH);
//    uint64_t currRowMask = createMask(bitsForRow, 0);
//
//    for(int i = 0; i < TABLES_DEPTH; i++){
//        searchRows[(TABLES_DEPTH - 1) - i] = getSubAddress(pageIndex, currRowMask, i*bitsForRow);
//        currRowMask = currRowMask << bitsForRow;
//    }
//}
//
//int main(){
//    uint64_t virtAddress = 1302; // (10100010110)
//    uint64_t searchRows[TABLES_DEPTH];  // Holds indexes of table entries relevant to the search.
//    uint64_t offset;
//    uint64_t pageIndex;
//
//    translatePreprocces(virtAddress, offset, pageIndex, searchRows);
//
//    std::cout << "offset: " << offset << std::endl;
//    std::cout << "pageIndex: " << pageIndex << std::endl;
//    std::cout << "number of levels " << TABLES_DEPTH << std::endl;
//
//    for(int i = 0; i < TABLES_DEPTH; i++){
//        std::cout << "row Index " << i  << " " << searchRows[i] << std::endl;
//    }
//
//    return 0;
//}


/// Translate() test
/// Tested with consts: OFFSET_WIDTH 4, PHYSICAL_ADDRESS_WIDTH 10, VIRTUAL_ADDRESS_WIDTH 11
/// The output should be 38 since: frame 0 -> frame 1 -> frame 2 (the actual page). Each frame is 16 words long and the
/// offset is 6. So: 16 + 16 + 6 = 38.
//
//#include <cstdint>
//#include <cmath>
//#include <iostream>
//#include <algorithm>
//#include "MemoryConstants.h"
//#include "PhysicalMemory.h"
//
//void clearTable(uint64_t frameIndex) {
//    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
//        PMwrite(frameIndex * PAGE_SIZE + i, 0);
//    }
//}
//
//#define MAX(X, Y)  ((X) > (Y) ? (X) : (Y))
//#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))
//#define ABS(X)  ((X) < 0 ? (-X) : (X))
//
//
//
//bool findFrameHelper(word_t currFrameIdx, unsigned int hierLevel, word_t currParentIdx, uint64_t pathToFrame,
//                     word_t &emptyFrameIdx, word_t &unusedFrameIdx, word_t &frameIdxWithMaxCyclicScore,
//                     uint64_t &maxCyclicScore, word_t &parentIdx,
//                     const uint64_t &pageIndex, const uint64_t (&doNotDelete)[TABLES_DEPTH])
//{
//    bool isFirstFulfilled = true;
//    word_t tableEntry;
//
//    // Check if 1st criteria is fulfilled:
//    for (uint64_t i = 0; i < PAGE_SIZE; ++i) //stop condition: empty frame. TODO: check bounds
//    {
//        PMread(currFrameIdx * PAGE_SIZE + i, &tableEntry);
//        if (tableEntry != 0)
//        {
//            isFirstFulfilled = false;
//            unusedFrameIdx = MAX(unusedFrameIdx,tableEntry); //maintain 2nd criteria. TODO: what if we're in a leaf?
//        }
//    }
//
//    // Check that it's not a "false alarm"- the frame belongs to path:
//    if (isFirstFulfilled)
//    {
//        if (std::find(std::begin(doNotDelete), std::end(doNotDelete), currFrameIdx) == std::end(doNotDelete))
//        {
//            //prepare for successful return on 1st criteria:
//            parentIdx = currParentIdx;
//            emptyFrameIdx = currFrameIdx;
//        }
//        else{
//            isFirstFulfilled = false;
//        }
//    }
//
//    // Respond accordingly:
//    if (!isFirstFulfilled)
//    {
//        // stop condition: leaf.
//        if (hierLevel == TABLES_DEPTH + 1) //TODO: check bounds
//        {
//            uint64_t absDist = ABS(pageIndex - pathToFrame);
//            uint64_t pageDist = MIN(NUM_PAGES - absDist, absDist);
//            if(pageDist > maxCyclicScore)
//            {
//                maxCyclicScore = pageDist; //maintain 3rd criteria
//
//                //prepare for successful return on 3rd criteria:
//                frameIdxWithMaxCyclicScore = currFrameIdx;
//                parentIdx = currParentIdx;
//            }
//        }
//        else
//        {
//            //recursion:
//
//            uint64_t pathOfSon = pathToFrame << (uint64_t)std::ceil((float)(VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH) / (float)TABLES_DEPTH);
//
//            for (uint64_t i = 0; i < PAGE_SIZE; ++i) //TODO: check bounds
//            {
//                PMread((uint64_t)currFrameIdx * PAGE_SIZE + i, &tableEntry);
//                if (tableEntry != 0)
//                {
//
//                    return findFrameHelper(tableEntry, hierLevel + 1, currFrameIdx, (pathOfSon | i),
//                                           emptyFrameIdx, unusedFrameIdx, frameIdxWithMaxCyclicScore,
//                                           maxCyclicScore, parentIdx, pageIndex, doNotDelete);
//                }
//            }
//        }
//    }
//
//    //return indicator suggesting to which return value to address:
//    return isFirstFulfilled;
//}
//
//void dereferenceFromParent(const word_t& currIdx, const word_t& parentIdx)
//{
//    word_t tableEntry;
//    //find offset:
//    uint64_t i;
//    for (i = 0 ; i < PAGE_SIZE; ++i) //TODO: check bounds
//    {
//        PMread((uint64_t) parentIdx * PAGE_SIZE + i, &tableEntry);
//        if (tableEntry == currIdx) {
//            break;
//        }
//    }
//    //dereference:
//    PMwrite(parentIdx * PAGE_SIZE + i, 0);
//}
//
//word_t findFrame(const uint64_t &pageIndex, const uint64_t (&doNotDelete)[TABLES_DEPTH])
//{
//    //will contain the frames that answer thw 1st, 2nd and 3rd criteria accordingly:
//    word_t emptyFrameIdx = 0, unusedFrameIdx = 0, frameIdxWithMaxCyclicScore = 0;
//    uint64_t maxCyclicScore = 0;
//    word_t parentIdx = 0;
//
//    bool isFirstFulfilled = findFrameHelper(0, 0, 0, 0, emptyFrameIdx, unusedFrameIdx,
//                                            frameIdxWithMaxCyclicScore, maxCyclicScore,
//                                            parentIdx, pageIndex, doNotDelete);
//    if (isFirstFulfilled)
//    {
//        dereferenceFromParent(parentIdx, emptyFrameIdx);
//        return emptyFrameIdx;
//    }
//    if (unusedFrameIdx + 1 < NUM_FRAMES)
//    {
//        return unusedFrameIdx + 1;
//    }
//    dereferenceFromParent(parentIdx, frameIdxWithMaxCyclicScore);
//    return frameIdxWithMaxCyclicScore;
//}
//
//
//uint64_t getSubAddress(uint64_t  address, uint64_t mask, int shifts){
//    // Apply mask:
//    uint64_t temp = address & mask;
//
//    // Shift as needed:
//    temp = temp >> shifts;
//
//    return temp;
//}
//
//
//uint64_t createMask(int length, int position){
//    uint64_t mask = 0;
//    uint64_t oneBit = 1;                // in binary 0000000.....0001
//
//    // Turn on the correct number of bits:
//    for(int i = 0; i < length; i++){
//        mask += oneBit;
//        oneBit = oneBit << 1;
//    }
//
//    // Move the bits to the correct place:
//    mask = mask << position;
//
//    return mask;
//}
//
//
//void translatePreprocces(uint64_t virtualAddress, uint64_t& offset, uint64_t& pageIndex,
//                         uint64_t (&searchRows)[TABLES_DEPTH]){
//
//    // Init offset:
//    uint64_t offsetMask = createMask(OFFSET_WIDTH, 0);
//    offset = getSubAddress(virtualAddress, offsetMask, 0);
//
//    // Init pageIndex:
//    int bitsForPageIndex = VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH;
//    uint64_t pageIndexMask = createMask(bitsForPageIndex, OFFSET_WIDTH);
//    pageIndex = getSubAddress(virtualAddress, pageIndexMask, OFFSET_WIDTH);
//
//    // Init searchRows:
//    int bitsForRow = std::ceil( (float)bitsForPageIndex / (float)TABLES_DEPTH);
//    uint64_t currRowMask = createMask(bitsForRow, 0);
//
//    for(int i = 0; i < TABLES_DEPTH; i++){
//        searchRows[(TABLES_DEPTH - 1) - i] = getSubAddress(pageIndex, currRowMask, i*bitsForRow);
//        currRowMask = currRowMask << bitsForRow;
//    }
//}
//
//
//uint64_t translate(uint64_t toTrans){
//    uint64_t doNotEvict[TABLES_DEPTH] = {0};
//    uint64_t usedTablesNum = 1;        // This is also the number of elements stored in doNotEvict.
//    uint64_t searchRows[TABLES_DEPTH];  // Holds indexes of table entries relevant to the search.
//    uint64_t offset;
//    uint64_t pageIndex;
//    word_t currFrameIndex = 0;          // We always start the search from frame 0.
//    word_t nextFrameIndex;
//
//    // Split the virtual address into parts and init relevant variables:
//    translatePreprocces(toTrans, offset, pageIndex, searchRows);
//
//    // Init nextFrameIndex for the first time (it's important to do it outside the loop in case we have just one level):
//    PMread(currFrameIndex * PAGE_SIZE + searchRows[0], &nextFrameIndex);
//
//    // Find the frame for the requested page:
//    for(int i = 0; i < TABLES_DEPTH - 1; i++){
//        // If the requested table is not in memory (Page Fault):
//        if(nextFrameIndex == 0){
//            // Find and init a new table:
//            nextFrameIndex = (word_t)findFrame(pageIndex, doNotEvict);
//            std::cout << "findFrame output " << nextFrameIndex << std::endl;
//            clearTable(nextFrameIndex);
//            PMwrite(currFrameIndex * PAGE_SIZE + searchRows[i], nextFrameIndex);
//
//            // Verify it won't be deleted next time we'll need an empty frame:
//            doNotEvict[usedTablesNum] = nextFrameIndex;
//            usedTablesNum++;
//        }
//
//        currFrameIndex = nextFrameIndex;
//        PMread(currFrameIndex * PAGE_SIZE + searchRows[i + 1], &nextFrameIndex);
//    }
//
//
//    // If the requested page is not in memory (Page Fault):
//    if(nextFrameIndex == 0){
//        nextFrameIndex = (word_t)findFrame(pageIndex, doNotEvict);
//        std::cout << "findFrame output " << nextFrameIndex << std::endl;
//        PMrestore(nextFrameIndex, pageIndex);
//        PMwrite(currFrameIndex * PAGE_SIZE + searchRows[TABLES_DEPTH - 1], nextFrameIndex);
//    }
//
//    return nextFrameIndex * PAGE_SIZE + offset;
//}
//
//
//int main(){
//    uint64_t virtAddress = 1302; // (10100010110)
////    uint64_t virtAddress  = 22; // (00010110)
//    std::cout << translate(virtAddress) << std::endl;
//
//    return 0;
//}