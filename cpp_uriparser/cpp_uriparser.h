#pragma once

#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <stdexcept>
#include <iterator>
#include <vector>
#include <memory>
#include <atomic>
#include "cpp_uriparser_query.h"
#include "uriparser/Uri.h"

namespace uri_parser
{
  template <class UriPathSegmentType, class UrlReturnType>
  class UrlPathIterator: public std::iterator <std::forward_iterator_tag, UriPathSegmentType>
  {
    typedef UrlPathIterator<UriPathSegmentType, UrlReturnType> IteratorType;
  public:
    explicit UrlPathIterator(UriPathSegmentType pathSegment):
      pathSegment_(pathSegment){}
    UrlPathIterator()
    {
      pathSegment_ = CrteateEmptySegment();
    }
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
      return internal::GetStringFromUrlPartInternal<UriTextRangeTypeT, UrlReturnType>(val);
    }

    template <typename OutItType>
    OutItType CreateEndIt() const
    {
      return OutItType(CrteateEmptySegment());
    }

    UriPathSegmentType CrteateEmptySegment() const
    {
      decltype(pathSegment_.text) endTextRange = {nullptr, nullptr};
      return {endTextRange, nullptr, nullptr};
    }

    UriPathSegmentType pathSegment_;
    UrlReturnType returnObjStorage_;
  };

  template <class UrlTextType>
  class UriEntry: boost::noncopyable
  {
    typedef internal::UriTypes<UrlTextType> UriApiTypes;
    typedef typename UriApiTypes::UriObjType UriObjType;
    typedef typename UriApiTypes::UrlReturnType UrlReturnType;
    typedef typename UriApiTypes::UriStateType UriStateType;
    typedef typename UriApiTypes::UriQueryListType UriQueryListType;
    typedef typename UriApiTypes::UriPathSegmentType UriPathSegmentType;
  public:
    UriEntry(UrlTextType urlText) :
      freeMemoryOnClose_(true)
    {
      state_.uri = &uriObj_;
      if (uriTypes_.parseUri(&state_, urlText) != URI_SUCCESS)
      {
        throw std::runtime_error("uriparser: Uri entry creation failed");
      }
    }

    UriEntry(UriEntry&& right) :
      state_(std::move(right.state_)),
      uriObj_(std::move(right.uriObj_)),
      uriTypes_(std::move(right.uriTypes_)),
      freeMemoryOnClose_(true)
    {
      right.freeMemoryOnClose_ = false;
    }

    virtual ~UriEntry()
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

    const UriQuery<UrlReturnType>& Query()
    {
      if (lazy_query_.is_initialized())
      {
        return lazy_query_.get();
      }
      int itemCount;
      UriQueryListType* queryList_;

      if (uriTypes_.uriDissectQueryMalloc(&queryList_, &itemCount, uriObj_.query.first, uriObj_.query.afterLast) != 0)
      {
        static const UriQuery<UrlReturnType> empty;
        return empty;
      }

      UriQueryListType* curQuery{queryList_};
      UriQueryItem<UrlReturnType> keyValue;

      // initializing query object once for the first time
      lazy_query_ = UriQuery<UrlReturnType>();

      if (itemCount > 0)
      {
        lazy_query_->reserve(itemCount);
      }

      for (auto itemIdx = 0; itemIdx < itemCount; ++itemIdx)
      {
        keyValue = {};
        if (curQuery->key)
        {
          keyValue.key = curQuery->key;
        }

        if (curQuery->value)
        {
          keyValue.value = curQuery->value;
        }

        lazy_query_->push_back(std::move(keyValue));
        curQuery = curQuery->next;
      }

      if (queryList_ != nullptr)
      {
        uriTypes_.uriFreeQueryList(queryList_);
      }

      return lazy_query_.get();
    }

    boost::optional<UrlReturnType> Fragment() const
    {
      return GetStringFromUrlPart(uriObj_.fragment);
    }

    UrlPathIterator<UriPathSegmentType, UrlReturnType> PathHead() const
    {
      if (uriObj_.pathHead)
      {
        return UrlPathIterator<UriPathSegmentType, UrlReturnType>();
      }
      return UrlPathIterator<UriPathSegmentType, UrlReturnType>(*uriObj_.pathHead);
    }

    void Normalize()
    {
      uriTypes_.uriNormalizeSyntax(&uriObj_);
    }

    boost::optional<UrlReturnType> GetUnescapedFragment(
      bool plusToSpace = true
      , UriBreakConversion breakConversion = URI_BR_DONT_TOUCH) const
    {
      auto frag = GetStringFromUrlPart(uriObj_.fragment);
      if (!frag.is_initialized())
      {
        return frag;
      }

      UrlReturnType retVal;
      return UnescapeString(frag.get().c_str(), retVal, plusToSpace, breakConversion)
        ? boost::optional<UrlReturnType>(retVal)
        : boost::optional<UrlReturnType>();
    }

    UrlReturnType GetUnescapedUrlString(bool plusToSpace = true, UriBreakConversion breakConversion = URI_BR_DONT_TOUCH) const
    {
      UrlReturnType reslt;
      return UnescapeString(uriObj_.scheme.first, reslt, plusToSpace, breakConversion) ? reslt : UrlReturnType();
    }

  private:
    template <class UriTextRangeType>
    boost::optional<UrlReturnType> GetStringFromUrlPart(UriTextRangeType range) const
    {
      auto retVal = internal::GetStringFromUrlPartInternal<UriTextRangeType, UrlReturnType>(range);
      if (!retVal.empty())
      {
        return boost::optional<UrlReturnType>(retVal);
      }
      return boost::optional<UrlReturnType>();
    }

    UriApiTypes uriTypes_;
    UriStateType state_;
    UriObjType uriObj_;
    boost::optional<UriQuery<UrlReturnType>> lazy_query_;
    std::atomic<bool> freeMemoryOnClose_;
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