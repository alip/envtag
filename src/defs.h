/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Ali Polatel
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

#ifndef ENVTAG_GUARD_DEFS_H
#define ENVTAG_GUARD_DEFS_H 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define ENV_INIT "ENVTAG_INIT"

void __lg(const char *func, size_t len, const char *fmt, ...)
    __attribute__ ((format (printf, 3, 4)));
#define lg(...)     __lg(__func__, __LINE__, __VA_ARGS__)

#endif /* ENVTAG_GUARD_DEFS_H */
