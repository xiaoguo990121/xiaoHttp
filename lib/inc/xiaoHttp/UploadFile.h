/**
 * @file UploadFile.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-14
 *
 *
 */

#pragma once
#include <string>
#include <xiaoHttp/HttpTypes.h>

namespace xiaoHttp
{
    /**
     * @brief This class represents an upload file which will be transfered to the server
     * via the multipart/form-data format
     *
     */
    class UploadFile
    {
    public:
        explicit UploadFile(const std::string &filePath,
                            const std::string &fileName = "",
                            const std::string &itemName = "file",
                            ContentType contentType = CT_NONE)
            : path_(filePath), itemName_(itemName), contentType_(contentType)
        {
            if (!fileName.empty())
            {
                fileName_ = fileName;
            }
            else
            {
                auto pos = filePath.rfind('/');
                if (pos != std::string::npos)
                {
                    fileName_ = filePath.substr(pos + 1);
                }
                else
                {
                    fileName_ = filePath;
                }
            }
        }

        const std::string &path() const
        {
            return path_;
        }

        const std::string &fileName() const
        {
            return fileName_;
        }

        const std::string &itemName() const
        {
            return itemName_;
        }

        ContentType contentType() const
        {
            return contentType_;
        }

    private:
        std::string path_;
        std::string fileName_;
        std::string itemName_;
        ContentType contentType_;
    };

}