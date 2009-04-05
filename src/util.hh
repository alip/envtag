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

#ifndef ENVTAG_GUARD_UTIL_HH
#define ENVTAG_GUARD_UTIL_HH 1

#include <fileref.h>

enum file_type {
    TagLib_File_MPEG,
    TagLib_File_OggVorbis,
    TagLib_File_FLAC,
    TagLib_File_MPC,
    TagLib_File_OggFlac,
    TagLib_File_WavPack,
    TagLib_File_Speex,
    TagLib_File_TrueAudio
};

enum id3v2_encoding {
  TagLib_ID3v2_Latin1,
  TagLib_ID3v2_UTF16,
  TagLib_ID3v2_UTF16BE,
  TagLib_ID3v2_UTF8
};

TagLib::FileRef *openFile(const char *path, int type, bool readprops);
void setID3v2DefaultEncoding(int encoding);
bool dumpTags(const TagLib::FileRef f, bool export_vars);
bool dumpProperties(const TagLib::FileRef f, bool export_vars);
bool setTags(TagLib::FileRef f, bool unicode);

#endif // ENVTAG_GUARD_UTIL_HH
