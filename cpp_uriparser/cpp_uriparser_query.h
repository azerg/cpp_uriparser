#pragma once

#include <exception>
#include <iostream>
#include <iterator>
#include <type_traits>
#include "uriparser/Uri.h"

namespace uri_parser
{
  template <class UrlReturnType>
  struct UriQueryEntry
  {
    UrlReturnType key_;
    UrlReturnType value_;
  };

  template <class UriQueryListType, class UrlReturnType>
  class UriQueryListIterator :
    public std::iterator<std::forward_iterator_tag, UriQueryEntry<UrlReturnType>>
  {
    typedef UriQueryListIterator<UriQueryListType, UrlReturnType> IteratorType;
  public:
    UriQueryListIterator(UriQueryListType* list) :
      currentQueryList_(list){}

    IteratorType& operator++()
    {
      if (currentQueryList_->next != nullptr)
      {
        currentQueryList_ = currentQueryList_->next;
      }
      else
      {
        *this = IteratorType(nullptr);
      }
      return *this;
    }

    IteratorType operator++(int) const
    {
      IteratorType tmp(*this);
      return ++tmp;
    }

    bool operator==(const IteratorType& right)
    {
      return ( currentQueryList_->key == right.currentQueryList_->key &&
        (currentQueryList_->value == right.currentQueryList_->value) &&
        (currentQueryList_->next == right.currentQueryList_->next));
    }

    bool operator!=(const IteratorType& right)
    {
      return !operator==(right);
    }

    UrlReturnType operator*() const
    {
      return UriQueryEntry<UrlReturnType>{currentQueryList_->key, currentQueryList_->value};
    }

    UrlReturnType* operator->()
    {
      returnObjStorage_ = std::move(operator*());
      return &returnObjStorage_;
    }

  private:
    UriQueryListType* currentQueryList_;
    UrlReturnType returnObjStorage_;
  };


  template <class UriQueryListType, class UrlReturnType, class UriObjType>
  class UriQueryListBase
  {
    typedef decltype(UriQueryListType::key) QueryListCharType;
    typedef std::function<int(UriQueryListType**, int*, QueryListCharType, QueryListCharType)> UriDissectQueryMallocProc;
    typedef std::function<void(UriQueryListType*)> UriFreeQueryListProc;
    typedef UriQueryListIterator<UriQueryListType, UrlReturnType> IteratorType;
  public:
    IteratorType begin(){ return IteratorType(queryList_); };
    IteratorType end(){ return IteratorType(nullptr); };

    UriQueryListBase(const UriObjType& uri, UriDissectQueryMallocProc uriDissectQueryMallocProc, UriFreeQueryListProc uriFreeQueryListProc) :
      uriDissectQueryMallocProc_(uriDissectQueryMallocProc),
      uriFreeQueryListProc_(uriFreeQueryListProc)
    {
      if (uriDissectQueryMallocProc_(&queryList_, &itemCount_, uri.query.first,
        uri.query.afterLast) != URI_SUCCESS)
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

    virtual ~UriQueryListBase()
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

  template <class UrlTextType, class Empty = void>
  class UriQueryList;

  template <class UrlTextType>
  class UriQueryList < UrlTextType, typename std::enable_if<std::is_convertible<UrlTextType, const char*>::value >::type> :
    public UriQueryListBase<UriQueryListStructA, std::string, UriUriA>
  {
  public:
    explicit UriQueryList(const UriUriA& uri):
      UriQueryListBase(uri, &uriDissectQueryMallocA, &uriFreeQueryListA){}
    //UriQueryList(UriQueryList&& right) :
    //  UriQueryListBase(std::move(right)){}
  };
  /*
  template <class UrlTextType>
  class UriQueryList < UrlTextType, typename std::enable_if<std::is_convertible<UrlTextType, const wchar_t*>::value >::type> :
    public UriQueryListBase<UriQueryListStructA, std::wstring, UriUriW>
  {
  public:
    explicit UriQueryList(UriUriW& uri) :
      UriQueryListBase(uri, &uriDissectQueryMallocW, &uriFreeQueryListW){}
    UriQueryList(UriQueryList&& right) :
      UriQueryListBase(std::move(right)){}
  };
  */
} // namespace uri_parser