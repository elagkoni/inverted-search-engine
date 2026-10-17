#ifndef TESTS

//#include <iostream>
//#include "sigismund.h"

//static const int Q = 10;
//static const int D = 2;

//int main(void)
//{
//    InitializeIndex();

//    /* Start Queries */
//    for (int i = 0; i < Q; i++) {
//        StartQuery(i, "Hello world", MT_EDIT_DIST, 3);
//    }

//    for (int i = 0; i < D; i++) {
//        MatchDocument(i, "My milkshake brings all the boys to the yard");
//    }

//    DestroyIndex();

//    return 0;
//}

#else

#define CATCH_CONFIG_RUNNER
#include "../tests/catch.hpp"

int main( int argc, char* argv[] ) {
  int result = Catch::Session().run(1, argv);
  return result;
}

#endif
