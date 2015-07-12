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

  auto unescapedString = entry.GetUnescapedUrlString();
  EXPECT_FALSE(unescapedString.empty());
  EXPECT_STREQ("http://www.example.com/name with spaces/lalala\x01\xffg\r\n", unescapedString.c_str());
}

TEST(cppUriParser, unescaping_fragment)
{
  {
    const wchar_t* url = L"https://www.google.com/webhp?lala=la#q=%D0%BF%D1%80%D0%B8%D0%B2%D0%B5%D1%82";
    auto entry = uri_parser::UriParseUrl(url);
    auto frag = entry.GetUnescapedFragment();

    EXPECT_TRUE(frag.is_initialized());
    EXPECT_STREQ(frag.get().c_str(), L"q=\xd0\xbf\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82");
  }
  const char* url = "https://www.google.com/webhp?lala=la#q=%D0%BF%D1%80%D0%B8%D0%B2%D0%B5%D1%82";
  auto entry = uri_parser::UriParseUrl(url);
  auto frag = entry.GetUnescapedFragment();

  EXPECT_TRUE(frag.is_initialized());
  EXPECT_STREQ(frag.get().c_str(), "q=\xd0\xbf\xd1\x80\xd0\xb8\xd0\xb2\xd0\xb5\xd1\x82");
}

TEST(cppUriParser, DISABLED_UriTypesMoveTest)
{
  typedef uri_parser::internal::UriTypes<wchar_t*> UriTypesChar;
  UriTypesChar uriTypes;
  /*
  EXPECT_TRUE(uriTypes.freeUriMembers);

  std::unique_ptr<UriTypesChar> uriTypesEmpty = std::make_unique<UriTypesChar>(std::move(uriTypes));
  EXPECT_TRUE(uriTypesEmpty->freeUriMembers);
  EXPECT_TRUE(uriTypes.freeUriMembers);
  */
}