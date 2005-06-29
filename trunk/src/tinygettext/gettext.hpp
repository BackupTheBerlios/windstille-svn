#ifndef __GETTEXT_H__
#define __GETTEXT_H__

#include "tinygettext/tinygettext.hpp"

extern TinyGetText::DictionaryManager* dictionaryManager;

static inline const char* _(const char* message)
{
    return dictionaryManager->get_dictionary().translate(message);
}
#define N_(s)      s

#endif
