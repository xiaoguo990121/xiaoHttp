/**
 * @file Exception.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 *
 */

#pragma once

#include <xiaoHttp/exports.h>
#include <functional>
#include <stdexcept>
#include <string>

namespace xiaoHttp
{
    namespace orm
    {

        class XIAOHTTP_EXPORT XiaoHttpDbException
        {
        public:
            virtual ~XiaoHttpDbException() noexcept
            {
            }

            /// Return std::exception base-class object
            /**
             * Use this to get at the exception's what() function, or to downcast to a
             * more specific type using dynamic_cast.
             *
             * Casting directly from DrogonDbException to a specific exception type is
             * not likely to work since DrogonDbException is not (and could not safely
             * be) derived from std::exception.
             *
             * For example, to test dynamically whether an exception is an SqlError:
             *
             * @code
             * try
             * {
             *   // ...
             * }
             * catch (const drogon::orm::DrogonDbException &e)
             * {
             *   std::cerr << e.base().what() << std::endl;
             *   const drogon::orm::SqlError *s=dynamic_cast<const
             * drogon::orm::SqlError*>(&e.base());
             *   if (s) std::cerr << "Query was: " << s->query() << std::endl;
             * }
             * @endcode
             */
            virtual const std::exception &base() const noexcept
            {
                static std::exception except;
                return except;
            }
        };

        /// Run-time Failure encountered by xiaoHttp orm lib, similar to
        /// std::runtime_error
        class Failure : public XiaoHttpDbException, public std::runtime_error
        {
            const std::exception &base() const noexcept override
            {
                return *this;
            }

        public:
            XIAOHTTP_EXPORT explicit Failure(const std::string &);
        };

        /// Exception class for lost or failed backend connection.
        /**
         * @warning When this happens on Unix-like systems, you may also get a SIGPIPE
         * signal.  That signal aborts the program by default, so if you wish to be able
         * to continue after a connection breaks, be sure to disarm this signal.
         *
         * If you're working on a Unix-like system, see the manual page for
         * @c signal (2) on how to deal with SIGPIPE.  The easiest way to make this
         * signal harmless is to make your program ignore it:
         *
         * @code
         * #include <signal.h>
         *
         * int main()
         * {
         *   signal(SIGPIPE, SIG_IGN);
         *   // ...
         * @endcode
         */
        class BrokenConnection : public Failure
        {
        public:
            XIAOHTTP_EXPORT BrokenConnection();
            XIAOHTTP_EXPORT explicit BrokenConnection(const std::string &);
        };

        /// Exception class for failed queries.
        /** Carries, in addition to a regular error message, a copy of the failed query
         * and (if available) the SQLSTATE value accompanying the error.
         */
        class SqlError : public Failure
        {
            const std::string query_;

            const std::string sqlState_;

            const int errcode_{0};

            const int extendedErrcode_{0};

        public:
            XIAOHTTP_EXPORT explicit SqlError(const std::string &msg = "",
                                              const std::string &Q = "",
                                              const char sqlstate[] = nullptr);
            XIAOHTTP_EXPORT explicit SqlError(const std::string &msg,
                                              const std::string &Q,
                                              const int errcode,
                                              const int extendedErrcode);
            XIAOHTTP_EXPORT virtual ~SqlError() noexcept;

            XIAOHTTP_EXPORT const std::string &query() const noexcept;
            XIAOHTTP_EXPORT const std::string &sqlState() const noexcept;
            XIAOHTTP_EXPORT int errcode() const noexcept;
            XIAOHTTP_EXPORT int extendedErrcode() const noexcept;
        };

        class InDoubtError : public Failure
        {
        public:
            XIAOHTTP_EXPORT explicit InDoubtError(const std::string &);
        };

        /// The backend saw itself forced to roll back the ongoing transaction.
        class TransactionRollback : public Failure
        {
        public:
            XIAOHTTP_EXPORT explicit TransactionRollback(const std::string &);
        };

        /// Transaction failed to serialize.  Please retry it.
        /** Can only happen at transaction isolation levels REPEATABLE READ and
         * SERIALIZABLE.
         *
         * The current transaction cannot be committed without violating the guarantees
         * made by its isolation level.  This is the effect of a conflict with another
         * ongoing transaction.  The transaction may still succeed if you try to
         * perform it again.
         */
        class SerializationFailure : public TransactionRollback
        {
        public:
            XIAOHTTP_EXPORT explicit SerializationFailure(const std::string &);
        };

        /// We can't tell whether our last statement succeeded.
        class StatementCompletionUnknown : public TransactionRollback
        {
        public:
            XIAOHTTP_EXPORT explicit StatementCompletionUnknown(const std::string &);
        }

        /// The ongoing transaction has deadlocked.  Retrying it may help.
        class DeadlockDetected : public TransactionRollback
        {
        public:
            XIAOHTTP_EXPORT explicit DeadlockDetected(const std::string &);
        };

        /// Internal error in internal library
        class InternalError : public XiaoHttpDbException, public std::logic_error
        {
            const std::exception &base() const noexcept override
            {
                return *this;
            }

        public:
            XIAOHTTP_EXPORT explicit InternalError(const std::string &);
        };

        /// Timeout error in when executing the SQL statement.
        class TimeoutError : public XiaoHttpDbException, public std::logic_error
        {
            const std::exception &base() const noexcept override
            {
                return *this;
            }

        public:
            XIAOHTTP_EXPORT explicit TimeoutError(const std::string &);
        };

        /// Error in usage of drogon orm library, similar to std::logic_error
        class UsageError : public XiaoHttpDbException, public std::logic_error
        {
            const std::exception &base() const noexcept override
            {
                return *this;
            }

        public:
            XIAOHTTP_EXPORT explicit UsageError(const std::string &);
        };

        /// Invalid argument passed to drogon orm lib, similar to std::invalid_argument
        class ArgumentError : public XiaoHttpDbException, public std::invalid_argument
        {
            const std::exception &base() const noexcept override
            {
                return *this;
            }

        public:
            XIAOHTTP_EXPORT explicit ArgumentError(const std::string &);
        };

        /// Value conversion failed, e.g. when converting "Hello" to int.
        class ConversionError : public XiaoHttpDbException, public std::invalid_argument
        {
            const std::exception &base() const noexcept override
            {
                return *this;
            }

        public:
            XIAOHTTP_EXPORT explicit ConversionError(const std::string &);
        };

        class RangeError : public XiaoHttpDbException, public std::invalid_argument
        {
            const std::exception &base() const noexcept override
            {
                return *this;
            }

        public:
            XIAOHTTP_EXPORT explicit RangeError(const std::string &);
        };

        /// Query returned an unexpected number of rows.
        class UnexpectedRows : public RangeError
        {
            const std::exception &base() const noexcept override
            {
                return *this;
            }

        public:
            explicit UnexpectedRows(const std::string &msg) : RangeError(msg)
            {
            }
        };

        class FeatureNotSupported : public SqlError
        {
        public:
            explicit FeatureNotSupported(const std::string &err,
                                         const std::string &Q = "",
                                         const char sqlstate[] = nullptr)
                : SqlError(err, Q, sqlstate)
            {
            }

            explicit FeatureNotSupported(const std::string &msg,
                                         const std::string &Q,
                                         const int errcode,
                                         const int extendedErrcode)
                : SqlError(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        /// Error in data provided to SQL statement
        class DataException : public SqlError
        {
        public:
            explicit DataException(const std::string &err,
                                   const std::string &Q = "",
                                   const char sqlstate[] = nullptr)
                : SqlError(err, Q, sqlstate)
            {
            }

            explicit DataException(const std::string &msg,
                                   const std::string &Q,
                                   const int errcode,
                                   const int extendedErrcode)
                : SqlError(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class IntegrityConstraintViolation : public SqlError
        {
        public:
            explicit IntegrityConstraintViolation(const std::string &err,
                                                  const std::string &Q = "",
                                                  const char sqlstate[] = nullptr)
                : SqlError(err, Q, sqlstate)
            {
            }

            explicit IntegrityConstraintViolation(const std::string &msg,
                                                  const std::string &Q,
                                                  const int errcode,
                                                  const int extendedErrcode)
                : SqlError(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class RestrictViolation : public IntegrityConstraintViolation
        {
        public:
            explicit RestrictViolation(const std::string &err,
                                       const std::string &Q = "",
                                       const char sqlstate[] = nullptr)
                : IntegrityConstraintViolation(err, Q, sqlstate)
            {
            }

            explicit RestrictViolation(const std::string &msg,
                                       const std::string &Q,
                                       const int errcode,
                                       const int extendedErrcode)
                : IntegrityConstraintViolation(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class NotNullViolation : public IntegrityConstraintViolation
        {
        public:
            explicit NotNullViolation(const std::string &err,
                                      const std::string &Q = "",
                                      const char sqlstate[] = nullptr)
                : IntegrityConstraintViolation(err, Q, sqlstate)
            {
            }

            explicit NotNullViolation(const std::string &msg,
                                      const std::string &Q,
                                      const int errcode,
                                      const int extendedErrcode)
                : IntegrityConstraintViolation(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class ForeignKeyViolation : public IntegrityConstraintViolation
        {
        public:
            explicit ForeignKeyViolation(const std::string &err,
                                         const std::string &Q = "",
                                         const char sqlstate[] = nullptr)
                : IntegrityConstraintViolation(err, Q, sqlstate)
            {
            }

            explicit ForeignKeyViolation(const std::string &msg,
                                         const std::string &Q,
                                         const int errcode,
                                         const int extendedErrcode)
                : IntegrityConstraintViolation(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class UniqueViolation : public IntegrityConstraintViolation
        {
        public:
            explicit UniqueViolation(const std::string &err,
                                     const std::string &Q = "",
                                     const char sqlstate[] = nullptr)
                : IntegrityConstraintViolation(err, Q, sqlstate)
            {
            }

            explicit UniqueViolation(const std::string &msg,
                                     const std::string &Q,
                                     const int errcode,
                                     const int extendedErrcode)
                : IntegrityConstraintViolation(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class CheckViolation : public IntegrityConstraintViolation
        {
        public:
            explicit CheckViolation(const std::string &err,
                                    const std::string &Q = "",
                                    const char sqlstate[] = nullptr)
                : IntegrityConstraintViolation(err, Q, sqlstate)
            {
            }

            explicit CheckViolation(const std::string &msg,
                                    const std::string &Q,
                                    const int errcode,
                                    const int extendedErrcode)
                : IntegrityConstraintViolation(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class InvalidCursorState : public SqlError
        {
        public:
            explicit InvalidCursorState(const std::string &err,
                                        const std::string &Q = "",
                                        const char sqlstate[] = nullptr)
                : SqlError(err, Q, sqlstate)
            {
            }

            explicit InvalidCursorState(const std::string &msg,
                                        const std::string &Q,
                                        const int errcode,
                                        const int extendedErrcode)
                : SqlError(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class InvalidSqlStatementName : public SqlError
        {
        public:
            explicit InvalidSqlStatementName(const std::string &err,
                                             const std::string &Q = "",
                                             const char sqlstate[] = nullptr)
                : SqlError(err, Q, sqlstate)
            {
            }

            explicit InvalidSqlStatementName(const std::string &msg,
                                             const std::string &Q,
                                             const int errcode,
                                             const int extendedErrcode)
                : SqlError(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class InvalidCursorName : public SqlError
        {
        public:
            explicit InvalidCursorName(const std::string &err,
                                       const std::string &Q = "",
                                       const char sqlstate[] = nullptr)
                : SqlError(err, Q, sqlstate)
            {
            }

            explicit InvalidCursorName(const std::string &msg,
                                       const std::string &Q,
                                       const int errcode,
                                       const int extendedErrcode)
                : SqlError(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class SyntaxError : public SqlError
        {
        public:
            /// Approximate position in string where error occurred, or -1 if unknown.
            const int errorPosition_;

            explicit SyntaxError(const std::string &err,
                                 const std::string &Q = "",
                                 const char sqlstate[] = nullptr,
                                 int pos = -1)
                : SqlError(err, Q, sqlstate), errorPosition_(pos)
            {
            }

            explicit SyntaxError(const std::string &msg,
                                 const std::string &Q,
                                 const int errcode,
                                 const int extendedErrcode,
                                 int pos = -1)
                : SqlError(msg, Q, errcode, extendedErrcode), errorPosition_(pos)
            {
            }
        };

        class UndefinedColumn : public SyntaxError
        {
        public:
            explicit UndefinedColumn(const std::string &err,
                                     const std::string &Q = "",
                                     const char sqlstate[] = nullptr)
                : SyntaxError(err, Q, sqlstate)
            {
            }

            explicit UndefinedColumn(const std::string &msg,
                                     const std::string &Q,
                                     const int errcode,
                                     const int extendedErrcode)
                : SyntaxError(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class UndefinedFunction : public SyntaxError
        {
        public:
            explicit UndefinedFunction(const std::string &err,
                                       const std::string &Q = "",
                                       const char sqlstate[] = nullptr)
                : SyntaxError(err, Q, sqlstate)
            {
            }

            explicit UndefinedFunction(const std::string &msg,
                                       const std::string &Q,
                                       const int errcode,
                                       const int extendedErrcode)
                : SyntaxError(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class UndefinedTable : public SyntaxError
        {
        public:
            explicit UndefinedTable(const std::string &err,
                                    const std::string &Q = "",
                                    const char sqlstate[] = nullptr)
                : SyntaxError(err, Q, sqlstate)
            {
            }

            explicit UndefinedTable(const std::string &msg,
                                    const std::string &Q,
                                    const int errcode,
                                    const int extendedErrcode)
                : SyntaxError(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class InsufficientPrivilege : public SqlError
        {
        public:
            explicit InsufficientPrivilege(const std::string &err,
                                           const std::string &Q = "",
                                           const char sqlstate[] = nullptr)
                : SqlError(err, Q, sqlstate)
            {
            }

            explicit InsufficientPrivilege(const std::string &msg,
                                           const std::string &Q,
                                           const int errcode,
                                           const int extendedErrcode)
                : SqlError(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        /// Resource shortage on the server
        class InsufficientResources : public SqlError
        {
        public:
            explicit InsufficientResources(const std::string &err,
                                           const std::string &Q = "",
                                           const char sqlstate[] = nullptr)
                : SqlError(err, Q, sqlstate)
            {
            }

            explicit InsufficientResources(const std::string &msg,
                                           const std::string &Q,
                                           const int errcode,
                                           const int extendedErrcode)
                : SqlError(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class DiskFull : public InsufficientResources
        {
        public:
            explicit DiskFull(const std::string &err,
                              const std::string &Q = "",
                              const char sqlstate[] = nullptr)
                : InsufficientResources(err, Q, sqlstate)
            {
            }

            explicit DiskFull(const std::string &msg,
                              const std::string &Q,
                              const int errcode,
                              const int extendedErrcode)
                : InsufficientResources(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class OutOfMemory : public InsufficientResources
        {
        public:
            explicit OutOfMemory(const std::string &err,
                                 const std::string &Q = "",
                                 const char sqlstate[] = nullptr)
                : InsufficientResources(err, Q, sqlstate)
            {
            }

            explicit OutOfMemory(const std::string &msg,
                                 const std::string &Q,
                                 const int errcode,
                                 const int extendedErrcode)
                : InsufficientResources(msg, Q, errcode, extendedErrcode)
            {
            }
        };

        class TooManyConnections : public BrokenConnection
        {
        public:
            explicit TooManyConnections(const std::string &err) : BrokenConnection(err)
            {
            }
        };

        using XiaoHttpDbExceptionCallback =
            std::function<void(const XiaoHttpDbException &)>;
    }
}
