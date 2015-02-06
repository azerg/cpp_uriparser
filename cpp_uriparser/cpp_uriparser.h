#pragma once

#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <exception>
#include <iterator>
#include <memory>
#include "cpp_uriparser_query.h"
#include "uriparser/Uri.h"

namespace uri_parser
{
  template <class UrlTextType, class Empty = void>
  class UriTypes
  { UriTypes(); };

  template <class UrlTextType>
  struct UriTypes<UrlTextType, typename std::enable_if<std::is_convertible<UrlTextType, const char*>::value >::type>
  {
    typedef UriUriA UriObjType;
    typedef UriParserStateA UriStateType;
    typedef UriPathSegmentA UriPathSegmentType;

    std::function<int(UriStateType*, UrlTextType)> parseUri;
    std::function<void(UriObjType*)> freeUriMembers;
    std::function<int(UriObjType*)> uriNormalizeSyntax;
    std::function<UrlTextType(char*, UriBool, UriBreakConversion)> uriUnescapeInPlaceEx;

    UriTypes() :
      parseUri(&uriParseUriA),
      freeUriMembers(&uriFreeUriMembersA),
      uriNormalizeSyntax(&uriNormalizeSyntaxA),
      uriUnescapeInPlaceEx(&uriUnescapeInPlaceExA)
    {}
  };

  template <class UrlTextType>
  struct UriTypes<UrlTextType, typename std::enable_if<std::is_convertible<UrlTextType, const wchar_t*>::value >::type>
  {
    typedef UriUriW UriObjType;
    typedef UriParserStateW UriStateType;
    typedef UriPathSegmentW UriPathSegmentType;

    std::function<int(UriStateType*, UrlTextType)> parseUri;
    std::function<void(UriObjType*)> freeUriMembers;
    std::function<int(UriObjType*)> uriNormalizeSyntax;
    std::function<UrlTextType(wchar_t*, UriBool, UriBreakConversion)> uriUnescapeInPlaceEx;

    UriTypes() :
      parseUri(&uriParseUriW),
      freeUriMembers(&uriFreeUriMembersW),
      uriNormalizeSyntax(&uriNormalizeSyntaxW),
      uriUnescapeInPlaceEx(&uriUnescapeInPlaceExW)
    {}
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

  template <class UriPathSegmentType, class UrlReturnType>
  class UrlPathIterator: public std::iterator <std::forward_iterator_tag, UriPathSegmentType>
  {
    typedef UrlPathIterator<UriPathSegmentType, UrlReturnType> IteratorType;
  public:
    explicit UrlPathIterator(UriPathSegmentType pathSegment):
      pathSegment_(pathSegment){}
    UrlPathIterator(const UrlPathIterator& mit):
      pathSegment_(mit.pathSegment_),
      returnObjStorage_(mit.returnObjStorage_){}

    IteratorType& operator++()
    {
      if (pathSegment_.next != nullptr)
      {
        pathSegment_ = *pathSegment_.next;
      }
      else
      {
        *this = CreateEndIt<const IteratorType>();
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
      return ((pathSegment_.text.afterLast == right.pathSegment_.text.afterLast) &&
        (pathSegment_.next == right.pathSegment_.next));
    }

    bool operator!=(const IteratorType& right)
    {
      return !operator==(right);
    }

    UrlReturnType operator*() const { return GetStringFromUrlPart(pathSegment_.text); }
    UrlReturnType* operator->()
    {
      returnObjStorage_ = std::move(operator*());
      return &returnObjStorage_;
    }

    IteratorType begin() const { return UrlPathIterator(pathSegment_); }
    const IteratorType cbegin() const { return UrlPathIterator(pathSegment_); }
    IteratorType end() const { return CreateEndIt<IteratorType>(); }
    const IteratorType cend() const { return CreateEndIt<const IteratorType>(); }
  private:
    template <typename UriTextRangeTypeT>
    UrlReturnType GetStringFromUrlPart(UriTextRangeTypeT val) const
    {
      return GetStringFromUrlPartInternal<UriTextRangeTypeT, UrlReturnType>(val);
    }

    template <typename OutItType>
    OutItType CreateEndIt() const
    {
      decltype(pathSegment_.text) endTextRange = {nullptr, nullptr};
      UriPathSegmentType endPathSegment = {endTextRange, nullptr, nullptr};
      return OutItType(endPathSegment);
    }

    UriPathSegmentType pathSegment_;
    UrlReturnType returnObjStorage_;
  };

  template <class UrlTextType, class UrlReturnType>
  class UriEntryBase: private boost::noncopyable
  {
    typedef UriTypes<UrlTextType> UriApiTypes;
    typedef typename UriApiTypes::UriObjType UriObjType;
    typedef typename UriApiTypes::UriStateType UriStateType;
    typedef typename UriApiTypes::UriPathSegmentType UriPathSegmentType;
  public:
    virtual ~UriEntryBase()
    {
      if (freeMemoryOnClose_)
      {
        uriTypes_.freeUriMembers(&uriObj_);
      }
    }

    boost::optional<UrlReturnType> Scheme() const
    {
      return GetStringFromUrlPart(uriObj_.scheme);
    }

    boost::optional<UrlReturnType> UserInfo() const
    {
      return GetStringFromUrlPart(uriObj_.userInfo);
    }

    boost::optional<UrlReturnType> HostText() const
    {
      return GetStringFromUrlPart(uriObj_.hostText);
    }

    UriQueryList<UrlTextType> Query() const
    {
      return UriQueryList<UrlTextType>(uriObj_);
    }

    boost::optional<UrlReturnType> Fragment() const
    {
      return GetStringFromUrlPart(uriObj_.fragment);
    }

    UrlPathIterator<UriPathSegmentType, UrlReturnType> PathHead() const
    {
      return UrlPathIterator<UriPathSegmentType, UrlReturnType>(*uriObj_.pathHead);
    }

    void Normalize()
    {
      uriTypes_.uriNormalizeSyntax(&uriObj_);
    }

  protected:
    UriEntryBase(UrlTextType urlText):
      freeMemoryOnClose_(true)
    {
      state_.uri = &uriObj_;
      if (uriTypes_.parseUri(&state_, urlText) != URI_SUCCESS)
      {
        throw std::runtime_error("uriparser: Uri entry creation failed");
      }
    }

    UriEntryBase(UriEntryBase&& right):
      state_(std::move(right.state_)),
      uriObj_(std::move(right.uriObj_)),
      freeMemoryOnClose_(true)
    {
      right.freeMemoryOnClose_ = false;
    }

  private:
    template <class UriTextRangeType>
    boost::optional<UrlReturnType> GetStringFromUrlPart(UriTextRangeType range) const
    {
      auto retVal = GetStringFromUrlPartInternal<UriTextRangeType, UrlReturnType>(range);
      if (!retVal.empty())
      {
        return boost::optional<UrlReturnType>(retVal);
      }
      return boost::optional<UrlReturnType>();
    }

    UriApiTypes uriTypes_;

    UriStateType state_;
    UriObjType uriObj_;
    bool freeMemoryOnClose_;
  };


  template <class UrlTextType, class Empty = void>
  class UriEntry;

  template <class UrlTextType>
  class UriEntry < UrlTextType, typename std::enable_if<std::is_convertible<UrlTextType, const char*>::value >::type>:
    public UriEntryBase<UrlTextType, std::string>
  {
  public:
    explicit UriEntry(UrlTextType url):
      UriEntryBase(url){}
    UriEntry(UriEntry&& right):
      UriEntryBase(std::move(right)){}
  };

  template <class UrlTextType>
  class UriEntry < UrlTextType, typename std::enable_if<std::is_convertible<UrlTextType, const wchar_t*>::value >::type>:
    public UriEntryBase<UrlTextType, std::wstring>
  {
  public:
    explicit UriEntry(UrlTextType url):
      UriEntryBase(url){}
    UriEntry(UriEntry&& right):
      UriEntryBase(std::move(right)){}
  };

  // Use this helper proc to create UriEntry obj
  template <typename T>
  UriEntry<T> UriParseUrl(T url)
  {
    return UriEntry<T>(url);
  }
    
  template <typename T>
  UriEntry<T> ParseUrlWithHost(T url)
  {
    UriEntry<T> retVal(url);

    auto hostText = retVal.HostText();

    if (!hostText.is_initialized())
    {
      throw std::runtime_error("uriparser: Expected url with host, but no host found");
    }

    return retVal;
  }
} // namespace uri_parser