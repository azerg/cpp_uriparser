// testConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cpp_uriparser.h"
#include "cpp_uriparser_query.h"
#include <iostream>

using namespace uri_parser;

int _tmain(int argc, _TCHAR* argv[])
{
  {
    UriParserStateA state;
    UriUriA uri;

    state.uri = &uri;
    if (uriParseUriA(&state, "http://delicious.com/post?url=http://domain.tld/&title=Thetitleofapost&lala=1&blabla") != URI_SUCCESS){
      throw std::runtime_error("1");
    }
  }

  const char* url = "http://delicious.com/post?url=http://domain.tld/&title=Thetitleofapost&lala=1&blabla";
  auto entry = uri_parser::UriParseUrl(url);

  auto query = entry.Query();
  if (!query.is_initialized())
  {
    std::cout << "oeps";
    return 1;
  }

  for (auto item = std::begin(query.get()); item != std::end(query.get()); ++item)
  {
    std::cout << item->key_.c_str() << ":" << item->value_.c_str() << std::endl;
  }

  //uri_parser::UriQueryList<UriQueryListStructA, UriUriA> queryList(uri, &uriDissectQueryMallocA, &uriFreeQueryListA);
  /*
  const char* url = "http://www.example.com/name%20with%20spaces/lalala/TheLastOne";

  auto entry = uri_parser::UriParseUrl( url );
  auto pathIt = entry.PathHead();

  for ( auto pathFragment = pathIt.cbegin(); pathFragment != pathIt.cend(); ++pathFragment )
  {
    std::cout << pathFragment->c_str() << std::endl;
  }
  */
}

