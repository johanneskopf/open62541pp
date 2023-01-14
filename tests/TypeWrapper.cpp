#include <utility>  // move

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "open62541pp/Helper.h"  // detail::toString
#include "open62541pp/TypeWrapper.h"
#include "open62541pp/Types.h"

using namespace Catch::Matchers;
using namespace opcua;

TEST_CASE("TypeWrapper") {
    SECTION("Constructor") {
        // Empty constructor
        REQUIRE_NOTHROW(TypeWrapper<bool, Type::Boolean>());

        // Constructor with wrapped type (lvalue)
        {
            bool value{true};
            TypeWrapper<bool, Type::Boolean> wrapper(value);
            REQUIRE(*wrapper.handle() == value);
        }

        // Constructor with wrapped type (rvalue)
        {
            TypeWrapper<double, Type::Double> wrapper(11.11);
            REQUIRE(*wrapper.handle() == 11.11);
        }
    }

    SECTION("Copy constructor / assignment") {
        TypeWrapper<UA_String, Type::String> wrapper1(UA_STRING_ALLOC("String1"));
        TypeWrapper<UA_String, Type::String> wrapper2(UA_STRING_ALLOC("String2"));

        TypeWrapper<UA_String, Type::String> wrapperConstructor(wrapper1);
        REQUIRE(wrapperConstructor.handle()->data != wrapper1.handle()->data);
        REQUIRE_THAT(detail::toString(*wrapperConstructor.handle()), Equals("String1"));

        TypeWrapper<UA_String, Type::String> wrapperAssignmnet = wrapper2;
        REQUIRE(wrapperAssignmnet.handle()->data != wrapper2.handle()->data);
        REQUIRE_THAT(detail::toString(*wrapperAssignmnet.handle()), Equals("String2"));
    }

    SECTION("Move constructor / assignment") {
        TypeWrapper<UA_String, Type::String> wrapper1(UA_STRING_ALLOC("String1"));
        TypeWrapper<UA_String, Type::String> wrapper2(UA_STRING_ALLOC("String2"));

        TypeWrapper<UA_String, Type::String> wrapperConstructor(std::move(wrapper1));
        // wrapper1 still points to same data, but doesn't own it anymore
        REQUIRE(wrapperConstructor.handle()->data == wrapper1.handle()->data);

        TypeWrapper<UA_String, Type::String> wrapperAssignmnet = std::move(wrapper2);
        // wrapper2 still points to same data, but doesn't own it anymore
        REQUIRE(wrapperAssignmnet.handle()->data == wrapper2.handle()->data);
    }

    SECTION("Get type") {
        TypeWrapper<float, Type::Float> wrapperFloat;
        REQUIRE(wrapperFloat.getType() == Type::Float);
        REQUIRE(wrapperFloat.getDataType() == detail::getUaDataType(Type::Float));

        TypeWrapper<UA_String, Type::String> wrapperString;
        REQUIRE(wrapperString.getType() == Type::String);
        REQUIRE(wrapperString.getDataType() == detail::getUaDataType(Type::String));
    }
}

TEST_CASE("String") {
    String s("test");
    REQUIRE(s.handle()->length == 4);
    REQUIRE_THAT(s.get(), Equals("test"));
}

TEST_CASE("String from non-null-terminated view") {
    std::string str("test123");
    std::string_view sv(str.c_str(), 4);
    String s(sv);
    REQUIRE_THAT(s.get(), Equals("test"));
}

TEST_CASE("Guid wrapper") {
    UA_UInt32 data1{11};
    UA_UInt16 data2{22};
    UA_UInt16 data3{33};
    std::array<UA_Byte, 8> data4{1, 2, 3, 4, 5, 6, 7, 8};

    Guid wrapper(data1, data2, data3, data4);

    REQUIRE(wrapper.handle()->data1 == data1);
    REQUIRE(wrapper.handle()->data2 == data2);
    REQUIRE(wrapper.handle()->data3 == data3);
    for (int i = 0; i < 8; ++i) {
        REQUIRE(wrapper.handle()->data4[i] == data4[i]);  // NOLINT
    }
}
