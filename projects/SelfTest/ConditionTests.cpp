/*
 *  ConditionTests.cpp
 *  Catch - Test
 *
 *  Created by Phil on 08/11/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include "catch.hpp"

#include <string>
#include <limits>

struct TestData
{
    TestData()
    :   int_seven( 7 ),
        str_hello( "hello" ),
        float_nine_point_one( 9.1f ),
        double_pi( 3.1415926535 )
    {}
    
    int int_seven;
    std::string str_hello;
    float float_nine_point_one;
    double double_pi;
};

// The "failing" tests all use the CHECK macro, which continues if the specific test fails.
// This allows us to see all results, even if an earlier check fails

// Equality tests
TEST_CASE(  "./succeeding/conditions/equality", 
            "Equality checks that should succeed" )
{
    TestData data;
    
    REQUIRE( data.int_seven == 7 );
    REQUIRE( data.float_nine_point_one == Approx( 9.1f ) );
    REQUIRE( data.double_pi == Approx( 3.1415926535 ) );
    REQUIRE( data.str_hello == "hello" );
    REQUIRE( data.str_hello.size() == 5 );

    double x = 1.1 + 0.1 + 0.1;
    REQUIRE( x == Approx( 1.3 ) );
}

TEST_CASE(  "./failing/conditions/equality", 
            "Equality checks that should fail" )
{
    TestData data;
    
    CHECK( data.int_seven == 6 );
    CHECK( data.int_seven == 8 );
    CHECK( data.int_seven == 0 );
    CHECK( data.float_nine_point_one == Approx( 9.11f ) );
    CHECK( data.float_nine_point_one == Approx( 9.0f ) );
    CHECK( data.float_nine_point_one == Approx( 1 ) );
    CHECK( data.float_nine_point_one == Approx( 0 ) );
    CHECK( data.double_pi == Approx( 3.1415 ) );
    CHECK( data.str_hello == "goodbye" );
    CHECK( data.str_hello == "hell" );
    CHECK( data.str_hello == "hello1" );
    CHECK( data.str_hello.size() == 6 );

    double x = 1.1 + 0.1 + 0.1;
    CHECK( x == Approx( 1.301 ) );
}

TEST_CASE(  "./succeeding/conditions/inequality", 
            "Inequality checks that should succeed" )
{
    TestData data;
    
    REQUIRE( data.int_seven != 6 );
    REQUIRE( data.int_seven != 8 );
    REQUIRE( data.float_nine_point_one != Approx( 9.11f ) );
    REQUIRE( data.float_nine_point_one != Approx( 9.0f ) );
    REQUIRE( data.float_nine_point_one != Approx( 1 ) );
    REQUIRE( data.float_nine_point_one != Approx( 0 ) );
    REQUIRE( data.double_pi != Approx( 3.1415 ) );
    REQUIRE( data.str_hello != "goodbye" );
    REQUIRE( data.str_hello != "hell" );
    REQUIRE( data.str_hello != "hello1" );
    REQUIRE( data.str_hello.size() != 6 );
}

TEST_CASE(  "./failing/conditions/inequality", 
            "Inequality checks that should fails" )
{
    TestData data;
    
    CHECK( data.int_seven != 7 );
    CHECK( data.float_nine_point_one != Approx( 9.1f ) );
    CHECK( data.double_pi != Approx( 3.1415926535 ) );
    CHECK( data.str_hello != "hello" );
    CHECK( data.str_hello.size() != 5 );
}

// Ordering comparison tests
TEST_CASE(  "./succeeding/conditions/ordered", 
            "Ordering comparison checks that should succeed" )
{
    TestData data;
    
    REQUIRE( data.int_seven < 8 );
    REQUIRE( data.int_seven > 6 );
    REQUIRE( data.int_seven > 0 );
    REQUIRE( data.int_seven > -1 );

    REQUIRE( data.int_seven >= 7 );
    REQUIRE( data.int_seven >= 6 );
    REQUIRE( data.int_seven <= 7 );
    REQUIRE( data.int_seven <= 8 );
    
    REQUIRE( data.float_nine_point_one > 9 );
    REQUIRE( data.float_nine_point_one < 10 );
    REQUIRE( data.float_nine_point_one < 9.2 );
    
    REQUIRE( data.str_hello <= "hello" );
    REQUIRE( data.str_hello >= "hello" );
    
    REQUIRE( data.str_hello < "hellp" );
    REQUIRE( data.str_hello < "zebra" );
    REQUIRE( data.str_hello > "hellm" );
    REQUIRE( data.str_hello > "a" );
}

TEST_CASE(  "./failing/conditions/ordered", 
            "Ordering comparison checks that should fail" )
{
    TestData data;
    
    CHECK( data.int_seven > 7 );
    CHECK( data.int_seven < 7 );
    CHECK( data.int_seven > 8 );
    CHECK( data.int_seven < 6 );
    CHECK( data.int_seven < 0 );
    CHECK( data.int_seven < -1 );

    CHECK( data.int_seven >= 8 );
    CHECK( data.int_seven <= 6 );
    
    CHECK( data.float_nine_point_one < 9 );
    CHECK( data.float_nine_point_one > 10 );
    CHECK( data.float_nine_point_one > 9.2 );
    
    CHECK( data.str_hello > "hello" );
    CHECK( data.str_hello < "hello" );
    CHECK( data.str_hello > "hellp" );
    CHECK( data.str_hello > "z" );
    CHECK( data.str_hello < "hellm" );
    CHECK( data.str_hello < "a" );

    CHECK( data.str_hello >= "z" );
    CHECK( data.str_hello <= "a" );
}

// Comparisons with int literals
TEST_CASE(  "./succeeding/conditions/int literals", 
            "Comparisons with int literals don't warn when mixing signed/ unsigned" )
{
    int i = 1;
    unsigned int ui = 2;
    long l = 3;
    unsigned long ul = 4;
    char c = 5;
    unsigned char uc = 6;
    
    REQUIRE( i == 1 );
    REQUIRE( ui == 2 );
    REQUIRE( l == 3 );
    REQUIRE( ul == 4 );
    REQUIRE( c == 5 );
    REQUIRE( uc == 6 );

    REQUIRE( 1 == i );
    REQUIRE( 2 == ui );
    REQUIRE( 3 == l );
    REQUIRE( 4 == ul );
    REQUIRE( 5 == c );
    REQUIRE( 6 == uc );

    REQUIRE( (std::numeric_limits<unsigned long>::max)() > ul );
}

// These are not built normally to avoid warnings about signed/ unsigned
#ifdef ALLOW_TESTS_THAT_WARN
TEST_CASE(  "succeeding/conditions/negative ints", 
            "Comparisons between unsigned ints and negative signed ints match c++ standard behaviour" )
{
    CHECK( ( -1 > 2u ) );
    CHECK( -1 > 2u );

    CHECK( ( 2u < -1 ) );
    CHECK( 2u < -1 );

    const int minInt = (std::numeric_limits<int>::min)();
    CHECK( ( minInt > 2u ) );
    CHECK( minInt > 2u );
}
#endif

TEST_CASE(  "./succeeding/conditions/ptr", 
            "Pointers can be compared to null" )
{
    TestData* p = NULL;
    TestData* pNULL = NULL;
    
    REQUIRE( p == NULL );
    REQUIRE( p == pNULL );
    
    TestData data;
    p = &data;
    
    REQUIRE( p != NULL );

    const TestData* cp = p;
    REQUIRE( cp != NULL );

    const TestData* const cpc = p;
    REQUIRE( cpc != NULL );

//    REQUIRE( NULL != p ); // gives warning, but should compile and run ok
}

// Not (!) tests
// The problem with the ! operator is that it has right-to-left associativity.
// This means we can't isolate it when we decompose. The simple REQUIRE( !false ) form, therefore,
// cannot have the operand value extracted. The test will work correctly, and the situation
// is detected and a warning issued.
// An alternative form of the macros (CHECK_FALSE and REQUIRE_FALSE) can be used instead to capture
// the operand value.
TEST_CASE(  "./succeeding/conditions/not", 
            "'Not' checks that should succeed" )
{
    bool falseValue = false;
    
    REQUIRE( !false );
    REQUIRE_FALSE( false );

    REQUIRE( !falseValue );
    REQUIRE_FALSE( falseValue );

    REQUIRE( !(1 == 2) );
    REQUIRE_FALSE( 1 == 2 );
}

TEST_CASE(  "./failing/conditions/not", 
            "'Not' checks that should fail" )
{
    bool trueValue = true;
    
    CHECK( !true );
    CHECK_FALSE( true );
    
    CHECK( !trueValue );
    CHECK_FALSE( trueValue );
    
    CHECK( !(1 == 1) );
    CHECK_FALSE( 1 == 1 );
}

