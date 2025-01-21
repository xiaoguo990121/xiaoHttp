/**
 * @file DrTemplateBase.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-21
 *
 *
 */

#include <xiaoHttp/DrClassMap.h>
#include <xiaoHttp/DrTemplateBase.h>
#include <xiaoLog/Logger.h>
#include <memory>
#include <regex>

using namespace xiaoHttp;

std::shared_ptr<DrTemplateBase> DrTemplateBase::newTemplate(
    const std::string &templateName)
{
    LOG_TRACE << "http view name=" << templateName;
    auto l = templateName.length();
    if (l >= 4 && templateName[l - 4] == '.' && templateName[l - 3] == 'c' &&
        templateName[l - 2] == 's' && templateName[l - 1] == 'p')
    {
        std::string::size_type = 0;
        std::string newName;
        newName.reserve(templateName.size());
        if (templateName[0] == '/' || templateName[0] == '\\')
        {
            pos = 1;
        }
        else if (templateName[0] == '.' &&
                 (templateName[1] == '/' || templateName[1] == '\\'))
        {
            pos = 2;
        }
        while (pos < l - 4)
        {
            if (templateName[pos] == '/' || templateName[pos] == '\\')
            {
                newName.append("::");
            }
            else
            {
                newName.append(1, templateName[pos]);
            }
            ++pos;
        }
        return std::shared_ptr<DrTemplateBase>(dynamic_cast<DrTemplateBase *>(
            xiaoHttp::DrClassMap::newObject(newName)));
    }
    else
    {
        return std::shared_ptr<DrTemplateBase>(dynamic_cast<DrTemplateBase *>(
            xiaoHttp::DrClassMap::newObject(templateName)));
    }
}