/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <cstddef>
#include <memory>

namespace concrete {

template <typename ResourceType, typename... Args>
ResourceManager::Allocation<ResourceType> ResourceManager::new_resource(Args... args)
{
	std::unique_ptr<ResourceType> resource(new ResourceType(args...));
	auto id = append_resource(resource.get());
	auto ptr = resource.get();
	resource.release();

	return Allocation<ResourceType> {
		id,
		*ptr,
	};
}

template <typename ResourceType>
ResourceType &ResourceManager::resource(ResourceId id) const
{
	auto plain = find_resource(id);
	if (plain == NULL)
		throw ResourceError();

	auto typed = dynamic_cast<ResourceType *> (plain);
	if (typed == NULL)
		throw ResourceError();

	return *typed;
}

} // namespace
