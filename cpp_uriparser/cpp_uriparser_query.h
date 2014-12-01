#pragma once

#include <exception>
#include <iostream>
#include <iterator>
#include <type_traits>
#include "uriparser/Uri.h"

namespace uri_parser
{
  template <class UriQueryListType, class UriObjType>
  class UriQueryList
  {
    typedef decltype(UriQueryListType::key) QueryListCharType;
    typedef std::function<int(UriQueryListType**, int*, QueryListCharType, QueryListCharType)> UriDissectQueryMallocProc;
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

      /////////////// remove me
      currentQueryList_ = queryList_;

      UriQueryListType* cur = queryList_;
      for (int i = 0; i < itemCount_; i++)
      {
        std::cout << cur->key << ":" << cur->value << std::endl;
        cur = cur->next;
      }
      ///////////////////////////
    }

    virtual ~UriQueryList()
    {
      uriFreeQueryListProc_(queryList_);
    }

  private:
    int itemCount_;
    UriQueryListType* queryList_;
    UriQueryListType* currentQueryList_; //////////// remove me
    UriDissectQueryMallocProc uriDissectQueryMallocProc_;
    UriFreeQueryListProc uriFreeQueryListProc_;
  };

  template <class UrlReturnType>
  struct UriQueryEntry
  {
    UrlReturnType key_;
    UrlReturnType value_;
  };

  template <class UriQueryListType, class UrlReturnType>
  class UriQueryListIterator:
    public std::iterator<std::forward_iterator_tag, UriQueryEntry<UrlReturnType>>
  {
    typedef UriQueryListIterator<UriQueryListType, UrlReturnType> IteratorType;
  public:
    UriQueryListIterator(UriQueryListType* list):
      currentQueryList_(list){}

    IteratorType begin(){ return IteratorType(currentQueryList_); }
    IteratorType end(){ return IteratorType(nullptr); }

    IteratorType& operator++()
    {
      if (currentQueryList_->next != nullptr)
      {
        currentQueryList_ = currentQueryList_->next;
        return *this;
      }
      return IteratorType(nullptr);
    }

    IteratorType operator++(int) const
    {
      IteratorType tmp(*this);
      return ++tmp;
    }

    bool operator==(const IteratorType& right)
    {
      return currentQueryList_ == right;
    }

    bool operator!=(const IteratorType& right)
    {
      return !operator==(right);
    }

  private:
    UriQueryListType* currentQueryList_;
  };
} // uri_parser