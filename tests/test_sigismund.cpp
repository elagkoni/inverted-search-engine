#ifdef TESTS
#ifndef MULTITHREADING // FIXME: Multithreading is WIP

#include <iostream>
#include <cstring>
#include "catch.hpp"

#define private public
#include "../src/sigismund.h"

TEST_CASE("Sigismund Exact Index")
{
    Sigismund s;

    s.initialize();

    Query* q1 = new Query(1, "hello catto sir madam", MT_EXACT_MATCH, 0);
    Query* q2 = new Query(2, "catto", MT_EXACT_MATCH, 0);
    REQUIRE(q2->wordsNum == 1);
    Query* q3 = new Query(3, "hello", MT_EXACT_MATCH, 0);

    s.start(q1);
    s.start(q2);
    s.start(q3);

    REQUIRE(s.exactIndex->keys().count() == 4);
    REQUIRE(strcmp((*s.exactIndex->get("hello"))->m_word, "hello") == 0);
    REQUIRE((*s.exactIndex->get("hello"))->m_appearsIn.count() == 2);
    REQUIRE((*s.exactIndex->get("hello"))->m_appearsIn.at(0) == 1);
    REQUIRE((*s.exactIndex->get("hello"))->m_appearsIn.at(1) == 3);
    REQUIRE((*s.exactIndex->get("hello"))->m_appearsIn.count() == 2);

    s.destroy();
}

TEST_CASE("Sigismund Edit Index")
{
    Sigismund s;

    s.initialize();

    Query* q1 = new Query(1, "airport hello", MT_EDIT_DIST, 1);
    Query* q2 = new Query(2, "help hello airlines", MT_EDIT_DIST, 3);
    Query* q3 = new Query(3, "hello rebbe rabbi", MT_EDIT_DIST, 2);

    s.start(q1);
    s.start(q2);
    s.start(q3);


    REQUIRE(s.editEntries->keys().count() == 6);
    REQUIRE((*s.editEntries->get("help"))->m_appearsIn.count() == 1);

    auto results = s.editIndex->lookup("hell", 1);
    REQUIRE(results->count() == 2);
    delete results;
    results = s.editIndex->lookup("airebb", 3);
    REQUIRE(results->count() == 1);

    REQUIRE((*s.editEntries->get("hello"))->m_appearsIn.at(0) == 1);
    REQUIRE((*s.editEntries->get("hello"))->m_appearsIn.at(1) == 2);
    REQUIRE((*s.editEntries->get("hello"))->m_appearsIn.at(2) == 3);

    delete results;

    s.destroy();
}

TEST_CASE("Sigismund Hamming Index")
{
    Sigismund s;

    s.initialize();

    Query* q1 = new Query(10, "vacation hello work deadline incomplete", MT_HAMMING_DIST, 1);
    REQUIRE(q1->wordsNum == 5);
    Query* q2 = new Query(11, "help island", MT_HAMMING_DIST, 3);
    REQUIRE(q2->wordsNum == 2);
    Query* q3 = new Query(12, "hello rebbe rabbi", MT_HAMMING_DIST, 2);

    s.start(q1);
    s.start(q2);
    s.start(q3);

    REQUIRE(s.hammingEntries->keys().count() == 9);
    REQUIRE((*s.hammingEntries->get("help"))->m_appearsIn.count() == 1);

    auto results = (*s.hammingIndex)->lookup("henn", 2);
    REQUIRE(results->count() == 1);
    delete results;

    REQUIRE((*s.hammingEntries->get("work"))->m_appearsIn.at(0) == 10);
    REQUIRE((*s.hammingEntries->get("hello"))->m_appearsIn.at(1) == 12);

    s.destroy();
}

#endif
#endif
