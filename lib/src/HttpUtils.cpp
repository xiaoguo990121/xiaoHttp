/**
 * @file HttpUtils.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#include "HttpUtils.h"
#include <mutex>

namespace xiaoHttp
{
    static const std::unordered_map<
        ContentType,
        std::pair<std::vector<std::string_view>, std::string_view>>
        mimeTypeDatabase_{
            {CT_NONE, {{""}, ""}},
            {CT_APPLICATION_OCTET_STREAM, {{"application/octet-stream"}, ""}},
            {CT_APPLICATION_X_FORM, {{"application/x-www-form-urlencoded"}, ""}},
            {CT_MULTIPART_FORM_DATA, {{"multipart/form-data"}, ""}},
            {CT_APPLICATION_GZIP, {{"application/gzip"}, ""}},
            {CT_APPLICATION_JSON,
             {{"application/json"}, "application/json; charset=utf-8"}},
            {CT_APPLICATION_FONT_WOFF, {{"application/font-woff"}, ""}},
            {CT_APPLICATION_FONT_WOFF2, {{"application/font-woff2"}, ""}},
            {CT_APPLICATION_JAVA_ARCHIVE,
             {{"application/java-archive", "application/x-java-archive"}, ""}},
            {CT_APPLICATION_MSWORD, {{"application/msword"}, ""}},
            {CT_APPLICATION_MSWORDX,
             {{"application/"
               "vnd.openxmlformats-officedocument.wordprocessingml.document"},
              ""}},
            {CT_APPLICATION_PDF, {{"application/pdf"}, ""}},
            {CT_APPLICATION_VND_MS_FONTOBJ,
             {{"application/vnd.ms-fontobject"}, ""}},
            {CT_APPLICATION_VND_RAR, {{"application/vnd.rar"}, ""}},
            {CT_APPLICATION_WASM, {{"application/wasm"}, ""}},
            {CT_APPLICATION_X_BZIP, {{"application/x-bzip"}, ""}},
            {CT_APPLICATION_X_BZIP2, {{"application/x-bzip2"}, ""}},
            {CT_APPLICATION_X_7Z, {{"application/x-7z-compressed"}, ""}},
            {CT_APPLICATION_X_HTTPD_PHP, {{"application/x-httpd-php"}, ""}},
            {CT_APPLICATION_X_JAVASCRIPT,
             {{"application/x-javascript"},
              "application/x-javascript; charset=utf-8"}},
            {CT_APPLICATION_X_FONT_OPENTYPE,
             {{"application/x-font-opentype", "font/otf"}, ""}},
            {CT_APPLICATION_X_FONT_TRUETYPE,
             {{"application/x-font-truetype", "font/ttf"}, ""}},
            {CT_APPLICATION_X_TAR, {{"application/x-tar"}, ""}},
            {CT_APPLICATION_X_TGZ, {{"application/x-tgz"}, ""}},
            {CT_APPLICATION_X_XZ, {{"application/x-xz", "application/x-lzma"}, ""}},
            {CT_APPLICATION_XHTML,
             {{"application/xhtml+xml", "application/xhtml"},
              "application/xhtml+xml; charset=utf-8"}},
            {CT_APPLICATION_XML,
             {{"application/xml"}, "application/xml; charset=utf-8"}},
            {CT_APPLICATION_ZIP, {{"application/zip"}, ""}},
            {CT_AUDIO_AAC, {{"audio/aac", "audio/aacp"}, ""}},
            {CT_AUDIO_AC3, {{"audio/ac3"}, ""}},
            {CT_AUDIO_AIFF, {{"audio/aiff", "audio/x-aiff"}, ""}},
            {CT_AUDIO_FLAC, {{"audio/flac"}, ""}},
            {CT_AUDIO_MATROSKA, {{"audio/matroska", "audio/x-matroska"}, ""}},
            {CT_AUDIO_MPEG, {{"audio/mpeg"}, ""}},
            {CT_AUDIO_MPEG4, {{"audio/mp4", "audio/x-m4a"}, ""}},
            {CT_AUDIO_OGG, {{"audio/ogg"}, ""}},
            {CT_AUDIO_WAVE, {{"audio/wav", "audio/x-wav"}, ""}},
            {CT_AUDIO_X_APE, {{"audio/x-ape"}, ""}},
            {CT_AUDIO_X_MS_WMA, {{"audio/x-ms-wma"}, ""}},
            {CT_AUDIO_X_TTA, {{"audio/x-tta"}, ""}},
            {CT_AUDIO_X_WAVPACK, {{"audio/x-wavpack"}, ""}},
            {CT_AUDIO_WEBM, {{"audio/webm"}, ""}},
            {CT_IMAGE_APNG, {{"image/apng"}, ""}},
            {CT_IMAGE_AVIF, {{"image/avif"}, ""}},
            {CT_IMAGE_BMP, {{"image/bmp"}, ""}},
            {CT_IMAGE_GIF, {{"image/gif"}, ""}},
            {CT_IMAGE_ICNS, {{"image/icns"}, ""}},
            {CT_IMAGE_JP2, {{"image/jp2", "image/jpx", "image/jpm"}, ""}},
            {CT_IMAGE_JPG, {{"image/jpeg"}, ""}},
            {CT_IMAGE_PNG, {{"image/png"}, ""}},
            {CT_IMAGE_SVG_XML, {{"image/svg+xml"}, ""}},
            {CT_IMAGE_TIFF, {{"image/tiff"}, ""}},
            {CT_IMAGE_WEBP, {{"image/webp"}, ""}},
            {CT_IMAGE_X_MNG, {{"image/x-mng"}, ""}},
            {CT_IMAGE_X_TGA, {{"image/x-tga", "image/x-targa"}, ""}},
            {CT_IMAGE_XICON, {{"image/vnd.microsoft.icon", "image/x-icon"}, ""}},
            {CT_TEXT_CSS, {{"text/css"}, "text/css; charset=utf-8"}},
            {CT_TEXT_CSV, {{"text/csv"}, "text/csv; charset=utf-8"}},
            {CT_TEXT_HTML, {{"text/html"}, "text/html; charset=utf-8"}},
            {CT_TEXT_JAVASCRIPT,
             {{"text/javascript"}, "text/javascript; charset=utf-8"}},
            {CT_TEXT_PLAIN, {{"text/plain"}, "text/plain; charset=utf-8"}},
            {CT_TEXT_XML, {{"text/xml"}, "text/xml; charset=utf-8"}},
            {CT_TEXT_XSL, {{"text/xsl"}, "text/xsl; charset=utf-8"}},
            {CT_VIDEO_APG, {{"video/apg"}, ""}},
            {CT_VIDEO_AV1, {{"video/av01", "video/av1"}, ""}},
            {CT_VIDEO_QUICKTIME, {{"video/quicktime"}, ""}},
            {CT_VIDEO_MPEG, {{"video/mpeg"}, ""}},
            {CT_VIDEO_MPEG2TS, {{"video/mp2t"}, ""}},
            {CT_VIDEO_MP4, {{"video/mp4"}, ""}},
            {CT_VIDEO_OGG, {{"video/ogg"}, ""}},
            {CT_VIDEO_WEBM, {{"video/webm"}, ""}},
            {CT_VIDEO_X_M4V, {{"video/x-m4v"}, ""}},
            {CT_VIDEO_MATROSKA, {{"video/matroska", "video/x-matroska"}, ""}},
            {CT_VIDEO_X_MSVIDEO, {{"video/x-msvideo"}, ""}},
        };

    const std::string_view &statusCodeToString(int code)
    {
        switch (code)
        {
        case 100:
        {
            static std::string_view sv = "Continue";
            return sv;
        }
        case 101:
        {
            static std::string_view sv = "Switching Protocols";
            return sv;
        }
        case 102:
        {
            static std::string_view sv = "Processing";
            return sv;
        }
        case 103:
        {
            static std::string_view sv = "Early Hints";
            return sv;
        }
        case 200:
        {
            static std::string_view sv = "OK";
            return sv;
        }
        case 201:
        {
            static std::string_view sv = "Created";
            return sv;
        }
        case 202:
        {
            static std::string_view sv = "Accepted";
            return sv;
        }
        case 203:
        {
            static std::string_view sv = "Non-Authoritative Information";
            return sv;
        }
        case 204:
        {
            static std::string_view sv = "No Content";
            return sv;
        }
        case 205:
        {
            static std::string_view sv = "Reset Content";
            return sv;
        }
        case 206:
        {
            static std::string_view sv = "Partial Content";
            return sv;
        }
        case 207:
        {
            static std::string_view sv = "Multi-Status";
            return sv;
        }
        case 208:
        {
            static std::string_view sv = "Already Reported";
            return sv;
        }
        case 226:
        {
            static std::string_view sv = "IM Used";
            return sv;
        }
        case 300:
        {
            static std::string_view sv = "Multiple Choices";
            return sv;
        }
        case 301:
        {
            static std::string_view sv = "Moved Permanently";
            return sv;
        }
        case 302:
        {
            static std::string_view sv = "Found";
            return sv;
        }
        case 303:
        {
            static std::string_view sv = "See Other";
            return sv;
        }
        case 304:
        {
            static std::string_view sv = "Not Modified";
            return sv;
        }
        case 305:
        {
            static std::string_view sv = "Use Proxy";
            return sv;
        }
        case 306:
        {
            static std::string_view sv = "(Unused)";
            return sv;
        }
        case 307:
        {
            static std::string_view sv = "Temporary Redirect";
            return sv;
        }
        case 308:
        {
            static std::string_view sv = "Permanent Redirect";
            return sv;
        }
        case 400:
        {
            static std::string_view sv = "Bad Request";
            return sv;
        }
        case 401:
        {
            static std::string_view sv = "Unauthorized";
            return sv;
        }
        case 402:
        {
            static std::string_view sv = "Payment Required";
            return sv;
        }
        case 403:
        {
            static std::string_view sv = "Forbidden";
            return sv;
        }
        case 404:
        {
            static std::string_view sv = "Not Found";
            return sv;
        }
        case 405:
        {
            static std::string_view sv = "Method Not Allowed";
            return sv;
        }
        case 406:
        {
            static std::string_view sv = "Not Acceptable";
            return sv;
        }
        case 407:
        {
            static std::string_view sv = "Proxy Authentication Required";
            return sv;
        }
        case 408:
        {
            static std::string_view sv = "Request Time-out";
            return sv;
        }
        case 409:
        {
            static std::string_view sv = "Conflict";
            return sv;
        }
        case 410:
        {
            static std::string_view sv = "Gone";
            return sv;
        }
        case 411:
        {
            static std::string_view sv = "Length Required";
            return sv;
        }
        case 412:
        {
            static std::string_view sv = "Precondition Failed";
            return sv;
        }
        case 413:
        {
            static std::string_view sv = "Request Entity Too Large";
            return sv;
        }
        case 414:
        {
            static std::string_view sv = "Request-URI Too Large";
            return sv;
        }
        case 415:
        {
            static std::string_view sv = "Unsupported Media Type";
            return sv;
        }
        case 416:
        {
            static std::string_view sv = "Requested Range Not Satisfiable";
            return sv;
        }
        case 417:
        {
            static std::string_view sv = "Expectation Failed";
            return sv;
        }
        case 418:
        {
            static std::string_view sv = "I'm a Teapot";
            return sv;
        }
        case 421:
        {
            static std::string_view sv = "Misdirected Request";
            return sv;
        }
        case 422:
        {
            static std::string_view sv = "Unprocessable Entity";
            return sv;
        }
        case 423:
        {
            static std::string_view sv = "Locked";
            return sv;
        }
        case 424:
        {
            static std::string_view sv = "Failed Dependency";
            return sv;
        }
        case 425:
        {
            static std::string_view sv = "Too Early";
            return sv;
        }
        case 426:
        {
            static std::string_view sv = "Upgrade Required";
            return sv;
        }
        case 428:
        {
            static std::string_view sv = "Precondition Required";
            return sv;
        }
        case 429:
        {
            static std::string_view sv = "Too Many Requests";
            return sv;
        }
        case 431:
        {
            static std::string_view sv = "Request Header Fields Too Large";
            return sv;
        }
        case 451:
        {
            static std::string_view sv = "Unavailable For Legal Reasons";
            return sv;
        }
        case 500:
        {
            static std::string_view sv = "Internal Server Error";
            return sv;
        }
        case 501:
        {
            static std::string_view sv = "Not Implemented";
            return sv;
        }
        case 502:
        {
            static std::string_view sv = "Bad Gateway";
            return sv;
        }
        case 503:
        {
            static std::string_view sv = "Service Unavailable";
            return sv;
        }
        case 504:
        {
            static std::string_view sv = "Gateway Time-out";
            return sv;
        }
        case 505:
        {
            static std::string_view sv = "HTTP Version Not Supported";
            return sv;
        }
        case 506:
        {
            static std::string_view sv = "Variant Also Negotiates";
            return sv;
        }
        case 507:
        {
            static std::string_view sv = "Insufficient Storage";
            return sv;
        }
        case 508:
        {
            static std::string_view sv = "Loop Detected";
            return sv;
        }
        case 510:
        {
            static std::string_view sv = "Not Extended";
            return sv;
        }
        case 511:
        {
            static std::string_view sv = "Network Authentication Required";
            return sv;
        }
        default:
            if (code >= 100 && code < 200)
            {
                static std::string_view sv = "Informational";
                return sv;
            }
            else if (code >= 200 && code < 300)
            {
                static std::string_view sv = "Successful";
                return sv;
            }
            else if (code >= 300 && code < 400)
            {
                static std::string_view sv = "Redirection";
                return sv;
            }
            else if (code >= 400 && code < 500)
            {
                static std::string_view sv = "Bad Request";
                return sv;
            }
            else if (code >= 500 && code < 600)
            {
                static std::string_view sv = "Server Error";
                return sv;
            }
            else
            {
                static std::string_view sv = "Undefined Error";
                return sv;
            }
        }
    }

    const std::string_view &contentTypeToMime(ContentType contentType)
    {
        auto it = mimeTypeDatabase_.find(contentType);
        return (it == mimeTypeDatabase_.end())
                   ? mimeTypeDatabase_.at(CT_APPLICATION_OCTET_STREAM).first.front()
                   : (it->second.second.empty() ? it->second.first.front()
                                                : it->second.second);
    }

    ContentType parseContentType(const std::string_view &contentType)
    {
        // Generate map from database for faster query
        static std::unordered_map<std::string_view, ContentType> contentTypeMap_;
        // Thread safe initialization
        static std::once_flag flag;
        std::call_once(flag, []()
                       {
        for (const auto &e : mimeTypeDatabase_)
        {
            for (const auto &type : e.second.first)
                contentTypeMap_[type] = e.first;
        } });
        auto ext = contentType.find(';');
        if (ext != std::string_view::npos)
            return parseContentType(contentType.substr(0, ext));
        if (contentType == "application/x-www-form-urlencoded")
            return CT_APPLICATION_X_FORM;
        if (contentType == "multipart/form-data")
            return CT_MULTIPART_FORM_DATA;
        auto it = contentTypeMap_.find(contentType);
        return (it == contentTypeMap_.end()) ? CT_CUSTOM : it->second;
    }
}