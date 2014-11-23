// testConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cpp_uriparser.h"
#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
  const char* url = "http://www.example.com/name%20with%20spaces/lalala/TheLastOne";

  auto entry = core::common_parsers::UriParseUrl( url );
  auto pathIt = entry.PathHead();

  for ( auto pathFragment = pathIt.cbegin(); pathFragment != pathIt.cend(); ++pathFragment )
  {
    std::cout << pathFragment->c_str() << std::endl;
  }
}

