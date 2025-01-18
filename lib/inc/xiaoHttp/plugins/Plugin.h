/**
 * @file Plugin.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#pragma once

#include <xiaoHttp/DrObject.h>
#include <json/json.h>
#include <memory>
#include <functional>
#include <xiaoLog/Logger.h>
#include <xiaoNet/utils/NonCopyable.h>

namespace xiaoHttp
{
    enum class PluginStatus
    {
        None,
        Initializing,
        Initialized
    };

    class XIAOHTTP_EXPORT PluginBase : public virtual DrObjectBase,
                                       public xiaoNet::NonCopyable
    {
    public:
        void initialize()
        {
            if (status_ == PluginStatus::None)
            {
                status_ = PluginStatus::Initializing;
                for (auto dependency : dependencies_)
                {
                    dependency->initialize();
                }
                initAndStart(config_);
                status_ = PluginStatus::Initialized;
                if (initializedCallback_)
                    initializedCallback_(this);
            }
            else if (status_ == PluginStatus::Initialized)
            {
            }
            else
            {
                LOG_FATAL << "There are a circular dependency within plugins.";
                abort();
            }
        }

        /**
         * @brief This method must be called by xiaoHttp to initialize and start the plugin.
         * It must be implemented by the user.
         * @param config
         */
        virtual void initAndStart(const Json::Value &config) = 0;

        /**
         * @brief This method must be called by xiaoHttp to shutdown the plugin.
         *
         */
        virtual void shutdown() = 0;

        virtual ~PluginBase()
        {
        }

    protected:
        PluginBase()
        {
        }

    private:
        PluginStatus status_{PluginStatus::None};
        friend class PluginsManager;

        void setConfig(const Json::Value &config)
        {
            config_ = config;
        }

        void addDependency(PluginBase *dp)
        {
            dependencies_.push_back(dp);
        }

        void setInitializedCallback(const std::function<void(PluginBase *)> &cb)
        {
            initializedCallback_ = cb;
        }

        Json::Value config_;
        std::vector<PluginBase *> dependencies_;
        std::function<void(PluginBase *)> initializedCallback_;
    };

    template <typename T>
    struct IsPlugin
    {
        using TYPE = std::remove_cv_t<typename std::remove_reference_t<T>>;

        static int test(void *)
        {
            return 0;
        }

        static char test(PluginBase *)
        {
            return 0;
        }

        static constexpr bool value =
            (sizeof(test((TYPE *)nullptr)) == sizeof(char));
    };

    /**
     * @brief The reflection base class for plugins.
     *
     * @tparam T The type of the implementation plugin classes.
     */
    template <typename T>
    class Plugin : public PluginBase, public DrObject<T>
    {
    public:
        virtual ~Plugin()
        {
        }

    protected:
        Plugin()
        {
        }
    };
}