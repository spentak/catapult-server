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

#include "src/plugins/SecretLockTransactionPlugin.h"
#include "src/model/LockNotifications.h"
#include "src/model/SecretLockTransaction.h"
#include "tests/test/LockTransactionUtils.h"
#include "tests/test/core/mocks/MockNotificationSubscriber.h"
#include "tests/test/plugins/TransactionPluginTestUtils.h"

using namespace catapult::model;

namespace catapult { namespace plugins {

#define TEST_CLASS SecretLockTransactionPluginTests

	// region TransactionPlugin

	namespace {
		DEFINE_TRANSACTION_PLUGIN_TEST_TRAITS(SecretLock)
	}

	DEFINE_BASIC_EMBEDDABLE_TRANSACTION_PLUGIN_TESTS(TEST_CLASS, Entity_Type_Secret_Lock)

	PLUGIN_TEST(CanCalculateSize) {
		// Arrange:
		auto pPlugin = TTraits::CreatePlugin();
		typename TTraits::TransactionType transaction;

		// Act:
		auto realSize = pPlugin->calculateRealSize(transaction);

		// Assert:
		EXPECT_EQ(sizeof(typename TTraits::TransactionType), realSize);
	}

	// endregion

	// region accounts extraction

	PLUGIN_TEST(CanExtractAccounts) {
		// Arrange:
		mocks::MockNotificationSubscriber sub;
		auto pPlugin = TTraits::CreatePlugin();
		auto pTransaction = test::CreateTransaction<TTraits>();
		test::FillWithRandomData(pTransaction->Recipient);

		// Act:
		pPlugin->publish(*pTransaction, sub);

		// Assert:
		EXPECT_EQ(5u, sub.numNotifications());
		EXPECT_EQ(1u, sub.numAddresses());
		EXPECT_EQ(0u, sub.numKeys());

		EXPECT_TRUE(sub.contains(pTransaction->Recipient));
	}

	// endregion

	// region duration notification

	PLUGIN_TEST(CanPublishDurationNotification) {
		// Arrange:
		mocks::MockTypedNotificationSubscriber<SecretLockDurationNotification> sub;
		auto pPlugin = TTraits::CreatePlugin();
		auto pTransaction = test::CreateTransaction<TTraits>();
		pTransaction->Duration = test::GenerateRandomValue<BlockDuration>();

		// Act:
		pPlugin->publish(*pTransaction, sub);

		// Assert:
		ASSERT_EQ(1u, sub.numMatchingNotifications());
		const auto& notification = sub.matchingNotifications()[0];
		EXPECT_EQ(pTransaction->Duration, notification.Duration);
	}

	// endregion

	// region lock hash algorithm notification

	PLUGIN_TEST(CanPublishHashAlgorithmNotification) {
		// Arrange:
		mocks::MockTypedNotificationSubscriber<SecretLockHashAlgorithmNotification> sub;
		auto pPlugin = TTraits::CreatePlugin();
		auto pTransaction = test::CreateTransaction<TTraits>();

		// Act:
		pPlugin->publish(*pTransaction, sub);

		// Assert:
		ASSERT_EQ(1u, sub.numMatchingNotifications());
		const auto& notification = sub.matchingNotifications()[0];
		EXPECT_EQ(pTransaction->HashAlgorithm, notification.HashAlgorithm);
	}

	// endregion

	// region secret notification

	namespace {
		template<typename TTransaction>
		void AssertSecretLockNotification(const SecretLockNotification& notification, const TTransaction& transaction) {
			test::AssertBaseLockNotification(notification, transaction);
			EXPECT_EQ(transaction.HashAlgorithm, notification.HashAlgorithm);
			EXPECT_EQ(transaction.Secret, notification.Secret);
			EXPECT_EQ(transaction.Recipient, notification.Recipient);
		}
	}

	PLUGIN_TEST(CanPublishSecretNotification) {
		// Arrange:
		mocks::MockTypedNotificationSubscriber<SecretLockNotification> sub;
		auto pPlugin = TTraits::CreatePlugin();
		auto pTransaction = test::CreateTransaction<TTraits>();

		// Act:
		pPlugin->publish(*pTransaction, sub);

		// Assert:
		ASSERT_EQ(1u, sub.numMatchingNotifications());
		const auto& notification = sub.matchingNotifications()[0];
		AssertSecretLockNotification(notification, *pTransaction);
	}

	// endregion

	// region balance transfer

	PLUGIN_TEST(CanPublishBalanceTransferNotification) {
		// Arrange:
		mocks::MockTypedNotificationSubscriber<model::BalanceTransferNotification> sub;
		auto pPlugin = TTraits::CreatePlugin();
		auto pTransaction = test::CreateTransaction<TTraits>();

		// Act:
		pPlugin->publish(*pTransaction, sub);

		// Assert:
		ASSERT_EQ(1u, sub.numMatchingNotifications());
		const auto& notification = sub.matchingNotifications()[0];
		EXPECT_EQ(model::NotificationChannel::Validator, GetNotificationChannel(notification.Type));
		EXPECT_EQ(pTransaction->Signer, notification.Sender);
		EXPECT_EQ(pTransaction->Mosaic.MosaicId, notification.MosaicId);
		EXPECT_EQ(pTransaction->Mosaic.Amount, notification.Amount);
		EXPECT_EQ(pTransaction->Recipient, notification.Recipient);
	}

	// endregion
}}
