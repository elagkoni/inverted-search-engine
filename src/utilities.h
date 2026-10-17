#ifndef UTILITIES_H
#define UTILITIES_H

//---------------------------------------------------------
//   Compare
//---------------------------------------------------------

template<typename T> using Compare = int (*)(T, T);

int compareInts(int i, int j);

//---------------------------------------------------------
//   Equal
//---------------------------------------------------------

template<typename T> using Equal = bool (*)(T, T);

bool equalStrings(const char* strA, const char* strB);
bool equalIntegers(unsigned int i, unsigned int j);

//---------------------------------------------------------
//   Matching
//---------------------------------------------------------

using Matching = int (*)(const char*, const char*);

int exactMatching(const char* strA, const char* strB);
int hammingDistanceMatching(const char* strA, const char* strB);
int editDistanceMatching(const char* strA, const char* strB);

//---------------------------------------------------------
//   Deduplicate
///     Forward declaration of class List in order to
///     avoid circular dependency.
//---------------------------------------------------------

template<typename T>
class List;

List<const char*>* deduplicate(const char* words);

//---------------------------------------------------------
//   Hash Function
//---------------------------------------------------------

unsigned long djb2(const char *str);

#endif // UTILITIES_H
