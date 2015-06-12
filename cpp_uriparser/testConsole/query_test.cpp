#include "cpp_uriparser.h"
#include <gtest/gtest.h>

using namespace uri_parser;

/*
TEST(cppUriParser, traversing_through_query)
{
  auto entry = uri_parser::UriParseUrl("http://lol.wat/post?url=http://domain.tld/&title=Thetitleofapost&lala=1&blabla");
  auto query = entry.Query();

  auto id = 0;
  for (auto item = std::begin(query); item != std::end(query); ++item, ++id)
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

TEST(cppUriParser, traversing_through_EMPTY_query)
{
  auto entry = uri_parser::UriParseUrl("http://lol.wat/pos");
  auto query = entry.Query();

  for (auto item = std::begin(query); item != std::end(query); ++item)
  {
    ADD_FAILURE();
  }
  SUCCEED();
}

TEST(cppUriParser, empty_query)
{
  EXPECT_NO_THROW(
  {
    auto entry = uri_parser::UriParseUrl("http://lol.wat/post");
    auto query = entry.Query();
    EXPECT_TRUE(query.empty());
  });
}

TEST(cppUriParser, query_size)
{
  const char* url = "http://lol.wat/post?url=http://domain.tld/&title=Thetitleofapost&lala=1&blabla";
  auto entry = uri_parser::UriParseUrl(url);
  EXPECT_EQ(entry.Query().size(), 4);
}

TEST(cppUriParser, find_key)
{
  EXPECT_NO_THROW(
  {
    auto entry = uri_parser::UriParseUrl("http://lol.wat/post?url=http://domain.tld/&title=Thetitleofapost&lala=1&blabla");
    auto query = entry.Query();
    auto keyIt = query.findKey("title");
    EXPECT_STREQ(keyIt->value_.c_str(), "Thetitleofapost");

    auto keyInvalid = query.findKey("hello");
    EXPECT_TRUE(keyInvalid == query.end());
  });
}

TEST(cppUriParser, find_value)
{
  EXPECT_NO_THROW(
  {
    auto entry = uri_parser::UriParseUrl("http://lol.wat/post?url=http://domain.tld/&title=Thetitleofapost&lala=1&blabla");
    auto query = entry.Query();
    auto keyIt = query.findValue("http://domain.tld/");
    EXPECT_STREQ(keyIt->key_.c_str(), "url");

    auto keyInvalid = query.findValue("hello");
    EXPECT_TRUE(keyInvalid == query.end());
  });
}
*/
TEST(cppUriParser, basic_query_test)
{
  auto entry = uri_parser::UriParseUrl("http://lol.wat/post?url=http://domain.tld/&title=Thetitleofapost&lala=1&blabla");
  auto query = entry.GetQuery();

  for (auto item = std::begin(query); item != std::end(query); ++item)
  {
    ADD_FAILURE();
  }
  SUCCEED();
}