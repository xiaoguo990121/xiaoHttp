/**
 * @file DrClassMap.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>
#include <xiaoLog/Logger.h>
#include <functional>
#include <vector>
#include <unordered_map>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif

namespace xiaoHttp
{
    class DrObjectBase;
    using DrAllocFunc = std::function<DrObjectBase *()>;
    using DrSharedAlloFunc = std::function<std::shared_ptr<DrObjectBase>()>;

    /**
     * @brief A map class which can create DrObjects from names.
     *
     */
    class XIAOHTTP_EXPORT DrClassMap
    {
    public:
        /**
         * @brief Register a class into the map
         *
         * @param className The name of the class
         * @param func The function which can create a new instance of the class.
         * @param sharedFunc
         */
        static void registerClass(const std::string &className,
                                  const DrAllocFunc &func,
                                  const DrSharedAlloFunc &sharedFunc = nullptr);

        /**
         * @brief Create a new instance of the class named by className
         *
         * @param className The name of the class
         * @return DrObjectBase* The pointer to the newly created instance.
         */
        static DrObjectBase *newObject(const std::string &className);

        /**
         * @brief Get the shared_ptr instance of the class named by className
         *
         * @param className
         * @return std::shared_ptr<DrObjectBase>
         */
        static std::shared_ptr<DrObjectBase> newSharedObject(
            const std::string &className);

        /**
         * @brief Get the Single Instance object
         *
         * @param className
         * @return const std::shared_ptr<DrObjectBase>&
         */
        static const std::shared_ptr<DrObjectBase> &getSingleInstance(
            const std::string &className);

        /**
         * @brief Get the Single Instance object
         *
         * @tparam T The type of the class
         * @return std::shared_ptr<T>
         * @note The T must be a subclass of the DrObjectBase class.
         */
        template <typename T>
        static std::shared_ptr<T> getSingleInstance()
        {
            static_assert(std::is_base_of<DrObjectBase, T>::value,
                          "T must be a sub-class of DrObjectBase");
            static auto const singleton =
                std::dynamic_pointer_cast<T>(getSingleInstance(T::classTypeName()));
            assert(singleton);
            return singleton;
        }

        /**
         * @brief Set the Single Instance object into the map.
         *
         * @param ins
         */
        static void setSingleInstance(const std::shared_ptr<DrObjectBase> &ins);

        /**
         * @brief Get the All Class Name object
         *
         * @return std::vector<std::string>
         */
        static std::vector<std::string> getAllClassName();

        static std::string demangle(const char *mangled_name)
        {
#ifndef _MSC_VER
            std::size_t len = 0;
            int status = 0;
            std::unique_ptr<char, decltype(&std::free)> ptr(
                __cxxabiv1::__cxa_demangle(mangled_name, nullptr, &len, &status),
                &std::free);
            if (status == 0)
            {
                return std::string(ptr.get());
            }
            LOG_ERROR << "Demangle error!";
            return "";
#else
#endif
        }

    protected:
        static std::unordered_map<std::string,
                                  std::pair<DrAllocFunc, DrSharedAlloFunc>> &
        getMap();
    };
}