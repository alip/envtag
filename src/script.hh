/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Ali Polatel <polatel@gmail.com>
 *
 * This file is part of the envtag audio tagger. envtag is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * envtag is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef ENVTAG_GUARD_SCRIPT_HH
#define ENVTAG_GUARD_SCRIPT_HH 1

#include <string>

#include <lua.hpp>
#include <fileref.h>

#include "defs.hh"

lua_State *init_lua(void);
std::string getscript(const char *script);
int dobuiltin(const char *name, lua_State *L, TagLib::FileRef *f,
        const char *file, struct envtag_opts opts,
        int count, int total);
int doscript(const char *script, lua_State *L, TagLib::FileRef *f,
        const char *file, struct envtag_opts opts,
        int count, int total);

#endif // ENVTAG_GUARD_SCRIPT_HH
