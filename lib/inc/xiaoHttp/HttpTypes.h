/**
 * @file HttpTypes.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-16
 *
 *
 */

namespace xiaoHttp
{
    enum HttpStatusCode
    {
        kUnKnown = 0,
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
    };
}
