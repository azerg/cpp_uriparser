#pragma once

#include <exception>
#include <iostream>
#include <iterator>
#include "uriparser/Uri.h"

namespace uri_parser
{
  template <class UriQueryListStructType, class charType>
  struct UriQueryListStruct
  {
    const charType* key_;
    const charType* value_;
    UriQueryListStruct* next_;
  };

  template <class UriQueryListType, class UriObjType>
  class UriQueryList
  {
    typedef std::function<int(UriQueryListType**, int*, decltype(UriQueryListType::key),
      decltype(UriQueryListType::key))> UriDissectQueryMallocProc;
    typedef std::function<void(UriQueryListType*)> UriFreeQueryListProc;
  public:
    UriQueryList(UriObjType& uri, UriDissectQueryMallocProc uriDissectQueryMallocProc, UriFreeQueryListProc uriFreeQueryListProc):
      uriDissectQueryMallocProc_(uriDissectQueryMallocProc),
      uriFreeQueryListProc_(uriFreeQueryListProc)
    {
      if (uriDissectQueryMallocProc_(&queryList_, &itemCount_, uri.query.first,
        uri.query.afterLast ) != URI_SUCCESS )
      {
        throw std::runtime_error("UriQueryList error: error parsing query array");
      }
      currentQueryList_ = queryList_;

      UriQueryListType* cur = queryList_;
      for (int i = 0; i < itemCount_; i++)
      {
        std::cout << cur->key << ":" << cur->value << std::endl;
        cur = cur->next;
      }
    }

    virtual ~UriQueryList()
    {
      uriFreeQueryListProc_(queryList_);
    }

  private:
    int itemCount_;
    UriQueryListType* queryList_;
    UriQueryListType* currentQueryList_;
    UriDissectQueryMallocProc uriDissectQueryMallocProc_;
    UriFreeQueryListProc uriFreeQueryListProc_;
  };
} // uri_parser