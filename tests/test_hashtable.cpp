#ifdef TESTS

#include <iostream>
#include "catch.hpp"

#define private public
#include "../src/hashtable.h"

TEST_CASE("Hashtable")
{
    Hashtable<const char*, int> map(100, equalStrings);

    map.insert("Hello", 0);
    map.insert("world", 1);
    map.insert("syspro", 10);

    REQUIRE(map.keys().count() == 3);

    REQUIRE(*map.get("Hello") == 0);
    REQUIRE(map.get("WORLD") == nullptr);
    REQUIRE(*map.get("syspro") == 10);

    map.insert("syspro", 10);

    REQUIRE(map.keys().count() == 3);
}

TEST_CASE("Hashtable Set")
{
    Hashtable<const char*, int> map(100, equalStrings);

    map.insert("Hello", 0);
    map.insert("world", 0);
    map.insert("syspro", 0);

    REQUIRE(map.get("Hello") != nullptr);
    REQUIRE(map.get("Hallo") == nullptr);
}

TEST_CASE("Hashtable Rehash")
{
    Hashtable<const char*, int> map(100, equalStrings);

    map.insert("Hello", 0);
    map.insert("world", 0);
    map.insert("syspro", 0);

    REQUIRE(map.m_dataArraySize == 100);
    REQUIRE(map.get("Hello") != nullptr);
    REQUIRE(map.get("Hallo") == nullptr);

    map.rehash();

    REQUIRE(map.m_dataArraySize == 200);
    REQUIRE(map.get("Hello") != nullptr);
    REQUIRE(map.get("world") != nullptr);
    REQUIRE(map.get("syspro") != nullptr);
    REQUIRE(map.get("Hallo") == nullptr);
}

TEST_CASE("Hashtable Rehash 2")
{
    Hashtable<const char*, int> map(1, equalStrings);

    map.insert("Hello", 0);
    map.insert("world", 0);
    map.insert("syspro", 0);
    map.insert("yes", 0);

    REQUIRE(map.m_dataArraySize == 1);

    map.insert("no", 0);

    REQUIRE(map.m_dataArraySize == 2);
}


#endif
