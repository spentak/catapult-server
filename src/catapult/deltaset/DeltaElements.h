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

namespace catapult { namespace deltaset {

	/// Slim wrapper around changed elements.
	template<typename TSet>
	struct DeltaElements {
	public:
		/// Creates new delta elements from \a addedElements, \a removedElements and \a copiedElements.
		constexpr explicit DeltaElements(const TSet& addedElements, const TSet& removedElements, const TSet& copiedElements)
				: Added(addedElements)
				, Removed(removedElements)
				, Copied(copiedElements)
		{}

	public:
		/// Added elements.
		const TSet& Added;

		/// Removed elements.
		const TSet& Removed;

		/// Copied elements.
		const TSet& Copied;
	};
}}
