#include "cpp_uriparser.h"
#include <gtest/gtest.h>

using namespace uri_parser;

TEST(cppUriParser, find_key)
{
  EXPECT_NO_THROW(
  {
    auto entry = uri_parser::UriParseUrl("http://lol.wat/post?url=http://domain.tld/&title=Thetitleofapost&lala=1&blabla");
    auto query = entry.GetQuery();
    auto keyIt = query.findKey("title");
    EXPECT_FALSE(keyIt == query.end());
    EXPECT_STREQ(keyIt->value.c_str(), "Thetitleofapost");

    auto keyInvalid = query.findKey("hello");
    EXPECT_TRUE(keyInvalid == query.end());

    auto keyEmpty = query.findKey("");
    EXPECT_TRUE(keyEmpty == query.end());
  });
}

TEST(cppUriParser, find_multiple_key_entries)
{
  EXPECT_NO_THROW(
  {
    auto entry = uri_parser::UriParseUrl("http://lol.wat/post?url=http://domain.tld/&lala=23&title=Thetitleofapost&lala=1&blabla");
    auto query = entry.GetQuery();
    auto keyIt = query.findKeyEntries("lala");
    EXPECT_TRUE(keyIt.is_initialized());
    EXPECT_EQ(keyIt->size(), 2);
    auto id = 0;
    for (auto item = std::begin(keyIt.get()); item != std::end(keyIt.get()); ++item, ++id)
    {
      switch (id)
      {
      case 0:
        EXPECT_STREQ((*item)->key.c_str(), "lala");
        EXPECT_STREQ((*item)->value.c_str(), "23");
        break;
      case 1:
        EXPECT_STREQ((*item)->key.c_str(), "lala");
        EXPECT_STREQ((*item)->value.c_str(), "1");
        break;
      default:
        ADD_FAILURE();
        break;
      }
    }
  });
}

TEST(cppUriParser, find_multiple_value_entries)
{
  EXPECT_NO_THROW(
  {
    auto entry = uri_parser::UriParseUrl("http://lol.wat/post?heyhey=1&boom=1&title=Thetitleofapost&lala=1&blabla");
  auto query = entry.GetQuery();
  auto keyIt = query.findValueEntries("1");
  EXPECT_TRUE(keyIt.is_initialized());
  EXPECT_EQ(keyIt->size(), 3);
  auto id = 0;
  for (auto item = std::begin(keyIt.get()); item != std::end(keyIt.get()); ++item, ++id)
  {
    switch (id)
    {
    case 0:
      EXPECT_STREQ((*item)->key.c_str(), "heyhey");
      EXPECT_STREQ((*item)->value.c_str(), "1");
      break;
    case 1:
      EXPECT_STREQ((*item)->key.c_str(), "boom");
      EXPECT_STREQ((*item)->value.c_str(), "1");
      break;
    case 2:
      EXPECT_STREQ((*item)->key.c_str(), "lala");
      EXPECT_STREQ((*item)->value.c_str(), "1");
      break;
    default:
      ADD_FAILURE();
      break;
    }
  }
  });
}

TEST(cppUriParser, find_value)
{
  EXPECT_NO_THROW(
  {
    auto entry = uri_parser::UriParseUrl("http://lol.wat/post?url=http://domain.tld/&title=Thetitleofapost&lala=1&blabla");
    auto query = entry.GetQuery();
    auto keyIt = query.findValue("http://domain.tld/");
    EXPECT_STREQ(keyIt->key.c_str(), "url");

    auto keyInvalid = query.findValue("hello");
    EXPECT_TRUE(keyInvalid == query.end());
  });
}

TEST(cppUriParser, query_size)
{
  const char* url = "http://lol.wat/post?url=http://domain.tld/&title=Thetitleofapost&lala=1&blabla";
  auto entry = uri_parser::UriParseUrl(url);
  EXPECT_EQ(entry.GetQuery().size(), 4);
}

TEST(cppUriParser, empty_query)
{
  EXPECT_NO_THROW(
  {
    auto entry = uri_parser::UriParseUrl("http://lol.wat/post");
    auto query = entry.GetQuery();
    EXPECT_TRUE(query.empty());
  });
}

TEST(cppUriParser, basic_query_test)
{
  auto entry = uri_parser::UriParseUrl("http://lol.wat/post?url=http://domain.tld/&title=Thetitleofapost&lala=1&blabla");
  auto query = entry.GetQuery();

  auto id = 0;
  for (auto item = std::begin(query); item != std::end(query); ++item, ++id)
  {
    switch (id)
    {
    case 0:
      EXPECT_STREQ(item->key.c_str(), "url");
      EXPECT_STREQ(item->value.c_str(), "http://domain.tld/");
      break;
    case 1:
      EXPECT_STREQ(item->key.c_str(), "title");
      EXPECT_STREQ(item->value.c_str(), "Thetitleofapost");
      break;
    case 2:
      EXPECT_STREQ(item->key.c_str(), "lala");
      EXPECT_STREQ(item->value.c_str(), "1");
      break;
    case 3:
      EXPECT_STREQ(item->key.c_str(), "blabla");
      EXPECT_TRUE(item->value.empty());
      break;
    default:
      ADD_FAILURE();
      break;
    }
  }
  SUCCEED();
}

TEST(cppUriParser, traversing_through_EMPTY_query)
{
  auto entry = uri_parser::UriParseUrl("http://lol.wat/pos");
  auto query = entry.GetQuery();

  for (auto item = std::begin(query); item != std::end(query); ++item)
  {
    ADD_FAILURE();
  }
  SUCCEED();
}