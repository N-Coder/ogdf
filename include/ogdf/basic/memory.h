/** \file
 * \brief Declaration of memory manager for allocating small
 *        pieces of memory
 *
 * \author Carsten Gutwenger
 *
 * \par License:
 * This file is part of the Open Graph Drawing Framework (OGDF).
 *
 * \par
 * Copyright (C)<br>
 * See README.md in the OGDF root directory for details.
 *
 * \par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 or 3 as published by the Free Software Foundation;
 * see the file LICENSE.txt included in the packaging of this file
 * for details.
 *
 * \par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * \par
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see
 * http://www.gnu.org/copyleft/gpl.html
 */

#pragma once

#include <ogdf/basic/basic.h>
#include <ogdf/basic/memory/MallocMemoryAllocator.h>
#include <ogdf/basic/memory/PoolMemoryAllocator.h>

#include <cstddef>

namespace ogdf {

//! @name Managing memory
//! @{

/**
 * Creates new and delete operators in a class using the given memory allocator.
 *
 * In other words, adding this macro in a class declaration makes that class
 * managed by the respective memory manager.
 * Throws an ogdf::InsufficientMemoryException if no more memory is available.
 */
#define OGDF_MM(Alloc)                                              \
public:                                                             \
	static void* operator new(size_t nBytes) {                      \
		if (OGDF_LIKELY(Alloc::checkSize(nBytes)))                  \
			return Alloc::allocate(nBytes);                         \
		else                                                        \
			return ogdf::MallocMemoryAllocator::allocate(nBytes);   \
	}                                                               \
                                                                    \
	static void operator delete(void* p, size_t nBytes) {           \
		if (OGDF_LIKELY(p != 0)) {                                  \
			if (OGDF_LIKELY(Alloc::checkSize(nBytes)))              \
				Alloc::deallocate(nBytes, p);                       \
			else                                                    \
				ogdf::MallocMemoryAllocator::deallocate(nBytes, p); \
		}                                                           \
	}                                                               \
	static void* operator new(size_t, void* p) { return p; }        \
	static void operator delete(void*, void*) { }

#ifdef OGDF_MEMORY_MALLOC_TS
#	define OGDF_ALLOCATOR ogdf::MallocMemoryAllocator
#else
//! The used memory manager
#	define OGDF_ALLOCATOR ogdf::PoolMemoryAllocator
#endif

/**
 * Makes the class use OGDF's memory allocator.
 * @copydoc OGDF_MM
 * @ingroup macros
 */
#define OGDF_NEW_DELETE OGDF_MM(OGDF_ALLOCATOR)

/**
 * Makes the class use malloc for memory allocation.
 * @copydoc OGDF_MM
 * @ingroup macros
 */
#define OGDF_MALLOC_NEW_DELETE OGDF_MM(ogdf::MallocMemoryAllocator)

/**
 * Encapsulates OGDF_ALLOCATOR in a class that can be used as the Allocator for
 * containers of the C++ standard library.
 */
template<class T>
struct OGDFAllocator {
	using value_type = T;

	OGDFAllocator() noexcept = default;

	template<class U>
	OGDFAllocator(const OGDFAllocator<U>&) noexcept { }

	template<class U>
	bool operator==(const OGDFAllocator<U>&) const noexcept {
		return true;
	}

	template<class U>
	bool operator!=(const OGDFAllocator<U>&) const noexcept {
		return false;
	}

	T* allocate(const size_t n) const {
		const size_t s = n * sizeof(T);
		if (OGDF_LIKELY(OGDF_ALLOCATOR::checkSize(s))) {
			return static_cast<T*>(OGDF_ALLOCATOR::allocate(s));
		} else {
			return static_cast<T*>(ogdf::MallocMemoryAllocator::allocate(s));
		}
	}

	void deallocate(T* const p, size_t n) const noexcept {
		if (OGDF_LIKELY(p != nullptr)) {
			const size_t s = n * sizeof(T);
			if (OGDF_LIKELY(OGDF_ALLOCATOR::checkSize(s))) {
				OGDF_ALLOCATOR::deallocate(s, p);
			} else {
				ogdf::MallocMemoryAllocator::deallocate(s, p);
			}
		}
	}
};

//! @}

}
