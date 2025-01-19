/**
 * @file HttpTypes.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 *
 */

#pragma once

namespace xiaoHttp
{
    enum HttpStatusCode
    {
        kUnknown = 0,
        k100Continue = 100,
        k101SwitchingProtocols = 101,
        k102Processing = 102,
        k103EarlyHints = 103,
        k200OK = 200,
        k201Created = 201,
        k202Accepted = 202,
        k203NonAuthoritativeInformation = 203,
        k204NoContent = 204,
        k205ResetContent = 205,
        k206PartialContent = 206,
        k207MultiStatus = 207,
        k208AlreadyReported = 208,
        k226IMUsed = 226,
        k300MultipleChoices = 300,
        k301MovedPermanently = 301,
        k302Found = 302,
        k303SeeOther = 303,
        k304NotModified = 304,
        k305UseProxy = 305,
        k306Unused = 306,
        k307TemporaryRedirect = 307,
        k308PermanentRedirect = 308,
        k400BadRequest = 400,
        k401Unauthorized = 401,
        k402PaymentRequired = 402,
        k403Forbidden = 403,
        k404NotFound = 404,
        k405MethodNotAllowed = 405,
        k406NotAcceptable = 406,
        k407ProxyAuthenticationRequired = 407,
        k408RequestTimeout = 408,
        k409Conflict = 409,
        k410Gone = 410,
        k411LengthRequired = 411,
        k412PreconditionFailed = 412,
        k413RequestEntityTooLarge = 413,
        k414RequestURITooLarge = 414,
        k415UnsupportedMediaType = 415,
        k416RequestedRangeNotSatisfiable = 416,
        k417ExpectationFailed = 417,
        k418ImATeapot = 418,
        k421MisdirectedRequest = 421,
        k422UnprocessableEntity = 422,
        k423Locked = 423,
        k424FailedDependency = 424,
        k425TooEarly = 425,
        k426UpgradeRequired = 426,
        k428PreconditionRequired = 428,
        k429TooManyRequests = 429,
        k431RequestHeaderFieldsTooLarge = 431,
        k451UnavailableForLegalReasons = 451,
        k500InternalServerError = 500,
        k501NotImplemented = 501,
        k502BadGateway = 502,
        k503ServiceUnavailable = 503,
        k504GatewayTimeout = 504,
        k505HTTPVersionNotSupported = 505,
        k506VariantAlsoNegotiates = 506,
        k507InsufficientStorage = 507,
        k508LoopDetected = 508,
        k510NotExtended = 510,
        k511NetworkAuthenticationRequired = 511
    };

    enum class ReqResult
    {
        Ok = 0,
        BadResponse,
        NetworkFailure,
        BadServerAddress,
        Timeout,
        HandshakeError,
        InvalidCertificate,
        EntryptionFailure,
    };

    enum class Version
    {
        kUnKnown = 0,
        kHttp10,
        kHttp11
    };

    enum ContentType
    {
        CT_NONE = 0,
        CT_APPLICATION_JSON,
        CT_TEXT_PLAIN,
        CT_TEXT_HTML,
        CT_APPLICATION_X_FORM,
        CT_APPLICATION_X_JAVASCRIPT [[deprecated("use CT_TEXT_JAVASCRIPT")]],
        CT_TEXT_JAVASCRIPT,
        CT_TEXT_CSS,
        CT_TEXT_CSV,
        CT_TEXT_XML,        // suggests human readable xml
        CT_APPLICATION_XML, // suggest machine-to-machine xml
        CT_TEXT_XSL,
        CT_APPLICATION_WASM,
        CT_APPLICATION_OCTET_STREAM,
        CT_APPLICATION_FONT_WOFF,
        CT_APPLICATION_FONT_WOFF2,
        CT_APPLICATION_GZIP,
        CT_APPLICATION_JAVA_ARCHIVE,
        CT_APPLICATION_PDF,
        CT_APPLICATION_MSWORD,
        CT_APPLICATION_MSWORDX,
        CT_APPLICATION_VND_MS_FONTOBJ,
        CT_APPLICATION_VND_RAR,
        CT_APPLICATION_XHTML,
        CT_APPLICATION_X_7Z,
        CT_APPLICATION_X_BZIP,
        CT_APPLICATION_X_BZIP2,
        CT_APPLICATION_X_HTTPD_PHP,
        CT_APPLICATION_X_FONT_TRUETYPE,
        CT_APPLICATION_X_FONT_OPENTYPE,
        CT_APPLICATION_X_TAR,
        CT_APPLICATION_X_TGZ,
        CT_APPLICATION_X_XZ,
        CT_APPLICATION_ZIP,
        CT_AUDIO_AAC,
        CT_AUDIO_AC3,
        CT_AUDIO_AIFF,
        CT_AUDIO_FLAC,
        CT_AUDIO_MATROSKA,
        CT_AUDIO_MPEG,
        CT_AUDIO_MPEG4,
        CT_AUDIO_OGG,
        CT_AUDIO_WAVE,
        CT_AUDIO_WEBM,
        CT_AUDIO_X_APE,
        CT_AUDIO_X_MS_WMA,
        CT_AUDIO_X_TTA,
        CT_AUDIO_X_WAVPACK,
        CT_IMAGE_APNG,
        CT_IMAGE_AVIF,
        CT_IMAGE_BMP,
        CT_IMAGE_GIF,
        CT_IMAGE_ICNS,
        CT_IMAGE_JPG,
        CT_IMAGE_JP2,
        CT_IMAGE_PNG,
        CT_IMAGE_SVG_XML,
        CT_IMAGE_TIFF,
        CT_IMAGE_WEBP,
        CT_IMAGE_X_MNG,
        CT_IMAGE_X_TGA,
        CT_IMAGE_XICON,
        CT_VIDEO_APG,
        CT_VIDEO_AV1,
        CT_VIDEO_QUICKTIME,
        CT_VIDEO_MATROSKA,
        CT_VIDEO_MP4,
        CT_VIDEO_MPEG,
        CT_VIDEO_MPEG2TS,
        CT_VIDEO_OGG,
        CT_VIDEO_WEBM,
        CT_VIDEO_X_M4V,
        CT_VIDEO_X_MSVIDEO,
        CT_MULTIPART_FORM_DATA,
        CT_CUSTOM
    };

    enum FileType
    {
        FT_UNKNOWN = 0,
        FT_CUSTOM,
        FT_DOCUMENT,
        FT_ARCHIVE,
        FT_AUDIO,
        FT_MEDIA,
        FT_IMAGE
    };

    enum HttpMethod
    {
        Get = 0,
        Post,
        Head,
        Put,
        Delete,
        Options,
        Patch,
        Invalid
    };

    enum class ReqReult
    {
        Ok = 0,
        BadResponse,
        NetworkFailure,
        BadServerAddress,
        Timeout,
        HandshakeError,
        InvalidCertificate,
        EncryptionFailure,
    };

    enum class WebSocketMessageType
    {
        Text = 0,
        Binary,
        Ping,
        Pong,
        Close,
        Unknown
    };
}
