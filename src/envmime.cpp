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

TagLib::File *MagicFileTypeResolver::createFile(TagLib::FileName fileName,
        bool readProperties,
        TagLib::AudioProperties::ReadStyle propertiesStyle) const
{
    const char *desc;
    static magic_t db = NULL;

    if (NULL == db) {
        db = magic_open(MAGIC_NONE);
        if (NULL == db)
            return 0;
        if (0 > magic_load(db, NULL))
            return 0;
    }

    desc = magic_file(db, fileName);
    if (NULL == desc)
        return 0;

    if (0 == strncmp(desc, "MPEG ADTS", 9)) {
        TagLib::MPEG::File *f = new TagLib::MPEG::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
    else if (0 == strncmp(desc, "Ogg data, FLAC audio", 20)) {
        TagLib::Ogg::FLAC::File *f = new TagLib::Ogg::FLAC::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
    else if (0 == strncmp(desc, "Ogg data, Speex audio", 21)) {
        TagLib::Ogg::Speex::File *f = new TagLib::Ogg::Speex::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
    else if (0 == strncmp(desc, "Ogg data, Vorbis audio", 22)) {
        TagLib::Ogg::Vorbis::File *f = new TagLib::Ogg::Vorbis::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
    else if (0 == strncmp(desc, "FLAC audio", 10)) {
        TagLib::FLAC::File *f = new TagLib::FLAC::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
    else if (0 == strncmp(desc, "Musepack audio", 14)) {
        TagLib::MPC::File *f = new TagLib::MPC::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
#if 0
    // Not supported by libmagic yet...
    else if (0 == strncmp(desc, "Wavpack audio", 13)) {
        TagLib::WavPack::File *f = new TagLib::WavPack::File(fileName, readProperties, propertiesStyle);
        if (f->isValid())
            return f;
        else
            delete f;
    }
#endif
    return 0;
}
