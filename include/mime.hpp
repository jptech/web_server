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
        EXEC
    };

    /* Get the string representation of a MIME type */
    std::string get_type_string(MimeType m);

}
