/**
*** Copyright (c) 2016-present,
*** Jaguar0625, gimre, BloodyRookie, Tech Bureau, Corp. All rights reserved.
***
*** This file is part of Catapult.
***
*** Catapult is free software: you can redistribute it and/or modify
*** it under the terms of the GNU Lesser General Public License as published by
*** the Free Software Foundation, either version 3 of the License, or
*** (at your option) any later version.
***
*** Catapult is distributed in the hope that it will be useful,
*** but WITHOUT ANY WARRANTY; without even the implied warranty of
*** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*** GNU Lesser General Public License for more details.
***
*** You should have received a copy of the GNU Lesser General Public License
*** along with Catapult. If not, see <http://www.gnu.org/licenses/>.
**/

#include "catapult/io/PodIoUtils.h"
#include "tests/test/core/mocks/MockMemoryStream.h"
#include "tests/TestHarness.h"

namespace catapult { namespace io {

#define TEST_CLASS PodIoUtilsTests

	namespace {
		struct Traits64 {
			static constexpr uint64_t Value = 0x12345678'90ABCDEFull;

			static void Write(mocks::MockMemoryStream& stream) {
				io::Write64(stream, Value);
			}

			static auto Read(mocks::MockMemoryStream& stream) {
				return io::Read64(stream);
			}
		};

		struct Traits32 {
			static constexpr uint32_t Value = 0x12345678ul;

			static void Write(mocks::MockMemoryStream& stream) {
				io::Write32(stream, Value);
			}

			static auto Read(mocks::MockMemoryStream& stream) {
				return io::Read32(stream);
			}
		};

		struct Traits8 {
			static constexpr uint8_t Value = 0x12u;

			static void Write(mocks::MockMemoryStream& stream) {
				io::Write8(stream, Value);
			}

			static auto Read(mocks::MockMemoryStream& stream) {
				return io::Read8(stream);
			}
		};

		template<typename TTraits>
		void AssertCanRoundtripInteger() {
			// Arrange:
			std::vector<uint8_t> data;
			mocks::MockMemoryStream stream("dummy", data);

			// Act:
			TTraits::Write(stream);

			// Sanity:
			EXPECT_EQ(sizeof(TTraits::Value), data.size());

			// Act:
			auto result = TTraits::Read(stream);

			// Assert: (cast for gcc)
			EXPECT_EQ(static_cast<decltype(TTraits::Value)>(TTraits::Value), result);
			EXPECT_EQ(sizeof(TTraits::Value), stream.position());
		}
	}

#define ROUNDTRIP_SIZE_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_64) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<Traits64>(); } \
	TEST(TEST_CLASS, TEST_NAME##_32) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<Traits32>(); } \
	TEST(TEST_CLASS, TEST_NAME##_8) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<Traits8>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	ROUNDTRIP_SIZE_TEST(CanRoundtripInteger) {
		// Assert:
		AssertCanRoundtripInteger<TTraits>();
	}

	namespace {
		struct ReadReturnValueTraits {
			template<typename T>
			static T Read(mocks::MockMemoryStream& stream) {
				// Act: use return value read
				return io::Read<T>(stream);
			}
		};

		struct ReadOutParameterTraits {
			template<typename T>
			static T Read(mocks::MockMemoryStream& stream) {
				// Act: use out parameter read
				T value;
				io::Read(stream, value);
				return value;
			}
		};

		struct WriteTraits {
			template<typename TIo, typename T>
			static void Write(TIo& stream, const T& value) {
				io::Write(stream, value);
			}
		};

		template<typename TReadTraits, typename T>
		T RoundtripPod(const T& source) {
			// Arrange:
			std::vector<uint8_t> data;
			mocks::MockMemoryStream stream("dummy", data);

			// Act:
			WriteTraits::template Write(stream, source);
			return TReadTraits::template Read<T>(stream);
		}

	}

#define ROUNDTRIP_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_ReadReturnValue) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<ReadReturnValueTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_ReadOutParameter) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<ReadOutParameterTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	ROUNDTRIP_TEST(CanRoundtripBaseValue) {
		// Arrange:
		struct Dummy_tag {};
		using DummyValue = utils::BaseValue<uint64_t, Dummy_tag>;
		constexpr DummyValue Expected(0x12345678'90ABCDEFull);

		// Act:
		auto actual = RoundtripPod<TTraits>(Expected);

		// Assert:
		EXPECT_EQ(Expected, actual);
	}

	ROUNDTRIP_TEST(CanRoundtripArray) {
		// Arrange:
		std::array<uint8_t, 10> Expected = { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 } };

		// Act:
		auto actual = RoundtripPod<TTraits>(Expected);

		// Assert:
		EXPECT_EQ(Expected, actual);
	}
}}
