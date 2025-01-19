/**
 * @file WebSocketConnection.h
 * @author Guo Xiao (746921314@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-01-14
 *
 *
 */

#pragma once

#include <memory>
#include <xiaoHttp/HttpTypes.h>
#include <xiaoNet/net/InetAddress.h>

namespace xiaoHttp
{
   enum class CloseCode
   {
      /*1000 indicates a normal closure, meaning that the purpose for which the
         connection was established has been fulfilled.*/
      kNormalClosure = 1000,
      /*1001 indicates that an endpoint is "going away", such as a server going
         down or a browser having navigated away from a page.*/
      kEndpointGone = 1001,
      /*1002 indicates that an endpoint is terminating the connection due to a
         protocol error.*/
      kProtocolError = 1002,
      /*1003 indicates that an endpoint is terminating the connection because it
         has received a type of data it cannot accept (e.g., an endpoint that
         understands only text data MAY send this if it receives a binary
         message).*/
      kInvalidMessage = 1003,
      /*1005 is a reserved value and MUST NOT be set as a status code in a Close
         control frame by an endpoint.  It is designated for use in applications
         expecting a status code to indicate that no status code was actually
         present.*/
      kNone = 1005,
      /*1006 is a reserved value and MUST NOT be set as a status code in a Close
         control frame by an endpoint.  It is designated for use in applications
         expecting a status code to indicate that the connection was closed
         abnormally, e.g., without sending or receiving a Close control frame.
      */
      kAbnormally = 1006,
      /*1007 indicates that an endpoint is terminating the connection because it
         has received data within a message that was not consistent with the type
         of the message (e.g., non-UTF-8 [RFC3629] data within a text message).*/
      kWrongMessageContent = 1007,
      /*1008 indicates that an endpoint is terminating the connection because it
         has received a message that violates its policy.  This is a generic
         status code that can be returned when there is no other more suitable
         status code (e.g., 1003 or 1009) or if there is a need to hide specific
         details about the policy.
      */
      kViolation = 1008,
      /*1009 indicates that an endpoint is terminating the connection because it
         has received a message that is too big for it to process.*/
      kMessageTooBig = 1009,
      /*1010 indicates that an endpoint (client) is terminating the connection
         because it has expected the server to negotiate one or more extension,
         but the server didn't return them in the response message of the
         WebSocket handshake.  The list of extensions that are needed SHOULD
         appear in the /reason/ part of the Close frame. Note that this status
         code is not used by the server, because it can fail the WebSocket
         handshake instead.*/
      kNeedMoreExtensions = 1010,
      /*1011 indicates that a server is terminating the connection because it
         encountered an unexpected condition that prevented it from fulfilling the
         request.*/
      kUnexpectedCondition = 1011,
      /*1015 is a reserved value and MUST NOT be set as a status code in a Close
         control frame by an endpoint.  It is designated for use in applications
         expecting a status code to indicate that the connection was closed due to
         a failure to perform a TLS handshake (e.g., the server certificate can't
         be verified).*/
      kTLSFailed = 1015
   };

   /**
    * @brief The WebSocket connection abstract class.
    *
    */
   class WebSocketConnection
   {
   public:
      WebSocketConnection() = default;
      virtual ~WebSocketConnection() {};

      /**
       * @brief Send a message to the peer
       *
       * @param msg
       * @param len
       * @param type
       */
      virtual void send(
          const char *msg,
          uint64_t len,
          const WebSocketMessageType type = WebSocketMessageType::Text) = 0;

      virtual void send(
          std::string_view msg,
          const WebSocketMessageType type = WebSocketMessageType::Text) = 0;

      // Return the local IP address and port number of the connection
      virtual const xiaoNet::InetAddress &localAddr() const = 0;

      // Return the remote IP address and port number of the connection
      virtual const xiaoNet::InetAddress &peerAddr() const = 0;

      // Return true if the connection is open
      virtual bool connected() const = 0;

      // Return true if the connection is closed
      virtual bool disconnected() const = 0;

      /**
       * @brief Shut down the write direction, which means that further send
       * operations are disabled.
       *
       * @param code
       * @param reason
       */
      virtual void shutdown(const CloseCode code = CloseCode::kNormalClosure,
                            const std::string &reason = "") = 0;

      virtual void forceClose() = 0;

      /**
       * @brief Set custom data on the connection
       *
       * @param context the custom data.
       */
      void setContext(const std::shared_ptr<void> &context)
      {
         contextPtr_ = context;
      }

      void setContext(std::shared_ptr<void> &&context)
      {
         contextPtr_ = std::move(context);
      }

      /**
       * @brief Get custom data from the connection
       *
       * @tparam T
       * @return std::shared_ptr<T>
       */
      template <typename T>
      std::shared_ptr<T> getContext() const
      {
         return std::static_pointer_cast<T>(contextPtr_);
      }

      template <typename T>
      T &getContextRef() const
      {
         return *(static_cast<T *>(contextPtr_.get()));
      }

      bool hasContext()
      {
         return (bool)contextPtr_;
      }

      void clearContext()
      {
         contextPtr_.reset();
      }

      /**
       * @brief Set the heartbeat(ping) message sent to the peer.
       *
       * @param message
       * @param interval
       * @note
       * Both the server and the client in xiaoHttp automatically send the pong
       * message after receiving the ping message.
       * An empty ping message is sent every 30 seconds by default. The method
       * overrides the defautl behavior.
       */
      virtual void setPingMessage(
          const std::string &message,
          const std::chrono::duration<double> &interval) = 0;

      virtual void disablePing() = 0;

   private:
      std::shared_ptr<void>
          contextPtr_;
   };

   using WebSocketConnectionPtr = std::shared_ptr<WebSocketConnection>;

}
