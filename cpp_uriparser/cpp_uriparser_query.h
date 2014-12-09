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
      // supports end() == end()
      if (currentQueryList_ == right.currentQueryList_)
      {
        return true;
      }

      // comparing Iterator with end()
      if (right.currentQueryList_ == nullptr)
      {
        return false;
      }

      // comparing two "normal" iterators
      return ( currentQueryList_->key == right.currentQueryList_->key &&
        (currentQueryList_->value == right.currentQueryList_->value) &&
        (currentQueryList_->next == right.currentQueryList_->next));
    }

    bool operator!=(const IteratorType& right)
    {
      return !operator==(right);
    }

    UriQueryEntry<UrlReturnType> operator*() const
    {
      return UriQueryEntry<UrlReturnType>{
        currentQueryList_->key == nullptr ? UrlReturnType() : currentQueryList_->key,
        currentQueryList_->value == nullptr ? UrlReturnType() : currentQueryList_->value};
    }

    UriQueryEntry<UrlReturnType>* operator->()
    {
      returnObjStorage_ = std::move(operator*());
      return &returnObjStorage_;
    }

  private:
    UriQueryListType* currentQueryList_;
    UriQueryEntry<UrlReturnType> returnObjStorage_;
  };


  template <class UriQueryListType, class UrlReturnType, class UriObjType>
  class UriQueryListBase
  {
    typedef decltype(UriQueryListType::key) QueryListCharType;
    typedef std::function<int(UriQueryListType**, int*, QueryListCharType, QueryListCharType)> UriDissectQueryMallocProc;
    typedef std::function<void(UriQueryListType*)> UriFreeQueryListProc;
    typedef UriQueryListIterator<UriQueryListType, UrlReturnType> IteratorType;
  public:
    UriQueryListBase(UriQueryListBase&& right):
      itemCount_(std::move(right.itemCount_)),
      uriFreeQueryListProc_(std::move(right.uriFreeQueryListProc_)),
      uriDissectQueryMallocProc_(std::move(right.uriDissectQueryMallocProc_)),
      queryList_(std::move(right.queryList_))
    {
      // we have to cleaup right ptr, not to free it in right's destructor
      right.queryList_ = nullptr;
    }

    UriQueryListBase& operator=(UriQueryListBase&& right)
    {
      std::swap(this->itemCount_, right.itemCount_);
      std::swap(this->uriDissectQueryMallocProc_, right.uriDissectQueryMallocProc_);
      std::swap(this->uriFreeQueryListProc_, right.uriFreeQueryListProc_);
      this->queryList_ = right.queryList_;
      right.queryList_ = nullptr;
      return *this;
    }

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
    }

    virtual ~UriQueryListBase()
    {
      if (queryList_ != nullptr)
      {
        uriFreeQueryListProc_(queryList_);
      }
    }

  private:
    int itemCount_;
    UriQueryListType* queryList_;
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
    UriQueryList(UriQueryList&& right) :
      UriQueryListBase(std::move(right)){}
  };
  
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
} // namespace uri_parser