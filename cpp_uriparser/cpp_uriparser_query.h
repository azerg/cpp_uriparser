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

    template<class T, class = void>
    struct IsStdString
    {
      static const bool value = false;
    };

    // todo(azerg): tofix
    // dummy was to check if type is std::base_string. Will accept containers too :D :D
    template<class T>
    struct IsStdString <T, typename std::enable_if<
      std::is_object<typename T::const_iterator>::value && \
      std::is_object<typename std::iterator_traits<typename T::const_iterator>::value_type>::value>::type>
    {
      static const bool value = true;
    };

    // default API types
#define URI_API_TYPES(PREFIX) \
      typedef UriUri##PREFIX UriObjType; \
      typedef UriParserState##PREFIX UriStateType; \
      typedef UriPathSegment##PREFIX UriPathSegmentType; \
      typedef UriQueryListStruct##PREFIX UriQueryListType; \
      \
      std::function<int(UriStateType*, UrlTextType)> parseUri; /*NOLINT*/ \
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
    typedef typename ContainerType::const_iterator QueryItemIteratorType;

    // If failed returns end()
    // If succ - iterator to element found
    QueryItemIteratorType findKey(UrlReturnType keyStr) const
    {
      for (auto item = std::begin(*this); item != std::end(*this); ++item)
      {
        if (item->key.compare(keyStr) == 0)
        {
          return item;
        }
      }
      return ContainerType::end();
    }

    // pick first occurrence of value
    QueryItemIteratorType findValue(UrlReturnType valueStr) const
    {
      for (auto item = std::begin(*this); item != std::end(*this); ++item)
      {
        if (item->value.compare(valueStr) == 0)
        {
          return item;
        }
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

    auto uriUnescapeInPlaceExProc =
      std::is_convertible<UrlTextType, const char*>::value ?
      (uriUnescapeInPlaceEx)&uriUnescapeInPlaceExA : (uriUnescapeInPlaceEx)&uriUnescapeInPlaceExW;

    uriUnescapeInPlaceExProc(
      &reslt.front(),
      plusToSpace ? URI_TRUE : URI_FALSE,
      breakConversion);

    retVal = std::move(reslt);
    return true;
  }

  namespace internal
  {
    // char *, wchar*
    template <class UrlTextType>
    auto UnescapeStringBase(
      typename std::enable_if<!internal::IsStdString<UrlTextType>::value, UrlTextType>::type srcStrBegin,
      bool plusToSpace = true,
      UriBreakConversion breakConversion = URI_BR_DONT_TOUCH)
    {
      typedef std::conditional < std::is_convertible<UrlTextType, const wchar_t*>::value, std::wstring, std::string>::type resultType;
      typedef std::conditional < std::is_convertible<UrlTextType, const wchar_t*>::value, const wchar_t*, const char*>::type baseType;
      resultType result;

      if (!UnescapeString(srcStrBegin, result))
      {
        throw std::runtime_error("error unescaping string");
      }

      return result;
    }

    // string, wstring
    template <class UrlTextType>
    auto UnescapeStringBase(
      typename std::enable_if<internal::IsStdString<UrlTextType>::value, UrlTextType>::type srcStrBegin,
      bool plusToSpace = true,
      UriBreakConversion breakConversion = URI_BR_DONT_TOUCH)
    {
      typedef std::conditional < std::is_convertible<UrlTextType, const wchar_t*>::value, const wchar_t*, const char*>::type baseType;
      return UnescapeStringBase<baseType>(srcStrBegin.c_str());
    }
  } // namespace internal


  template <class UrlTextType> // helped proc to help compiler to deduce type of UrlTextType ( this occurs coz of enable_if<..,UrlTextType> )
  auto UnescapeString(
    UrlTextType srcStrBegin,
    bool plusToSpace = true,
    UriBreakConversion breakConversion = URI_BR_DONT_TOUCH)
  {
    return internal::UnescapeStringBase<UrlTextType>(srcStrBegin);
  }

} // namespace uri_parser