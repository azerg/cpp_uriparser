// testConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cpp_uriparser.h"
#include <iostream>
#include <gtest/gtest.h>

using namespace uri_parser;

TEST(cppUriParser, traversing_through_url)
{
  
  const char* url = "http://www.example.com/name%20with%20spaces/lalala/TheLastOne";
  
  auto entry = uri_parser::UriParseUrl(url);
  auto pathHead = entry.PathHead();
  
  auto id = 0;
  for (auto pathFragment = std::begin(pathHead); pathFragment != std::end(pathHead); ++pathFragment, ++id)
  {
    switch (id)
    {
    case 0:
      EXPECT_STREQ(pathFragment->c_str(), "name%20with%20spaces");
      break;
    case 1:
      EXPECT_STREQ(pathFragment->c_str(), "lalala");
      break;
    case 2:
      EXPECT_STREQ(pathFragment->c_str(), "TheLastOne");
      break;
    default:
      ADD_FAILURE();
      break;
    }
  }
}

TEST(cppUriParser, traversing_through_query)
{

  const char* url = "http://delicious.com/post?url=http://domain.tld/&title=Thetitleofapost&lala=1&blabla";
  auto entry = uri_parser::UriParseUrl(url);

  auto query = entry.Query();
  EXPECT_TRUE(query.is_initialized());

  auto id = 0;
  for (auto item = std::begin(query.get()); item != std::end(query.get()); ++item, ++id)
  {
    switch (id)
    {
    case 0:
      EXPECT_STREQ(item->key_.c_str(), "url");
      EXPECT_STREQ(item->value_.c_str(), "http://domain.tld/");
      break;
    case 1:
      EXPECT_STREQ(item->key_.c_str(), "title");
      EXPECT_STREQ(item->value_.c_str(), "Thetitleofapost");
      break;
    case 2:
      EXPECT_STREQ(item->key_.c_str(), "lala");
      EXPECT_STREQ(item->value_.c_str(), "1");
      break;
    case 3:
      EXPECT_STREQ(item->key_.c_str(), "blabla");
      EXPECT_TRUE(item->value_.empty());
      break;
    default:
      ADD_FAILURE();
      break;
    }
  }
}

int _tmain(int argc, _TCHAR* argv[])
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

