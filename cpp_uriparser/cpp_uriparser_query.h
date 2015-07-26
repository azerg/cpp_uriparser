#pragma once

#include <exception>
#include <iostream>
#include <iterator>
#include <string>
#include <list>
#include <boost/optional.hpp>
#include <type_traits>
#include "uriparser/Uri.h"

namespace uri_parser
{

  namespace internal
  {
    template <typename T>
    struct base_type{
      typedef typename std::remove_const<typename std::remove_pointer<T>::type>::type type;
    };

    template <typename T>
    struct base_const_ptr{
      typedef typename std::add_pointer<typename std::add_const<typename base_type<T>::type>::type>::type type;
    };

    template <typename T>
    struct base_ptr{
      typedef typename std::add_pointer<typename base_type<T>::type>::type type;
    };

    // default API types
#define URI_API_TYPES(PREFIX) \
      typedef UriUri##PREFIX UriObjType; \
      typedef UriParserState##PREFIX UriStateType; \
      typedef UriPathSegment##PREFIX UriPathSegmentType; \
      typedef UriQueryListStruct##PREFIX UriQueryListType; \
      \
      std::function<int(UriStateType*, UrlTextType)> parseUri;  \
      std::function<void(UriObjType*)> freeUriMembers;  \
      std::function<int(UriObjType*)> uriNormalizeSyntax; \
      typedef decltype(UriQueryListType::key) QueryListCharType;  \
      std::function<int(UriQueryListType**, int*, QueryListCharType, QueryListCharType)> uriDissectQueryMalloc; \
      std::function<void(UriQueryListType*)> uriFreeQueryList;  \
      /* add_const to support UrlTextType == tchar* & const tchar* ( api output is exactly const tchar* )*/ \
      std::function<typename base_const_ptr<UrlTextType>::type  \
        (typename base_ptr<UrlTextType>::type, UriBool, UriBreakConversion)> uriUnescapeInPlaceEx;  \
        \
      UriTypes() :  \
        parseUri(&uriParseUri##PREFIX),  \
        freeUriMembers(&uriFreeUriMembers##PREFIX),  \
        uriNormalizeSyntax(&uriNormalizeSyntax##PREFIX), \
        uriUnescapeInPlaceEx(&uriUnescapeInPlaceEx##PREFIX), \
        uriDissectQueryMalloc(&uriDissectQueryMalloc##PREFIX), \
        uriFreeQueryList(&uriFreeQueryList##PREFIX)  \
     {}


    // by default lets use ANSI api functions
    template <class UrlTextType, class Empty = void>
    struct UriTypes
    {
      typedef std::string UrlReturnType;
      URI_API_TYPES(A)
    };

    template <class UrlTextType>
    struct UriTypes<UrlTextType, typename std::enable_if<std::is_convertible<UrlTextType, const wchar_t*>::value >::type>
    {
      typedef std::wstring UrlReturnType;
      URI_API_TYPES(W)
    };

    template <class UriTextRangeType, class UrlReturnType>
    UrlReturnType GetStringFromUrlPartInternal(UriTextRangeType& range)
    {
      if (range.first == nullptr || (range.afterLast == nullptr))
      {
        return UrlReturnType();
      }

      return UrlReturnType(range.first, range.afterLast);
    }
  } // namespace internal

  template <class UrlReturnType>
  struct UriQueryItem
  {
    UrlReturnType key;
    UrlReturnType value;
  };

  template <class UrlReturnType>
  class UriQuery:
    public std::vector<UriQueryItem<UrlReturnType>>
  {
  public:
    typedef typename std::vector<UriQueryItem<UrlReturnType>> ContainerType;
    typedef typename ContainerType::const_iterator QueryItemType;

    boost::optional<std::list<QueryItemType>> findKeyEntries(UrlReturnType keyStr) const
    {
      std::list<QueryItemType> result;
      for (auto item = std::begin(*this); item != std::end(*this); ++item)
      {
        if (item->key.compare(keyStr) == 0)
        {
          result.push_back(item);
        }
      }
      return result.empty() ? boost::optional<std::list<QueryItemType>>() : result;
    }

    boost::optional<std::list<QueryItemType>> findValueEntries(UrlReturnType valueStr) const
    {
      std::list<QueryItemType> result;
      for (auto item = std::begin(*this); item != std::end(*this); ++item)
      {
        if (item->value.compare(valueStr) == 0)
        {
          result.push_back(item);
        }
      }
      return result.empty() ? boost::optional<std::list<QueryItemType>>() : result;
    }

    // pick first occurrence of key
    QueryItemType findKey(UrlReturnType keyStr) const
    {
      auto entries = findKeyEntries(keyStr);
      if (entries.is_initialized() && !(*entries).empty())
      {
        return (*entries).front();
      }
      return ContainerType::end();
    }

    // pick first occurrence of value
    QueryItemType findValue(UrlReturnType valueStr) const
    {
      auto entries = findValueEntries(valueStr);
      if (entries.is_initialized() && !(*entries).empty())
      {
        return (*entries).front();
      }
      return ContainerType::end();
    }
  };

  // free helper functions
  template <class UrlTextType, class UrlReturnType>
  bool UnescapeString(
    UrlTextType srcStrBegin,
    UrlReturnType& retVal,
    bool plusToSpace = true,
    UriBreakConversion breakConversion = URI_BR_DONT_TOUCH)
  {
    typedef typename internal::base_const_ptr<UrlTextType>::type(*uriUnescapeInPlaceEx)
      (typename internal::base_ptr<UrlTextType>::type, UriBool, UriBreakConversion);

    UrlReturnType reslt(srcStrBegin);

    if (reslt.empty())
    {
      return false;
    }

    uriUnescapeInPlaceEx uriUnescapeInPlaceExProc =
      std::is_convertible<UrlTextType, const char*>::value ?
      (uriUnescapeInPlaceEx)&uriUnescapeInPlaceExA : (uriUnescapeInPlaceEx)&uriUnescapeInPlaceExW;

    uriUnescapeInPlaceExProc(
      &reslt.front(),
      plusToSpace ? URI_TRUE : URI_FALSE,
      breakConversion);

    retVal = std::move(reslt);
    return true;
  }

} // namespace uri_parser