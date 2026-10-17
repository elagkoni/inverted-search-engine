#ifdef TESTS

#include <iostream>
#include <cstring>
#include "../src/list.h"
#include "../src/utilities.h"
#include "../src/entry.h"
#include "catch.hpp"

TEST_CASE("LinkedList")
{
    List<int> list;

    list.append(5);
    list.append(-1);
    list.append(0);

    REQUIRE(list.at(0) == 5);
    REQUIRE(list.at(1) == -1);
    REQUIRE(list.at(2) == 0);

    list.insert(4, 0);

    REQUIRE(list.at(0) == 4);
    REQUIRE(list.at(1) == 5);
    REQUIRE(list.at(2) == -1);
    REQUIRE(list.at(3) == 0);

    REQUIRE(list.first() == 4);
    REQUIRE(list.last() == 0);

    list.remove(1, 2);

    REQUIRE(list.at(0) == 4);
    REQUIRE(list.at(1) == 0);

    list.clear();
    list.append(8);

    REQUIRE(list.at(0) == 8);
    REQUIRE(list.count() == 1);

    for (auto& x : list) {
        std::cout << x << std::endl;
    }
}

TEST_CASE("OrderedList")
{
    OrderedList<int> ord(compareInts);

    ord.insert(5);
    ord.insert(-1);
    ord.insert(0);

    REQUIRE(ord.at(0) == -1);
    REQUIRE(ord.at(1) == 0);
    REQUIRE(ord.at(2) == 5);

    ord.insert(4);

    REQUIRE(ord.at(0) == -1);
    REQUIRE(ord.at(1) == 0);
    REQUIRE(ord.at(2) == 4);
    REQUIRE(ord.at(3) == 5);

    for (auto& x : ord) {
        std::cout << x << std::endl;
    }
}

TEST_CASE("EntryList")
{
    EntryList list;
    Entry* a = new Entry("Sergios");
    Entry* b = new Entry("Eleftheria");

    list.append(a);
    list.append(b);

    REQUIRE(strcmp(list.last()->m_word, "Eleftheria") == 0);

    delete a;
    delete b;
}

#endif
