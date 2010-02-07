/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2009 Ali Polatel <alip@exherbo.org>
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

#include <cstring>
#include <tfile.h>
#include <fileref.h>
#include <flacfile.h>
#include <mpcfile.h>
#include <mpegfile.h>
#include <oggflacfile.h>
#include <speexfile.h>
#include <vorbisfile.h>

#include <magic.h>

#include "envmime.hh"

using namespace TagLib;

File *MagicFileTypeResolver::createFile(FileName fileName,
        bool readProperties,
        AudioProperties::ReadStyle propertiesStyle) const
{
    const char *desc;
    static magic_t db = NULL;

    if (NULL == db) {
        db = magic_open(MAGIC_SYMLINK);
        if (NULL == db)
            return 0;
        if (0 > magic_load(db, NULL)) {
            magic_close(db);
            db = NULL;
            return 0;
        }
    }

    desc = magic_file(db, fileName);
    if (NULL == desc)
        return 0;

    if (0 == strncmp(desc, "MPEG ADTS", 9)) {
        MPEG::File *f = new MPEG::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
    else if (0 == strncmp(desc, "Ogg data, FLAC audio", 20)) {
        Ogg::FLAC::File *f = new Ogg::FLAC::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
    else if (0 == strncmp(desc, "Ogg data, Speex audio", 21)) {
        Ogg::Speex::File *f = new Ogg::Speex::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
    else if (0 == strncmp(desc, "Ogg data, Vorbis audio", 22)) {
        Ogg::Vorbis::File *f = new Ogg::Vorbis::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
    else if (0 == strncmp(desc, "FLAC audio", 10)) {
        FLAC::File *f = new FLAC::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
    else if (0 == strncmp(desc, "Musepack audio", 14)) {
        MPC::File *f = new MPC::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
#if 0
    // Not supported by libmagic yet...
    else if (0 == strncmp(desc, "Wavpack audio", 13)) {
        WavPack::File *f = new WavPack::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
#endif
    return 0;
}
