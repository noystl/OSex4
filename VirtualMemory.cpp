#include <cmath>
#include <iterator>
#include <algorithm>
#include <iostream>
#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include "FlowExampleSimulation.cpp"
//----------------------------Private Function




////EVYATAR

////
//// Created by evyatar100 on 5/27/19.
//// ver 3.0
////


#include <iomanip>
#include <vector>


using namespace std;

/**
 * fill the physical memory with zeros
 */
void resetRAM();

/**
 * validate that the physical memory is equal to expectedRAM. if it does not, halt's the script
 */
void assertRAM(std::vector<word_t> expectedRAM);

/**
 * print's the virtual memory tree. feel free to use this function is Virtual Memory for debuging.
 */
void printTree();

/**
 * print's the virtual memory tree. feel free to use this function is Virtual Memory for debuging.
 */
void printRAM();

/**
 * get a copy of the current physical memory
 */
std::vector<word_t> getRam();

void printRAMSimple();

std::vector<word_t> state0 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
std::vector<word_t> state1 = {1, 0, 0, 2, 0, 3, 4, 0, 0, 3, 0, 0, 0, 0, 0, 0};
std::vector<word_t> state2 = {1, 0, 5, 2, 0, 3, 4, 0, 0, 3, 0, 6, 0, 7, 0, 0};
std::vector<word_t> state3 = {1, 4, 5, 0, 0, 7, 0, 2, 0, 3, 0, 6, 0, 0, 0, 0};
std::vector<word_t> state4 = {1, 4, 0, 6, 0, 0, 0, 2, 0, 3, 7, 0, 0, 5, 0, 3};
std::vector<word_t> state5 = {1, 4, 0, 6, 0, 7, 0, 2, 0, 3, 0, 0, 0, 5, 0, 99};

static bool printType = false; // You may change this to True if you want.

//int main(int argc, char **argv)
//{
//    if (OFFSET_WIDTH != 1 or PAGE_SIZE != 2 or RAM_SIZE != 16 or VIRTUAL_ADDRESS_WIDTH != 5 or
//        VIRTUAL_MEMORY_SIZE != 32)
//    {
//        cout <<
//             "please override MemoryConstanrs.h with the version provided with the tester.\n";
//        exit(2);
//    }
//
//    cout <<
//         "Virtual Memory size is 32 = 2^5\n"
//         "Page/Frame/Table size is 2 = 2^1\n"
//         "There are 4 layers is the tree\n"
//         "Ram size is 16\n\n" << endl;
//
//    word_t word;
//    resetRAM();
//    VMinitialize();
//
//	printRAM();
//	printTree();
//	cout <<
//		 "State 0, after calling:\nVMwrite(13, 3)" << endl;
//	assertRAM(state0);
//
//    VMwrite(13, 3);
//	printRAM();
//	printTree();
//	cout <<
//		 "State 1, after calling:\nVMwrite(13, 3)" << endl;
//	assertRAM(state1);
//
//
//	VMread(13, &word);
//	printRAM();
//	printTree();
//	cout <<
//		 "State 1a, after calling:\nVMwrite(13, 3)\nVMread(13, &word)" << endl;
//	assertRAM(state1);
//	if (word != 3)
//	{
//		cout
//				<< "After the call VMwrite(13, 3) and then VMread(13, &word), word should be equal to 3 but"
//				   " word = " << word << endl;
//		cout << "Test failed. Aborting..." << endl;
//		exit(1);
//	}
//
//    VMread(6, &word);
//	printRAM();
//	printTree();
//	cout <<
//		 "State 2, after calling:\nVMwrite(13, 3)\nVMread(13, &word)\nVMread(6, &word)" << endl;
//	assertRAM(state2);
//
//
//    VMread(31, &word);
//    printRAM();
//    printTree();
//    cout <<
//         "State 3, after calling:\nVMwrite(13, 3)\nVMread(13, &word)\nVMread(6, &word)\nVMread(31, &word)"
//         << endl;
//    assertRAM(state3);
//
//    VMread(13, &word);
//    printRAM();
//    printTree();
//    cout <<
//         "State 4, after calling:\nVMwrite(13, 3)\nVMread(13, &word)\nVMread(6, &word)\nVMread(31, &word)\nVMread(13, &word)"
//         << endl;
//    assertRAM(state4);
//
//    if (word != 3)
//    {
//        cout
//                << "At this point, word should be equal to 3 but"
//                   " word = " << word << endl;
//        cout << "Test failed. Aborting..." << endl;
//        exit(1);
//    }
//
//std::cout << "FROM HERE\n";
//
//    VMwrite(31, 99);
//    printRAM();
//    printTree();
//    cout <<
//         "State 5, after calling:\nVMwrite(13, 3)\nVMread(13, &word)\nVMread(6, &word)\nVMread(31, &word)\nVMread(13, &word)\nVMwrite(31, 99)"
//         << endl;
//    assertRAM(state5);
//
//    cout << "\n\n$$$ Test passed $$$" << endl;
//    return 0;
//}


// ---------------------------------------- boring stuff ---------------------------------------------------
/**
 * fill the physical memory with zeros
 */
void resetRAM()
{
    std::vector<word_t> RAM(NUM_FRAMES * PAGE_SIZE);
    for (uint64_t i = 0; i < RAM_SIZE; i++)
    {
        PMwrite(i, 0);
    }
}


/**
 * validate that the physical memory is equal to expectedRAM. if it does not, halt's the script
 */
void assertRAM(std::vector<word_t> expectedRAM)
{
    std::vector<word_t> actualRAM = getRam();
    for (uint64_t i = 0; i < RAM_SIZE; i++)
    {
        if (actualRAM[i] != expectedRAM[i])
        {
            cout << "Your physical memory is not in its required state. See FlowExample.pdf" << endl;
            cout << "Test failed. Aborting..." << endl;
            exit(1);
        }
    }
    cout << "Your physical memory state is correct." << endl;
}

/*
 * helper for printTree
 */
void printSubTree(uint64_t root, int depth, bool isEmptyMode)
{
    if (depth == TABLES_DEPTH)
    {
        return;
    }
    word_t currValue = 0;

    if ((isEmptyMode || root == 0) && depth != 0)
    {
        isEmptyMode = true;
    }

    //right son
    PMread(root * PAGE_SIZE + 1, &currValue);
    printSubTree(static_cast<uint64_t>(currValue), depth + 1, isEmptyMode);

    //father
    for (int _ = 0; _ < depth; _++)
    {
        std::cout << '\t';
    }
    if (isEmptyMode)
    {
        std::cout << '_' << '\n';
    } else
    {
        if (depth == TABLES_DEPTH - 1)
        {
            word_t a, b;
            PMread(root * PAGE_SIZE + 0, &a);
            PMread(root * PAGE_SIZE + 1, &b);
            std::cout << root << " -> (" << a << ',' << b << ")\n";
        } else
        {
            std::cout << root << '\n';
        }
    }

    //left son
    PMread(root
           * PAGE_SIZE + 0, &currValue);
    printSubTree(static_cast <uint64_t>(currValue), depth + 1, isEmptyMode);
}

/**
 * print's the virtual memory tree. feel free to use this function is Virtual Memory for debuging.
 */
void printTree()
{
    std::cout << "---------------------" << '\n';
    std::cout << "Virtual Memory:" << '\n';
    printSubTree(0, 0, false);
    std::cout << "---------------------" << '\n';
}

/**
 * get a copy of the current physical memory
 */
std::vector<word_t> getRam()
{
    std::vector<word_t> RAM(RAM_SIZE);
    word_t tempWord;
    for (uint64_t i = 0; i < NUM_FRAMES; i++)
    {
        for (uint64_t j = 0; j < PAGE_SIZE; j++)
        {
            PMread(i * PAGE_SIZE + j, &tempWord);
            RAM[i * PAGE_SIZE + j] = tempWord;
        }
    }
    return RAM;
}


/**
 * print the current state of the pysical memory. feel free to use this function is Virtual Memory for debuging.
 */
void printRAM()
{
    std::cout << "---------------------" << '\n';
    std::cout << "Physical Memory:" << '\n';
    std::vector<word_t> RAM = getRam();

    if (printType)
    {
        for (uint64_t i = 0; i < NUM_FRAMES; i++)
        {
            std::cout << "frame " << i << ":\n";
            for (uint64_t j = 0; j < PAGE_SIZE; j++)
            {
                std::cout << "(" << j << ") " << RAM[i * PAGE_SIZE + j] << "\n";
            }
            std::cout << "-----------" << "\n";
        }
    } else
    {

        //std::cout << "FRAME INDICES -\t";
//        for (uint64_t i = 0; i < NUM_FRAMES; i++)
//        {
//            std::cout << "F" << i << ": (";
//            for (uint64_t j = 0; j < PAGE_SIZE - 1; j++)
//            {
//                std::cout << j << ",\t";
//            }
//            std::cout << PAGE_SIZE - 1 << ")\t";
//        }
//        std::cout << '\n';
//        std::cout << "DATA -\t\t\t";
        for (uint64_t i = 0; i < NUM_FRAMES; i++)
        {
            std::cout << "F" << i << ": (";

            for (uint64_t j = 0; j < PAGE_SIZE - 1; j++)
            {
                std::cout << RAM[i * PAGE_SIZE + j] << ",\t";
            }
            std::cout << RAM[i * PAGE_SIZE + PAGE_SIZE - 1] << ") \n";

        }
        std::cout << '\n';
    }

    std::cout << "---------------------" << '\n';
}


/*
 * print  the current state of the pysical memory.
 * the print will look like this { 1, 4, 0, 7, 0, 5, 0, 2, 0, 3, 0, 99, 0, 0, 0, 6}. to create state.
 */
void printRAMSimple()
{
    std::vector<word_t> RAM = getRam();
    std::cout << "{ ";
    for (uint64_t i = 0; i < RAM_SIZE - 1; i++)
    {
        std::cout << RAM[i] << ", ";
    }
    std::cout << RAM[RAM.size() - 1] << "}" << "\n";
}




//////////////////////------------------------------------CODE:
#define MAX(X, Y)  ((X) > (Y) ? (X) : (Y))
#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))
#define ABS(X)  ((X) < 0 ? (-X) : (X))



void clearTable(uint64_t frameIndex) {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite(frameIndex * PAGE_SIZE + i, 0);
    }
}

void dereferenceFromParent(const word_t& currIdx, const word_t& parentIdx)
{

    //std::cout << "currFrame: " << currIdx << ", currParent: " << parentIdx << std::endl;
    word_t tableEntry;
    //find offset:
    uint64_t i;
    for (i = 0 ; i < PAGE_SIZE; ++i)
    {
        //std::cout << "PMread input(line114): " << parentIdx * PAGE_SIZE + i << std::endl;
        PMread((uint64_t) parentIdx * PAGE_SIZE + i, &tableEntry);
        if (tableEntry == currIdx) {
            break;
        }
    }
    //std::cout << parentIdx * PAGE_SIZE + i << std::endl;
    //dereference:
    PMwrite(parentIdx * PAGE_SIZE + i, 0);
}

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
            //std::cout << "PMread input(line34): " << currFrameIdx * PAGE_SIZE + i << std::endl;
            //std::cout << "currFrameIndex: " << currFrameIdx << std::endl;
            PMread(currFrameIdx * PAGE_SIZE + i, &tableEntry);
            if (tableEntry != 0)
            {
                isFirstFulfilled = false;
                unusedFrameIdx = MAX(unusedFrameIdx, tableEntry); //maintain 2nd criteria.
                //std::cout << " max frame: " << unusedFrameIdx<< std::endl;

            }
        }
    }


    //std::cout << "max until now: " << unusedFrameIdx << std::endl;

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

        if (hierLevel == TABLES_DEPTH)
        {
            //std::cout << "I'm: " << currFrameIdx <<   " hier: " << hierLevel << std::endl;

            uint64_t absDist = ABS(pageIndex - currPath);
            uint64_t pageDist = MIN(NUM_PAGES - absDist, absDist);
            if(pageDist > maxCyclicScore)
            {
                maxCyclicScore = pageDist; //maintain 3rd criteria

                //prepare for successful return on 3rd criteria:
                frameIdxWithMaxCyclicScore = currFrameIdx;
                parentIdx = currParentIdx;
                pathToFrame = currPath;
                //std::cout << "pathToFrame in helper: " << pathToFrame << std::endl;
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



word_t findFrame(const uint64_t &pageIndex, const word_t (&doNotDelete)[TABLES_DEPTH])
{
    //will contain the frames that answer thw 1st, 2nd and 3rd criteria accordingly:
    word_t emptyFrameIdx = 0, unusedFrameIdx = 0, frameIdxWithMaxCyclicScore = 0;
    uint64_t maxCyclicScore = 0;
    word_t parentIdx = 0;
    uint64_t pathToFrame = 0;

    bool isFirstFulfilled = findFrameHelper(0, 0, 0, 0, pathToFrame, emptyFrameIdx, unusedFrameIdx,
                                            frameIdxWithMaxCyclicScore, maxCyclicScore,
                                            parentIdx, pageIndex, doNotDelete);
//    printRAM();
//    printTree();

    if (isFirstFulfilled)
    {

        //std::cout << "EMPTY" <<std::endl;

        dereferenceFromParent(emptyFrameIdx, parentIdx);
        return emptyFrameIdx;
    }

    if (unusedFrameIdx + 1 < NUM_FRAMES)
    {
        //std::cout << "MAXED" <<std::endl;

        return unusedFrameIdx + 1;
    }
    //std::cout << "max: " << unusedFrameIdx << std::endl;

    //std::cout << "SWAPPED" <<std::endl;
    //std::cout << "evict: " << pathToFrame << " from: " << frameIdxWithMaxCyclicScore << std::endl;

    dereferenceFromParent(frameIdxWithMaxCyclicScore, parentIdx);
    PMevict(frameIdxWithMaxCyclicScore, pathToFrame);
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
void translatePreproccess(uint64_t virtualAddress, uint64_t& offset, uint64_t& pageIndex,
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
    word_t doNotEvict[TABLES_DEPTH] = {0};
    uint64_t usedTablesNum = 1;        // This is also the number of elements stored in doNotEvict.
    uint64_t searchRows[TABLES_DEPTH];  // Holds indexes of table entries relevant to the search.
    uint64_t offset;
    uint64_t pageIndex;
    word_t currFrameIndex = 0;          // We always start the search from frame 0.
    word_t nextFrameIndex;

    // Split the virtual address into parts and init relevant variables:
    translatePreproccess(toTrans, offset, pageIndex, searchRows);

    // Init nextFrameIndex for the first time (it's important to do it outside the loop in case we have just one level):
    //std::cout << "PMread input(line239): " << currFrameIndex * PAGE_SIZE + searchRows[0] << std::endl;
    PMread(currFrameIndex * PAGE_SIZE + searchRows[0], &nextFrameIndex);

    // Find the frame for the requested page:
    for(int i = 0; i < TABLES_DEPTH - 1; i++){
        // If the requested table is not in memory (Page Fault):
        if(nextFrameIndex == 0){
            // Find and init a new table:
            nextFrameIndex = findFrame(pageIndex, doNotEvict);
            //std::cout << "findFrame output: " << nextFrameIndex << std::endl;
            clearTable(nextFrameIndex);
            PMwrite(currFrameIndex * PAGE_SIZE + searchRows[i], nextFrameIndex);


        }
// Verify it won't be deleted next time we'll need an empty frame
        doNotEvict[usedTablesNum] = nextFrameIndex;
        usedTablesNum++;


        currFrameIndex = nextFrameIndex;
        //std::cout << "PMread input(line258): " << currFrameIndex * PAGE_SIZE + searchRows[0] << std::endl;
        PMread(currFrameIndex * PAGE_SIZE + searchRows[i + 1], &nextFrameIndex);
    }


    // If the requested page is not in memory (Page Fault):
    if(nextFrameIndex == 0){
        nextFrameIndex = findFrame(pageIndex, doNotEvict);
        //std::cout << "findFrame output: " << nextFrameIndex << std::endl;
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
    uint64_t physicalAddress = translate(virtualAddress);
    PMread(physicalAddress, value);
//    std::cout << "Reading from: " << physicalAddress << " this value: " << *value << std::endl;
    return 1;
}


int VMwrite(uint64_t virtualAddress, word_t value) {
    uint64_t physicalAddress = translate(virtualAddress);
    PMwrite(physicalAddress, value);
    //std::cout << "Writing to: " << physicalAddress << " this value: " << value << std::endl;
    return 1;
}





//// Noy
//int main(){
//    uint64_t virtAddress = 1302; // (10100010110)
//    word_t temp;
//    VMwrite(virtAddress, 1);
//    VMread(virtAddress, &temp);
//
//    uint64_t virtAddress1 = 123; // (00001111011)
//    word_t temp1;
//    VMwrite(virtAddress1, 1);
//    VMread(virtAddress1, &temp1);
//
//    return 0;
//}



//////Simple
//#include <cstdio>
//#include <cassert>
//
//int main(int argc, char **argv) {
//    VMinitialize();
//    for (uint64_t i = 0; i < (2 * NUM_FRAMES); ++i) {
//        printf("writing to %llu: %d\n", (long long int) i, (word_t)i);
//        //std::cout << "virtAddr: " << 5 * i * PAGE_SIZE << std::endl;
//        VMwrite(5 * i * PAGE_SIZE, (word_t)i);
//    }
//
//    for (uint64_t i = 0; i < (2 * NUM_FRAMES) ; ++i) {
//        word_t value;
//        //std::cout << "virtAddr: " << 5 * i * PAGE_SIZE << std::endl;
//        VMread(5 * i * PAGE_SIZE, &value);
//        printf("reading from %llu: %d\n", (long long int) i, value);
//        assert(uint64_t(value) == i);
//    }
//    printf("success\n");
//
//    return 0;
//}


////Simpler Simple:
//#include "VirtualMemory.h"
//#include <cstdio>
//#include <cassert>
//// NUM_FRAMES = 64, TABLES_DEPTH = 4, ROWS = 16
//
//int main(int argc, char **argv) {
//    VMinitialize();
//
//    for (uint64_t i = 0; i < 47; ++i) {
//        std::cout << "writing to " <<  i << ": "  << i << std::endl;
//        VMwrite(5 * i * PAGE_SIZE, (word_t)i);
//    }
//
//    for (uint64_t i = 0; i < 47; ++i) {
//        word_t value;
//
//        VMread(5 * i * PAGE_SIZE, &value);
//        std::cout << "reading from " <<  i << ": "  << value << std::endl;
//
//
//        assert(uint64_t(value) == i);
//    }
//    printf("success\n");
//
//    return 0;
//}


//// Example:
//#include <cassert>
//
//int main(int argc, char **argv) {
//    VMinitialize();
//    VMwrite(13,3);
//    word_t value0;
//    VMread(13,&value0);
//    assert(value0 == 3);
//
//
//    word_t value;
//    VMread(6,&value);
//
//    std::cout << "From HERE" << std::endl;
//
//    word_t value2;
//    VMread(31,&value2);
//
//    return 0;
//}


