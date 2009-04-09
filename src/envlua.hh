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

#ifndef ENVTAG_GUARD_ENVLUA_HH
#define ENVTAG_GUARD_ENVLUA_HH 1

#include <lua.hpp>

#define ENV_INIT "ENVTAG_INIT"

lua_State *init_lua(void);
void close_lua(lua_State *L);
int docommand(lua_State *L, int argc, char **argv);

#endif // ENVTAG_GUARD_ENVLUA_HH
