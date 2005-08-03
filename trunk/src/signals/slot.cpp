/*
**  ClanLib SDK
**  Copyright (c) 1997-2005 The ClanLib Team
**
**  This software is provided 'as-is', without any express or implied
**  warranty.  In no event will the authors be held liable for any damages
**  arising from the use of this software.
**
**  Permission is granted to anyone to use this software for any purpose,
**  including commercial applications, and to alter it and redistribute it
**  freely, subject to the following restrictions:
**
**  1. The origin of this software must not be misrepresented; you must not
**     claim that you wrote the original software. If you use this software
**     in a product, an acknowledgment in the product documentation would be
**     appreciated but is not required.
**  2. Altered source versions must be plainly marked as such, and must not be
**     misrepresented as being the original software.
**  3. This notice may not be removed or altered from any source distribution.
**
**  Note: Some of the libraries ClanLib may link to may have additional
**  requirements or restrictions.
**
**  File Author(s):
**
**    Magnus Norddahl
**    (if your name is missing here, please add it)
*/

#include "signals/slot.hpp"
#include "signals/slot_generic.hpp"

/////////////////////////////////////////////////////////////////////////////
// Slot construction:

Slot::Slot() : impl(0)
{
}

Slot::Slot(const Slot &copy) : impl(copy.impl)
{
	if (impl != 0) impl->add_slot_ref();
}

Slot::~Slot()
{
	if (impl != 0) impl->release_slot_ref();
}

/////////////////////////////////////////////////////////////////////////////
// Slot attributes:

int Slot::get_slot_ref() const
{
	if (impl) return impl->get_slot_ref();
	else return 0;
}

int Slot::get_signal_ref() const
{
	if (impl) return impl->get_signal_ref();
	else return 0;
}

bool Slot::is_persistent() const
{
	return impl->is_persistent();
}

/////////////////////////////////////////////////////////////////////////////
// Slot operations:

void Slot::operator =(const Slot &copy)
{
	if (impl) impl->release_slot_ref();
	impl = copy.impl;
	if (impl) impl->add_slot_ref();
}

void Slot::set_persistent(bool flag)
{
	if (impl) impl->set_persistent(flag);
}

/////////////////////////////////////////////////////////////////////////////
// Slot implementation:

Slot::Slot(Slot_Generic *impl) : impl(impl)
{
	if (impl != 0) impl->add_slot_ref();
}
