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

#pragma once
#include "catapult/cache/CacheConfiguration.h"
#include "catapult/cache/CatapultCache.h"
#include "catapult/cache/SubCachePluginAdapter.h"
#include <memory>
#include <vector>

namespace catapult { namespace cache { class ReadOnlyCatapultCache; } }

namespace catapult { namespace test {

	/// Cache factory for creating a catapult cache composed of all core subcaches.
	struct CoreSystemCacheFactory {
		/// Creates an empty catapult cache around \a config.
		static cache::CatapultCache Create(const model::BlockChainConfiguration& config);

		/// Adds all core subcaches initialized with \a config to \a subCaches.
		static void CreateSubCaches(
				const model::BlockChainConfiguration& config,
				std::vector<std::unique_ptr<cache::SubCachePlugin>>& subCaches);
	};

	/// Creates a subcache plugin given \a args for a plugin that doesn't require configuration.
	template<typename TCache, typename TStorageTraits, typename... TArgs>
	std::unique_ptr<cache::SubCachePlugin> MakeConfigurationFreeSubCachePlugin(TArgs&&... args) {
		auto pCache = std::make_unique<TCache>(std::forward<TArgs>(args)...);
		return std::make_unique<cache::SubCachePluginAdapter<TCache, TStorageTraits>>(std::move(pCache));
	}

	/// Creates a subcache plugin given \a args.
	template<typename TCache, typename TStorageTraits, typename... TArgs>
	std::unique_ptr<cache::SubCachePlugin> MakeSubCachePlugin(TArgs&&... args) {
		return MakeConfigurationFreeSubCachePlugin<TCache, TStorageTraits>(cache::CacheConfiguration(), std::forward<TArgs>(args)...);
	}

	/// Creates an empty catapult cache.
	cache::CatapultCache CreateEmptyCatapultCache();

	/// Creates an empty catapult cache around \a config.
	cache::CatapultCache CreateEmptyCatapultCache(const model::BlockChainConfiguration& config);

	/// Creates an empty catapult cache around \a config.
	template<typename TCacheFactory>
	cache::CatapultCache CreateEmptyCatapultCache(const model::BlockChainConfiguration& config) {
		return TCacheFactory::Create(config);
	}

	/// Creates a catapult cache with a marker account.
	cache::CatapultCache CreateCatapultCacheWithMarkerAccount();

	/// Returns \c true if \a cache contains the marker account.
	bool IsMarkedCache(const cache::ReadOnlyCatapultCache& cache);

	/// Returns \c true if \a cache contains the marker account.
	bool IsMarkedCache(const cache::CatapultCacheDelta& cache);

	/// Extract all values from \a cache.
	template<typename TCache, typename TValue>
	std::vector<TValue> ExtractValuesFromCache(const TCache& cache) {
		std::vector<TValue> values;
		auto view = cache.createView();
		for (const auto& pair : *view)
			values.push_back(pair.second);

		return values;
	}
}}
