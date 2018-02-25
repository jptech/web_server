#include "mime.hpp"

#include <algorithm>
#include <string>
#include <map>

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
            case MimeType::ICON:        ret = "image/x-icon"; break;
            default:
            case MimeType::TEXT:        ret = "text/plain"; break;
        }
        return ret;
    }

    MimeType get_mime_type(std::string ext)
    {
        MimeType ret;

        // transform the extension to all lower case
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        // look up table for extensions
        static std::map<std::string, MimeType> ext_table = {
            {"html", MimeType::HTML},
            {"htm", MimeType::HTML},
            {"css", MimeType::CSS},
            {"js", MimeType::JAVASCRIPT},
            {"jpg", MimeType::JPEG},
            {"jpeg", MimeType::JPEG},
            {"png", MimeType::PNG},
            {"exe", MimeType::EXEC},
            {"txt", MimeType::TEXT}
        };

        // look up the extension to see its MIME type
        std::map<std::string, MimeType>::iterator mit = ext_table.find(ext);

        if(mit == ext_table.end())
        {
            ret = mit->second;
        }
        else
        {
            ret = MimeType::UNKNOWN;
        }

        return ret;
    }
}
