#ifdef TESTS

#include <iostream>
#include <cstring>
#include "../src/bktree.h"
#include "../src/utilities.h"
#include "../src/entry.h"
#include "catch.hpp"

TEST_CASE("BKTree Edit Distance")
{
    EntryList list;
    list.append(new Entry("hell"));
    list.append(new Entry("help"));
    list.append(new Entry("fall"));
    list.append(new Entry("fell"));
    list.append(new Entry("felt"));
    list.append(new Entry("melt"));
    list.append(new Entry("small"));

    BKTree tree(list, editDistanceMatching);

//    tree.print();

    auto results = tree.lookup("henn", 2);
    for (auto& x : *results) {
        std::cout << x->m_word << std::endl;
    }
    delete results;

    results = tree.lookup("henn", 3);
    for (auto& x : *results) {
        std::cout << x->m_word << std::endl;
    }
    delete results;

    for (auto& x : list) {
        delete x;
    }
}

TEST_CASE("BKTree Hamming Distance")
{
    EntryList list;
    list.append(new Entry("hell"));
    list.append(new Entry("help"));
    list.append(new Entry("fall"));
    list.append(new Entry("fell"));
    list.append(new Entry("felt"));
    list.append(new Entry("melt"));
    list.append(new Entry("smol"));

    BKTree tree(list, hammingDistanceMatching);

//    tree.print();

    auto results = tree.lookup("henn", 2);
    std::cout << "HENN 2:" << std::endl;
    for (auto& x : *results) {
        std::cout << x->m_word << std::endl;
    }
    delete results;

    results = tree.lookup("henn", 3);
    std::cout << "HENN 3:" << std::endl;
    for (auto& x : *results) {
        std::cout << x->m_word << std::endl;
    }
    delete results;

    results = tree.multiLookup("henn", 1, 3);
    std::cout << "HENN 1:" << std::endl;
    for (auto& x : results[0]) {
        std::cout << x->m_word << std::endl;
    }
    std::cout << "HENN 2:" << std::endl;
    for (auto& x : results[1]) {
        std::cout << x->m_word << std::endl;
    }
    std::cout << "HENN 3:" << std::endl;
    for (auto& x : results[2]) {
        std::cout << x->m_word << std::endl;
    }
    delete[] results;

    for (auto& x : list) {
        delete x;
    }
}

TEST_CASE("BKTree insert max word size")
{
    EntryList list;
    list.append(new Entry("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));
    list.append(new Entry("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"));

    BKTree tree(list, hammingDistanceMatching);

//    tree.print();

    for (auto& x : list) {
        delete x;
    }
}

TEST_CASE("BKTree Lookup 1")
{
    EntryList list;
    list.append(new Entry("aaabbb"));
    list.append(new Entry("aaaaaa"));
    list.append(new Entry("aaaaab"));
    list.append(new Entry("aaaabb"));
    list.append(new Entry("aabbbb"));
    list.append(new Entry("abbbbb"));
    list.append(new Entry("bbbbbb"));
    list.append(new Entry("ccccccc"));
    list.append(new Entry("ddddddd"));

    BKTree tree(list, editDistanceMatching);

//    tree.print();

    auto results = tree.lookup("bbbbbb", 0);
    REQUIRE(results->count() == 1);
    delete results;

    results = tree.lookup("caabbc", 2);
    std::cout << "caabbc 2:" << std::endl;
    for (auto& x : *results) {
        std::cout << x->m_word << std::endl;
    }
    REQUIRE(results->count() == 1);
    delete results;

    results = tree.lookup("caabbc", 3);
    std::cout << "caabbc 3:" << std::endl;
    for (auto& x : *results) {
        std::cout << x->m_word << std::endl;
    }
    delete results;

    results = tree.multiLookup("caabbc", 1, 3);
    std::cout << "caabbc 1:" << std::endl;
    for (auto& x : results[0]) {
        std::cout << x->m_word << std::endl;
    }
    std::cout << "caabbc 2:" << std::endl;
    for (auto& x : results[1]) {
        std::cout << x->m_word << std::endl;
    }
    std::cout << "caabbc 3:" << std::endl;
    for (auto& x : results[2]) {
        std::cout << x->m_word << std::endl;
    }
    delete[] results;

    for (auto& x : list) {
        delete x;
    }
}

TEST_CASE("BKTree Lookup 2")
{
    EntryList list;
    list.append(new Entry("air"));
    list.append(new Entry("aid"));
    list.append(new Entry("army"));
    list.append(new Entry("adopt"));
    list.append(new Entry("allusion"));
    list.append(new Entry("amend"));
    list.append(new Entry("alter"));
    list.append(new Entry("ally"));
    list.append(new Entry("assay"));

    BKTree tree(list, editDistanceMatching);

//    tree.print();

    auto results = tree.lookup("abcde", 1);
    REQUIRE(results->count() == 0);
    delete results;

    results = tree.lookup("abandon", 5);
    REQUIRE(results->count() == 4);
    delete results;

    for (auto& x : list) {
        delete x;
    }
}

TEST_CASE("BKTree Lookup 3")
{
    EntryList list;
    list.append(new Entry("10001110"));
    list.append(new Entry("10011110"));
    list.append(new Entry("11101110"));
    list.append(new Entry("01101110"));
    list.append(new Entry("11011110"));
    list.append(new Entry("10001101"));

    BKTree tree(list, editDistanceMatching);

//    tree.print();

    auto results = tree.lookup("00110011", 3);
    REQUIRE(results->count() == 1);
    delete results;

    results = tree.lookup("11111111", 2);
    REQUIRE(results->count() == 2);
    delete results;

    for (auto& x : list) {
        delete x;
    }
}

#endif
