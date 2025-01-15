/**
 * @file DrClassMap.cpp
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-14
 *
 *
 */

#include <xiaoHttp/DrClassMap.h>
#include <xiaoHttp/DrObject.h>
#include <xiaoLog/Logger.h>

using namespace xiaoHttp;

namespace xiaoHttp
{
    namespace internal
    {
        static std::unordered_map<std::string, std::shared_ptr<DrObjectBase>> &
        getObjsMap()
        {
            static std::unordered_map<std::string, std::shared_ptr<DrObjectBase>>
                singleInstanceMap;
            return singleInstanceMap;
        }

        static std::mutex &getMapMutex()
        {
            static std::mutex mtx;
            return mtx;
        }
    }
}

void DrClassMap::registerClass(const std::string &className,
                               const DrAllocFunc &func,
                               const DrSharedAlloFunc &sharedFunc)
{
    LOG_TRACE << "Register class:" << className;
    getMap().insert(
        std::make_pair(className, std::make_pair(func, sharedFunc)));
}

DrObjectBase *DrClassMap::newObject(const std::string &className)
{
    auto iter = getMap().find(className);
    if (iter != getMap().end())
        return iter->second.first();
    else
        return nullptr;
}

std::shared_ptr<DrObjectBase> DrClassMap::newSharedObject(
    const std::string &className)
{
    auto iter = getMap().find(className);
    if (iter != getMap().end())
    {
        if (iter->second.second)
            return iter->second.second();
        else
            return std::shared_ptr<DrObjectBase>(iter->second.first());
    }
    else
        return nullptr;
}

const std::shared_ptr<DrObjectBase> &DrClassMap::getSingleInstance(
    const std::string &className)
{
    auto &mtx = internal::getMapMutex();
    auto &singleInstanceMap = internal::getObjsMap();
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto iter = singleInstanceMap.find(className);
        if (iter != singleInstanceMap.end())
            return iter->second;
    }
    auto newObj = newSharedObject(className);
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto ret = singleInstanceMap.insert(
            std::make_pair(className, std::move(newObj)));
        return ret.first->second;
    }
}

void DrClassMap::setSingleInstance(const std::shared_ptr<DrObjectBase> &ins)
{
    auto &mtx = internal::getMapMutex();
    auto &singleInstanceMap = internal::getObjsMap();
    std::lock_guard<std::mutex> lock(mtx);
    singleInstanceMap[ins->className()] = ins;
}

std::vector<std::string> DrClassMap::getAllClassName()
{
    std::vector<std::string> ret;
    for (auto const &iter : getMap())
    {
        ret.push_back(iter.first);
    }
    return ret;
}

std::unordered_map<std::string, std::pair<DrAllocFunc, DrSharedAlloFunc>> &
DrClassMap::getMap()
{
    static std::unordered_map<std::string,
                              std::pair<DrAllocFunc, DrSharedAlloFunc>>
        map;
    return map;
}
