/**
 * @file DrObject.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-14
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>
#include <xiaoHttp/DrClassMap.h>

#include <string>
#include <type_traits>

#ifdef _MSC_VER
#pragma waring(disable : 4250)
#endif

namespace xiaoHttp
{
    /**
     * @brief The base class for all xiaoHttp reflection classes.
     *
     */
    class XIAOHTTP_EXPORT DrObjectBase
    {
    public:
        /**
         * @brief Get the class name
         *
         * @return const std::string&
         */
        virtual const std::string &className() const
        {
            static const std::string name{"DrObjectBase"};
            return name;
        }

        /**
         * @brief Return true if the class name is 'class_name'
         *
         * @param class_name
         * @return true
         * @return false
         */
        virtual bool isClass(const std::string &class_name) const
        {
            return (className() == class_name);
        }

        virtual ~DrObjectBase()
        {
        }
    };

    // 用于判断一个类是否支持自动创建功能
    template <typename T>
    struct isAutoCreationClass
    {
        // 如果类有静态成员isAutoCreation且类型为bool，则返回其值
        template <class C>
        static constexpr auto check(C *)
            -> std::enable_if_t<std::is_same_v<decltype(C::isAutoCreation), bool>, bool>
        {
            return C::isAutoCreation;
        }

        // 如果没有则返回false
        template <typename>
        static constexpr bool check(...)
        {
            return false;
        }

        static constexpr bool value = check<T>(nullptr);
    };

    template <typename T>
    class DrObject : public virtual DrObjectBase
    {
    public:
        const std::string &className() const override
        {
            return alloc_.className();
        }

        static const std::string &classTypeName()
        {
            return alloc_.className();
        }

        bool isClass(const std::string &class_name) const override
        {
            return (className() == class_name);
        }

    protected:
        DrObject() = default;
        ~DrObject() override = default;

    private:
        class DrAllocator
        {
        public:
            DrAllocator()
            {
                registerClass<T>();
            }

            const std::string &className() const
            {
                static std::string className =
                    DrClassMap::demangle(typeid(T).name());
                return className;
            }

            template <typeName D>
            void registerClass()
            {
                // 判断D是否具有默认构造函数，如果D可以通过无参构造函数创建的类型即D()
                // 是合法的，则条件为true
                if constexpr (std::is_default_constructible<D>::value)
                {
                    DrClassMap::registerClass(
                        className(),
                        []() -> DrObjectBase *
                        { return new T; },
                        []() -> std::shared_ptr<DrObjectBase>
                        {
                            return std::make_shared<T>();
                        });
                }
                else if constexpr (isAutoCreationClass<D>::value)
                {
                    static_assert(std::is_default_constructible<D>::value,
                                  "Class is not default constructable!");
                }
            }
        };

        static DrAllocator alloc_;
    };

    template <typename T>
    typename DrObject<T>::DrAllocator DrObject<T>::alloc_;
}