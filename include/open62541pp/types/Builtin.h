#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "open62541pp/ErrorHandling.h"
#include "open62541pp/TypeWrapper.h"
#include "open62541pp/open62541.h"

namespace opcua {

/**
 * UA_StatusCode wrapper class.
 * This type is not derived from TypeWrapper. @ref UA_StatusCode is just an alias for `uint32_t`.
 * StatusCode can be used interchangeably with @ref UA_StatusCode due to implicit conversions
 * (without any overhead) but provides some methods to simplify the handling with status codes.
 * @see statuscodes.h
 * @see https://reference.opcfoundation.org/Core/Part4/v105/docs/7.39
 * @ingroup TypeWrapper
 */
class StatusCode {
public:
    /// Create a StatusCode with the default status code `UA_STATUSCODE_GOOD`.
    constexpr StatusCode() noexcept = default;

    constexpr StatusCode(UA_StatusCode code) noexcept  // NOLINT, implicit wanted
        : code_(code) {}

    /// Implicit conversion to UA_StatusCode.
    constexpr operator UA_StatusCode() noexcept {  // NOLINT, implicit wanted
        return code_;
    }

    /// Explicitly get underlying UA_StatusCode.
    constexpr UA_StatusCode get() const noexcept {
        return code_;
    }

    /// Get human-readable name of the StatusCode.
    /// This feature might be disabled to create a smaller binary with the
    /// `UA_ENABLE_STATUSCODE_DESCRIPTIONS` build-flag. Then the function returns an empty string
    /// for every StatusCode.
    std::string_view name() const noexcept {
        return {UA_StatusCode_name(code_)};
    }

    /// Check if the status code is good.
    constexpr bool isGood() const noexcept {
        return detail::isGoodStatus(code_);
    }

    /// Check if the status code is uncertain.
    constexpr bool isUncertain() const noexcept {
        return detail::isUncertainStatus(code_);
    }

    /// Check if the status code is bad.
    constexpr bool isBad() const noexcept {
        return detail::isBadStatus(code_);
    }

    /// Throw a BadStatus exception if the status code is bad.
    /// @exception BadStatus
    void throwIfBad() const {
        detail::throwOnBadStatus(code_);
    }

private:
    UA_StatusCode code_{};
};

constexpr bool operator==(StatusCode lhs, UA_StatusCode rhs) noexcept {
    return lhs.get() == rhs;
}

constexpr bool operator==(UA_StatusCode lhs, StatusCode rhs) noexcept {
    return lhs == rhs.get();
}

constexpr bool operator!=(StatusCode lhs, UA_StatusCode rhs) noexcept {
    return !(lhs == rhs);
}

constexpr bool operator!=(UA_StatusCode lhs, StatusCode rhs) noexcept {
    return !(lhs == rhs);
}

/**
 * UA_String wrapper class.
 * @ingroup TypeWrapper
 */
class String : public TypeWrapper<UA_String, UA_TYPES_STRING> {
public:
    // NOLINTNEXTLINE, false positive?
    using TypeWrapperBase::TypeWrapperBase;  // inherit constructors

    explicit String(std::string_view str);

    bool empty() const noexcept;

    std::string_view get() const;
};

bool operator==(const String& lhs, std::string_view rhs) noexcept;
bool operator!=(const String& lhs, std::string_view rhs) noexcept;
bool operator==(std::string_view lhs, const String& rhs) noexcept;
bool operator!=(std::string_view lhs, const String& rhs) noexcept;

/**
 * UA_Guid wrapper class.
 * @ingroup TypeWrapper
 */
class Guid : public TypeWrapper<UA_Guid, UA_TYPES_GUID> {
public:
    // NOLINTNEXTLINE, false positive?
    using TypeWrapperBase::TypeWrapperBase;  // inherit constructors

    Guid(uint32_t data1, uint16_t data2, uint16_t data3, std::array<uint8_t, 8> data4);

    static Guid random();

    std::string toString() const;
};

/**
 * UA_ByteString wrapper class.
 * @ingroup TypeWrapper
 */
class ByteString : public TypeWrapper<UA_ByteString, UA_TYPES_BYTESTRING> {
public:
    // NOLINTNEXTLINE, false positive?
    using TypeWrapperBase::TypeWrapperBase;  // inherit constructors

    explicit ByteString(std::string_view str);
    explicit ByteString(const std::vector<uint8_t>& bytes);

    /// Read ByteString from binary file.
    static ByteString fromFile(const std::filesystem::path& filepath);

    /// Parse ByteString from Base64 encoded string.
    /// @note Only supported since open62541 v1.1
    static ByteString fromBase64(std::string_view encoded);

    /// Write ByteString to binary file.
    void toFile(const std::filesystem::path& filepath) const;

    /// Convert to Base64 encoded string.
    /// @note Only supported since open62541 v1.1
    std::string toBase64() const;

    bool empty() const noexcept;

    std::string_view get() const;
};

bool operator==(const ByteString& lhs, std::string_view rhs) noexcept;
bool operator!=(const ByteString& lhs, std::string_view rhs) noexcept;
bool operator==(std::string_view lhs, const ByteString& rhs) noexcept;
bool operator!=(std::string_view lhs, const ByteString& rhs) noexcept;

/**
 * UA_XmlElement wrapper class.
 * @ingroup TypeWrapper
 */
class XmlElement : public TypeWrapper<UA_XmlElement, UA_TYPES_XMLELEMENT> {
public:
    // NOLINTNEXTLINE, false positive?
    using TypeWrapperBase::TypeWrapperBase;  // inherit constructors

    explicit XmlElement(std::string_view str);

    bool empty() const noexcept;

    std::string_view get() const;
};

/**
 * UA_QualifiedName wrapper class.
 * @ingroup TypeWrapper
 */
class QualifiedName : public TypeWrapper<UA_QualifiedName, UA_TYPES_QUALIFIEDNAME> {
public:
    // NOLINTNEXTLINE, false positive?
    using TypeWrapperBase::TypeWrapperBase;  // inherit constructors

    QualifiedName(uint16_t namespaceIndex, std::string_view name);

    uint16_t getNamespaceIndex() const noexcept;

    std::string_view getName() const;
};

/**
 * UA_LocalizedText wrapper class.
 * The format of locale is `<language>[-<country/region>]`:
 * - `<language>` is the two letter ISO 639 code for a language
 * - `<country/region>` is the two letter ISO 3166 code for the country/region
 * @see https://reference.opcfoundation.org/Core/Part3/v105/docs/8.5/
 * @see https://reference.opcfoundation.org/Core/Part3/v105/docs/8.4/
 * @ingroup TypeWrapper
 */
class LocalizedText : public TypeWrapper<UA_LocalizedText, UA_TYPES_LOCALIZEDTEXT> {
public:
    // NOLINTNEXTLINE, false positive?
    using TypeWrapperBase::TypeWrapperBase;  // inherit constructors

    LocalizedText(std::string_view locale, std::string_view text, bool assertLocaleFormat = true);

    std::string_view getText() const;

    std::string_view getLocale() const;
};

using NumericRangeDimension = UA_NumericRangeDimension;

bool operator==(const NumericRangeDimension& lhs, const NumericRangeDimension& rhs) noexcept;
bool operator!=(const NumericRangeDimension& lhs, const NumericRangeDimension& rhs) noexcept;

/**
 * Numeric range to indicate subsets of (multidimensional) arrays.
 * They are no official data type in the OPC UA standard and are transmitted only with a string
 * encoding, such as "1:2,0:3,5". The colon separates min/max index and the comma separates
 * dimensions. A single value indicates a range with a single element (min==max).
 * @see https://reference.opcfoundation.org/Core/Part4/v105/docs/7.27
 */
class NumericRange {
public:
    NumericRange();
    explicit NumericRange(std::string_view encodedRange);
    explicit NumericRange(std::vector<NumericRangeDimension> dimensions);
    explicit NumericRange(const UA_NumericRange& native);

    bool empty() const noexcept;

    const std::vector<NumericRangeDimension>& get() const noexcept;

    std::string toString() const;

private:
    std::vector<NumericRangeDimension> dimensions_;
};

}  // namespace opcua
