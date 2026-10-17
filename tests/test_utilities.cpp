#ifdef TESTS

#include <iostream>
#include <cstring>
#include "../src/utilities.h"
#include "../src/list.h"
#include "catch.hpp"

TEST_CASE("Compare")
{
    REQUIRE(compareInts(5, 6) < 0);
    REQUIRE(compareInts(6, 6) == 0);
    REQUIRE(compareInts(7, 6) > 0);
}

TEST_CASE("Exact Matching")
{
    REQUIRE(exactMatching("hello", "world") != 0);
    REQUIRE(exactMatching("hello world", "hello world") == 0);
}

TEST_CASE("Hamming Distance Matching")
{
    REQUIRE(hammingDistanceMatching("ser", "sir") == 1);
    REQUIRE(hammingDistanceMatching("hello world", "hello world") == 0);
    REQUIRE(hammingDistanceMatching("ser", "sre") == 2);
    REQUIRE(hammingDistanceMatching("ser", "sergios") == -1);
}

TEST_CASE("Edit Distance Matching")
{
    REQUIRE(editDistanceMatching("cat", "cut") == 1);
    REQUIRE(editDistanceMatching("kitten", "sitting") == 3);
    REQUIRE(editDistanceMatching("dit", "dit") == 0);
    REQUIRE(editDistanceMatching("dit", "") == 3);
    REQUIRE(editDistanceMatching("hell", "help") == 1);
    REQUIRE(editDistanceMatching("hell", "fell") == 1);
    REQUIRE(editDistanceMatching("hell", "fall") == 2);
    REQUIRE(editDistanceMatching("hell", "felt") == 2);
    REQUIRE(editDistanceMatching("hell", "melt") == 2);
    REQUIRE(editDistanceMatching("hell", "small") == 3);
    REQUIRE(editDistanceMatching("small", "hell") == 3);
    REQUIRE(editDistanceMatching("caabbc", "aaabbb") == 2);
    REQUIRE(editDistanceMatching("aaabbb", "caabbc") == 2);
    REQUIRE(editDistanceMatching("caabbc", "aaaabb") == 3);
    REQUIRE(editDistanceMatching("aaaabb", "caabbc") == 3);
    REQUIRE(editDistanceMatching("aaaaa", "a") == 4);
    REQUIRE(editDistanceMatching("a", "aaaaa") == 4);
    REQUIRE(editDistanceMatching("aaaaa", "b") == 5);
    REQUIRE(editDistanceMatching("b", "aaaaa") == 5);
    REQUIRE(editDistanceMatching("agnes", "aanites") == 3);
}

TEST_CASE("Deduplicate")
{
    auto list = deduplicate("Sergios Eleftheria Sergios");
    REQUIRE(list->count() == 2);

    for(auto& x : *list) {
        free((void*)x);
    }
    delete list;
}

#endif
