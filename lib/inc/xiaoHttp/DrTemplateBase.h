/**
 * @file DrTemplateBase.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-21
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>
#include <xiaoHttp/DrObject.h>
#include <xiaoHttp/HttpViewData.h>
#include <memory>
#include <string>

namespace xiaoHttp
{
    using DrTemplateData = HttpViewData;

    /**
     * @brief This class can genertae a text string from the template file and template data.
     *
     */
    class XIAOHTTP_EXPORT DrTemplateBase : public virtual DrObjectBase
    {
    public:
        /**
         * @brief Create an object of the implementation class
         *
         * @param templateName represents the name of the template file. A template
         * file is a description file with a special format. Its extension is
         * usually .csp. The user should preprocess the template file with the
         * xiaoHttp_ctl tool to create c++ source files.
         * @return std::shared_ptr<DrTemplateBase>
         */
        static std::shared_ptr<DrTemplateBase> newTemplate(
            const std::string &templateName);

        /**
         * @brief Generate the text string
         *
         * @param data represents data rendered in the string in a format
         * according to the template file.
         * @return std::string
         */
        virtual std::string genText(
            const DrTemplateData &data = DrTemplateData()) = 0;

        virtual ~DrTemplateBase() {};
        DrTemplateBase() {};
    }
}
