## What is envtag?

envtag is a simple audio tagger using taglib.
It offers two interfaces to edit tags:
 * Simple interface using environment variables:

    # Reading tag information
    eval $(envtag /path/to/audio/file)
    echo "Title: $TITLE"
    echo "Artist: $ARTIST"
    echo "Album: $ALBUM"
    echo "Comment: $COMMENT"
    echo "Genre: $GENRE"
    echo "Year: $YEAR"
    echo "Track: $TRACK"

    # Saving tag information
    COMMENT="Tagged by envtag" envtag -s *.mp3

    # Changing file format preserving tag information
    eval $(envtag -E /path/to/audio/file) # exports tag information
    decoder -o - /path/to/audio/file | encoder - -o /path/to/audio/file.new
    envtag -s /path/to/audio/file.new # grabs exported tag information and saves it

## Contributing

Contributions to envtag are welcome:

 * Patches can be submitted via e-mail. Use the `git format-patch` tool when
   emailing patches.

## Authors

 * Ali Polatel &lt;polatel@gmail.com&gt;
