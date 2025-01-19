/**
 * @file IOThreadStorage.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-15
 *
 *
 */

#pragma once

#include <xiaoHttp/HttpAppFramework.h>
#include <xiaoNet/utils/NonCopyable.h>
#include <vector>

namespace xiaoHttp
{
    /**
     * @brief Utility class for thread storage handling
     *
     * Thread storage allows the efficient handling of reusable data without thread
     * synchroisation. For instace, such a thread storage would be useful to store
     * database connections.
     *
     * @tparam C
     */
    template <typename C>
    class IOThreadStorage : public xiaoNet::NonCopyable
    {
    public:
        using ValueType = C;
        using InitCallback = std::function<void(ValueType &, size_t)>;

        template <typename... Args>
        IOThreadStorage(Args &&...args)
        {
            static_assert(std::is_constructible<C, Args &&...>::value,
                          "Unable to construct storage with given signature");
            size_t numThreads = app().getThreadNum();
            assert(numThreads > 0 &&
                   numThreads != (std::numeric_limits<size_t>::max)());
            // set the size to numThreads+1 to enable access to this in the main thread.
            storage_.reserve(numThreads + 1);

            for (size_t i = 0; i <= numThreads; ++i)
            {
                storage_.emplace_back(std::forward<Args>(args)...);
            }
        }

        void init(const InitCallback &initCB)
        {
            for (size_t i = 0; i < storage_.size(); ++i)
            {
                initCB(storage_[i], i);
            }
        }

        /**
         * @brief Get the Thread Data object
         *
         * @return ValueType&
         */
        inline ValueType &getThreadData()
        {
            size_t idx = app().getCurrentThreadIndex();
            assert(idx < storage_.size());
            return storage_[idx];
        }

        inline const ValueType &getThreadData() const
        {
            size_t idx = app().getCurrentThreadIndex();
            assert(idx < storage_.size());
            return storage_[idx];
        }

        inline void setThreadData(const ValueType &newData)
        {
            size_t idx = app().getCurrentThreadIndex();
            assert(idx < storage_.size());
            storage_[idx] = newData;
        }

        inline void setThreadData(ValueType &&newData)
        {
            size_t idx = app().getCurrentThreadIndex();
            assert(idx < storage_.size());
            storage_[idx] = std::move(newData);
        }

        inline ValueType *operator->()
        {
            size_t idx = app().getCurrentThreadIndex();
            assert(idx < storage_.size());
            return &storage_[idx];
        }

        inline ValueType &operator*()
        {
            return getThreadData();
        }

        inline const ValueType *operator->() const
        {
            size_t idx = app().getCurrentThreadIndex();
            assert(idx < storage_.size());
            return &storage_[idx];
        }

        inline const ValueType &operator*() const
        {
            return getThreadData();
        }

    private:
        std::vector<ValueType> storage_;
    };

    inline xiaoNet::EventLoop *getIOThreadStorageLoop(size_t index) noexcept(false)
    {
        if (index > xiaoHttp::app().getThreadNum())
        {
            throw std::out_of_range("Event loop index is out of range");
        }
        if (index == xiaoHttp::app().getThreadNum())
        {
            return xiaoHttp::app().getLoop();
        }
        return xiaoHttp::app().getIOLoop(index);
    }
}