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

TEST(cppUriParser, unescape_test)
{
  const char* url = "http://www.example.com/name%20with%20spaces/lalala%01%FFg%0D%0A";
  auto entry = uri_parser::UriParseUrl(url);

  auto unescapedString = entry.GetUnescapedUrlString(true);
  EXPECT_TRUE(unescapedString.is_initialized());
  EXPECT_STREQ("http://www.example.com/name with spaces/lalala\x01\xffg\r\n", unescapedString.get().c_str());
}