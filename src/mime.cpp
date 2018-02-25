#include "mime.hpp"

namespace wwwserver
{
    std::string get_type_string(MimeType m)
    {
        std::string ret;
        switch(m)
        {
            case MimeType::HTML:        ret = "text/html"; break;
            case MimeType::CSS:         ret = "text/css"; break;
            case MimeType::JAVASCRIPT:  ret = "text/javascript"; break;
            case MimeType::JPEG:        ret = "image/jpeg"; break;
            case MimeType::PNG:         ret = "image/png"; break;
            case MimeType::MPEG:        ret = "audio/mpeg"; break;
            case MimeType::EXEC:        ret = "application/octet-stream"; break;
            default:
            case MimeType::TEXT:        ret = "text/plain"; break;
        }
        return ret;
    }
}
