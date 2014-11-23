// testConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cpp_uriparser.h"
#include "cpp_uriparser_query.h"
#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
  UriParserStateA state;
  UriUriA uri;

  state.uri = &uri;
  if (uriParseUriA(&state, "http://delicious.com/post?url=http://domain.tld/&title=Thetitleofapost") != URI_SUCCESS){
    throw std::runtime_error("1");
  }
  uri_parser::UriQueryList<UriQueryListStructA, UriUriA> queryList(uri, &uriDissectQueryMallocA, &uriFreeQueryListA);

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

