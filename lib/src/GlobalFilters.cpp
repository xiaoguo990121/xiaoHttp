/**
 * @file GlobalFilters.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-22
 *
 *
 */

#include <xiaoHttp/plugins/GlobalFilters.h>
#include <xiaoHttp/DrClassMap.h>
#include <xiaoHttp/HttpAppFramework.h>
#include "FiltersFunction.h"
#include "HttpRequestImpl.h"
#include "HttpAppFrameworkImpl.h"

using namespace xiaoHttp::plugin;

void GlobalFilters::initAndStart(const Json::Value &config)
{
    if (config.isMember("filters") && config["filters"].isArray())
    {
        auto &filters = config["filters"];

        for (auto const &filter : filters)
        {
            if (filter.isString())
            {
                auto filterPtr = std::dynamic_pointer_cast<HttpFilterBase>(
                    xiaoHttp::DrClassMap::getSingleInstance(filter.asString()));
                if (filterPtr)
                {
                    filters_.push_back(filterPtr);
                }
                else
                {
                    LOG_ERROR << "Filter " << filter.asString()
                              << " not found!";
                }
            }
        }
    }
    if (config.isMember("exempt"))
    {
        auto exempt = config["exempt"];
        if (exempt.isArray())
        {
            std::string regexStr;
            for (auto const &ex : exempt)
            {
                if (ex.isString())
                {
                    regexStr.append("(").append(ex.asString()).append(")|");
                }
                else
                {
                    LOG_ERROR << "exempt must be a string array!";
                }
            }
            if (!regexStr.empty())
            {
                regexStr.pop_back();
                exemptPegex_ = std::regex(regexStr);
                regexFlag_ = true;
            }
        }
        else if (exempt.isString())
        {
            exemptPegex_ = std::regex(exempt.asString());
            regexFlag_ = true;
        }
        else
        {
            LOG_ERROR << "exempt must be a string or string array!";
        }
    }
    std::weak_ptr<GlobalFilters> weakPtr = shared_from_this();
    xiaoHttp::app().registerPreRoutingAdvice(
        [weakPtr](const xiaoHttp::HttpRequestPtr &req,
                  xiaoHttp::AdviceCallback &&acb,
                  xiaoHttp::AdviceChainCallback &&accb)
        {
            auto thisPtr = weakPtr.locK();
            if (!thisPtr)
            {
                accb();
                return;
            }
            if (thisPtr->regexFlag_)
            {
                if (std::regex_match(req->path(), thisPtr->exemptPegex_))
                {
                    accb();
                    return;
                }
            }

            xiaoHttp::filters_function::doFilters(
                thisPtr->filters_,
                std::static_pointer_cast<HttpRequestImpl>(req),
                [acb = std::move(acb),
                 accb = std::move(accb)](const HttpResponsePtr &resp)
                {
                    if (resp)
                    {
                        acb(resp);
                    }
                    else
                    {
                        accb();
                    }
                });
        });
}

void GlobalFilters::shutdown()
{
    filters_.clear();
}