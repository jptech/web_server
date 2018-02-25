#pragma once

#include <string>

namespace wwwserver{

    /* Supported MIME Types */
    enum class MimeType
    {
        TEXT,
        HTML,
        CSS,
        JAVASCRIPT,
        JPEG,
        PNG,
        MPEG,
        EXEC,
        ICON,
        UNKNOWN
    };

    /* Get the string representation of a MIME type */
    std::string get_type_string(MimeType m);

    /* Get the Mime Type for the file extension */
    MimeType get_mime_type(std::string ext);

}
