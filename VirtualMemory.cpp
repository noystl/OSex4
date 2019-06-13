#include <cmath>
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
//----------------------------Private Function

#define MAX(X, Y)  ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))
#define ABS(X, Y)  ((X) > (Y) ? (X-Y) : (Y-X))

/**
 * sets the table entries in the "frameIndex"ed frame to zero.
 */
void clearTable(uint64_t frameIndex) {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(frameIndex * PAGE_SIZE + i, 0);
    }
}

/**
 * dereference the current frame from it's parent frame.
 * @param currIdx: holds the index of the frame containing the table we want to dereference from parent.
 * @param parentIdx: holds the frame index of the frame containing the parent table.
 */
void dereferenceFromParent(const word_t& currIdx, const word_t& parentIdx)
{

    word_t tableEntry;
    //find reference in the parent table:
    uint64_t i;
    for (i = 0 ; i < PAGE_SIZE; ++i)
    {
        PMread((uint64_t) parentIdx * PAGE_SIZE + i, &tableEntry);
        if (tableEntry == currIdx) {
            break;
        }
    }
    //dereference:
    PMwrite(parentIdx * PAGE_SIZE + i, 0);
}


/**
 * @param frameIdx: an index representing a frame.
 * @param doNotDelete : an array of frame indexes.
 * @return: true if the frameIdx supplied appears in doNotDelete, false otherwise.

 */
bool inDoNotDelete( word_t frameIdx, const word_t (&doNotDelete)[TABLES_DEPTH])
{
 for (auto element: doNotDelete)
    {
     if (element == frameIdx) return true;
    }
    return false;
}

/**
 * implementation of the finding frame algorithm.
 * @param currFrameIdx: the current frame we're in
 * @param hierLevel: the hierarchy level of the frame in the tables tree
 * @param currParentIdx: the current frame's parent frame index
 * @param currPath: the path of the current frame
 * @param pathToFrame : will hold the path to the page
 * @param emptyFrameIdx : will hold the index of the suitable empty frame (1st criteria)
 * @param unusedFrameIdx : will hold the index of the suitable unused frame (2nd criteria)
 * @param frameIdxWithMaxCyclicScore : will hold the index of the suitable frame-  the one with the maximum cyclic score (3rd criteria)
 * @param maxCyclicScore:  carries through the recursion the maximum cyclic score, for comparison
 * @param parentIdx: will hold the outcome's parent's index
 * @param pageIndex: holds the page number we want to find.
 * @param doNotDelete: holds the indexes of the frames along the path.
 * @return: indicator suggesting to which return value to address: true for the 1st criteria holders, false otherwise.

 */
bool findFrameHelper(word_t currFrameIdx, unsigned int hierLevel, word_t currParentIdx, uint64_t currPath, uint64_t &pathToFrame,
                     word_t &emptyFrameIdx, word_t &unusedFrameIdx, word_t &frameIdxWithMaxCyclicScore,
                     uint64_t &maxCyclicScore, word_t &parentIdx,
                     const uint64_t &pageIndex, const word_t (&doNotDelete)[TABLES_DEPTH])
{

    bool isFirstFulfilled = true;
    word_t tableEntry;

    if(hierLevel == TABLES_DEPTH) // if a leaf, can't return on 1st criteria
    {
        isFirstFulfilled = false;
    }

    else{ // for inner nodes: check if 1st criteria is fulfilled:

        for (uint64_t i = 0; i < PAGE_SIZE; ++i) //stop condition: empty frame.
        {
            PMread(currFrameIdx * PAGE_SIZE + i, &tableEntry);
            if (tableEntry != 0)
            {
                isFirstFulfilled = false;
                unusedFrameIdx = MAX(unusedFrameIdx, tableEntry); //maintain 2nd criteria.
            }
        }
    }

    // Check that it's not a "false alarm"- the frame belongs to path:
    if (isFirstFulfilled)
    {
        if (!inDoNotDelete(currFrameIdx, doNotDelete))
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

        if (hierLevel == TABLES_DEPTH)
        {
            uint64_t absDist = ABS(pageIndex, currPath);
            uint64_t pageDist = MIN(NUM_PAGES - absDist, absDist);
            if(pageDist > maxCyclicScore)
            {
                maxCyclicScore = pageDist; //maintain 3rd criteria

                //prepare for successful return on 3rd criteria:
                frameIdxWithMaxCyclicScore = currFrameIdx;
                parentIdx = currParentIdx;
                pathToFrame = currPath;
            }
        }

        else
        {
            //recursion:

            uint64_t pathOfSon = currPath << (uint64_t)std::ceil((float)(VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH) / (float)TABLES_DEPTH);
            for (uint64_t i = 0; i < PAGE_SIZE; ++i)
            {
                PMread((uint64_t)currFrameIdx * PAGE_SIZE + i, &tableEntry);
                if (tableEntry != 0)
                {
                     isFirstFulfilled = isFirstFulfilled || findFrameHelper(tableEntry, hierLevel + 1, currFrameIdx, (pathOfSon | i), pathToFrame,
                                    emptyFrameIdx, unusedFrameIdx, frameIdxWithMaxCyclicScore,
                                    maxCyclicScore, parentIdx, pageIndex, doNotDelete);
                }
            }
        }
    }

    //return indicator suggesting to which return value to address:
    return isFirstFulfilled;
}


/**
 * finds the correct frame index fo assign to the page.
 * @param pageIndex : the page we want to place.
 * @param doNotDelete: a list frame indices representing the page's path
 * @return: a frame index ready to be assigned to the page
 */
word_t findFrame(const uint64_t &pageIndex, const word_t (&doNotDelete)[TABLES_DEPTH])
{
    //will contain the frames that answer thw 1st, 2nd and 3rd criteria accordingly:
    word_t emptyFrameIdx = 0, unusedFrameIdx = 0, frameIdxWithMaxCyclicScore = 0, parentIdx = 0;
    uint64_t maxCyclicScore = 0, pathToFrame = 0;

    bool isFirstFulfilled = findFrameHelper(0, 0, 0, 0, pathToFrame, emptyFrameIdx, unusedFrameIdx,
                                            frameIdxWithMaxCyclicScore, maxCyclicScore,
                                            parentIdx, pageIndex, doNotDelete);

    if (isFirstFulfilled)
    {
        dereferenceFromParent(emptyFrameIdx, parentIdx);
        return emptyFrameIdx;
    }

    if (unusedFrameIdx + 1 < NUM_FRAMES)
    {
        return unusedFrameIdx + 1;
    }
    dereferenceFromParent(frameIdxWithMaxCyclicScore, parentIdx);
    PMevict((uint64_t )frameIdxWithMaxCyclicScore, pathToFrame);
    return frameIdxWithMaxCyclicScore;
}



/**
 * Returns a "substring" of the address according to the mask, and moves it left according to the shifts.
 * Example: If address = 1000101, mask = 1110000 and shifts = 4, getSubAddress() returns 0000100.
 * @param mask: as in example above.
 * @param shifts: as in example above.
 * @return as in example above.
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
void translatePreprocess(uint64_t virtualAddress, uint64_t &offset, uint64_t &pageIndex,
                         uint64_t (&searchRows)[TABLES_DEPTH]){

    // Init offset:
    uint64_t offsetMask = createMask(OFFSET_WIDTH, 0);
    offset = getSubAddress(virtualAddress, offsetMask, 0);

    // Init pageIndex:
    int bitsForPageIndex = VIRTUAL_ADDRESS_WIDTH - OFFSET_WIDTH;
    uint64_t pageIndexMask = createMask(bitsForPageIndex, OFFSET_WIDTH);
    pageIndex = getSubAddress(virtualAddress, pageIndexMask, OFFSET_WIDTH);

    // Init searchRows:
    int bitsForRow = (int)(std::ceil( (float)bitsForPageIndex / (float)TABLES_DEPTH));
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
    word_t doNotEvict[TABLES_DEPTH] = {0};
    uint64_t usedTablesNum = 1;        // This is also the number of elements stored in doNotEvict.
    uint64_t searchRows[TABLES_DEPTH];  // Holds indexes of table entries relevant to the search.
    uint64_t offset;
    uint64_t pageIndex;
    word_t currFrameIndex = 0;          // We always start the search from frame 0.
    word_t nextFrameIndex;

    // Split the virtual address into parts and init relevant variables:
    translatePreprocess(toTrans, offset, pageIndex, searchRows);

    // Init nextFrameIndex for the first time (it's important to do it outside the loop in case we have just one level):
    PMread(currFrameIndex * PAGE_SIZE + searchRows[0], &nextFrameIndex);

    // Find the frame for the requested page:
    for(int i = 0; i < TABLES_DEPTH - 1; i++){
        // If the requested table is not in memory (Page Fault):
        if(nextFrameIndex == 0){
            // Find and init a new table:
            nextFrameIndex = findFrame(pageIndex, doNotEvict);
            clearTable((uint64_t)nextFrameIndex);
            PMwrite(currFrameIndex * PAGE_SIZE + searchRows[i], nextFrameIndex);


        }
        // Verify it won't be deleted next time we'll need an empty frame
        doNotEvict[usedTablesNum] = nextFrameIndex;
        usedTablesNum++;


        currFrameIndex = nextFrameIndex;
        PMread(currFrameIndex * PAGE_SIZE + searchRows[i + 1], &nextFrameIndex);
    }


    // If the requested page is not in memory (Page Fault):
    if(nextFrameIndex == 0){
        nextFrameIndex = findFrame(pageIndex, doNotEvict);
        PMrestore((uint64_t)nextFrameIndex, pageIndex);
        PMwrite(currFrameIndex * PAGE_SIZE + searchRows[TABLES_DEPTH - 1], nextFrameIndex);
    }

    return nextFrameIndex * PAGE_SIZE + offset;
}



//----------------------------- Public Functions

void VMinitialize() {
    clearTable(0);
}

int VMread(uint64_t virtualAddress, word_t* value) {
    if(virtualAddress >= VIRTUAL_MEMORY_SIZE){
        return 0;
    }
    uint64_t physicalAddress = translate(virtualAddress);
    PMread(physicalAddress, value);
    return 1;
}


int VMwrite(uint64_t virtualAddress, word_t value) {
    if(virtualAddress >= VIRTUAL_MEMORY_SIZE){
        return 0;
    }
    uint64_t physicalAddress = translate(virtualAddress);
    PMwrite(physicalAddress, value);
    return 1;
}

