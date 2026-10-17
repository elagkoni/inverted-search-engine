#include "utilities.h"
#include <cstring>
#include <iostream>
#include "list.h"
#include "hashtable.h"

//---------------------------------------------------------
//   Compare
//---------------------------------------------------------

int compareInts(int i, int j)
{
    if (i < j) {
        return -1;
    } else if (i == j) {
        return 0;
    } else {
        return 1;
    }
}

//---------------------------------------------------------
//   Equal
//---------------------------------------------------------

bool equalStrings(const char* strA, const char* strB)
{
    return strcmp(strA, strB) == 0;
}

bool equalIntegers(unsigned int i, unsigned int j)
{
    return i == j;
}

//---------------------------------------------------------
//   Matching
//---------------------------------------------------------

int exactMatching(const char* strA, const char* strB)
{
    return strcmp(strA, strB);
}

int hammingDistanceMatching(const char* strA, const char* strB)
{
    size_t length = strlen(strA);

    if (length != strlen(strB)) { /* TODO: possible improvement, calculate the lengths once and pass as an argument */
        return -1;
    }

    unsigned int result = 0;
    for (size_t i = 0; i < length; i++) {
        if (strA[i] != strB[i]) {
            result++;
        }
    }

    return result;
}

int editDistanceMatching(const char* strA, const char* strB)
{
    // Calculating the edit distance of two strings is like building a matrix of values
    // which represent the various inserts, deletions and substitutions required.
    // To find the value at a specific row position we need only three values
    // - the one to the left    (previousColumn)
    // - the one directly above (previousRow)
    // - the last one diagonal  (previousDiagonal)
    //
    //   -----------------------------------------
    //   | substitution cost |   deletion cost   |
    //   |---------------------------------------|
    //   |   insertion cost  |     MIN of 3      |
    //   -----------------------------------------
    //

    size_t lenA = strlen(strA), lenB = strlen(strB);
    int previousDiagonal, previousColumn, indicator;
    int previousRow[lenB + 1];  // array to hold our costs

    auto min3 = [](int x, int y, int z) { return std::min(x, std::min(y, z)); };

    if (lenA == 0) {
        return lenB;
    }
    if (lenB == 0) {
        return lenA;
    }
    if (exactMatching(strA, strB) == 0) {
        return 0;
    }

    // setting the first row
    for (size_t j = 0; j <= lenB; j++) {
        previousRow[j] = j;
    }

    for (size_t i = 1; i <= lenA; i++) {
        previousDiagonal = previousRow[0];      // last substitution cost
        previousColumn   = previousRow[0] + 1;  // last insert cost (starts one higher than our substitution cost)

        for (size_t j = 1; j <= lenB; j++) {
            indicator = strA[i - 1] == strB[j - 1] ? 0 : 1;

            previousColumn = min3(previousColumn + 1,               // insertion
                                  previousRow[j] + 1,               // deletion
                                  previousDiagonal + indicator);    // substitution
            previousDiagonal = previousRow[j];
            previousRow[j] = previousColumn;    // current result here
        }
        previousRow[0] = i; // update the start of each logical row
    }

    return previousRow[lenB];
}

//---------------------------------------------------------
//   Deduplicate
///     Caller takes ownership of returned value.
//---------------------------------------------------------

List<const char*>* deduplicate(const char* words)
{
    List<const char*>* wordsList = new List<const char*>();
    Hashtable<const char*, int> map(100, equalStrings);

    char* rest;
    char* copy = strdup(words);
    char* token = strtok_r(copy, " ", &rest);

    while(token != NULL) {
        if (map.get(token) == nullptr) {
            map.insert(strdup(token), 0);
        }

        /* get next word */
        token = strtok_r(NULL, " ", &rest);
    }

    for (auto& x : map.keys()) {
        wordsList->append(x);
    }

    free(copy);
    return wordsList;
}

//---------------------------------------------------------
//   Hash Function
//---------------------------------------------------------

//
/// Copy & Paste from http://www.cse.yorku.ca/~oz/hash.html
//
unsigned long djb2(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}
