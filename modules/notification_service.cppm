module;

#define BOOST_ASIO_NO_DEPRECATED

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/log/trivial.hpp>
#include <chrono>
#include <cstdlib>
#include <format>
#include <memory>
#include <optional>

export module notification_service;

#define START_EXPORT_SECTION export {
#define END_EXPORT_SECTION }

#ifdef WEB_CRAWLER_DEBUG
  #define LOG_DEBUG() BOOST_LOG_TRIVIAL(debug)
#else
  #define LOG_DEBUG()
#endif

#define LOG_INFO() BOOST_LOG_TRIVIAL(info)
#define LOG_WARNING() BOOST_LOG_TRIVIAL(warning)
#define LOG_ERROR() BOOST_LOG_TRIVIAL(error)
#define LOG_FATAL() BOOST_LOG_TRIVIAL(fatal)

#define func auto

constexpr unsigned kDefaultConcurrency{2U};
constexpr unsigned kDefaultServerPort{10'000U};
constexpr int kHTTPVersion{11};
constexpr int kBufferCapacity{1024};

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace net = beast::net;
namespace http = beast::http;

template<typename = void>
concept IsNothrowConstructible =
  std::is_nothrow_constructible_v<asio::io_context, unsigned> &&
  std::is_nothrow_constructible_v<std::optional<net::ip::tcp::socket>> &&
  std::is_nothrow_constructible_v<net::ip::tcp::acceptor, net::io_context&, net::ip::tcp::endpoint> &&
  std::is_nothrow_constructible_v<net::signal_set, net::io_context&, decltype(SIGINT), decltype(SIGTERM)>;

class [[nodiscard]] Connection final : public std::enable_shared_from_this<Connection> {
 public:
  explicit Connection(net::ip::tcp::socket&& socket) : socket_{std::move(socket)} {
    response_.set(http::field::content_type, "text/plain");
  }

 private:
  /**
   * @private
   * @internal
   */
  func AsyncRead() -> void {
    beast::http::async_read(
      socket_,
      buffer_,
      request_,
      [self = shared_from_this()](boost::system::error_code error_code, [[maybe_unused]] size_t /* bytes */) -> void {
        if (error_code == net::error::eof) {
          LOG_INFO() << "end processing: " << self->socket_.remote_endpoint();
          return;
        } else if (error_code) {
          LOG_ERROR() << error_code.message();
          return;
        }
        self->response_.version(self->request_.version());
        self->response_.keep_alive(self->request_.keep_alive());
        self->response_.body() = std::format("{{\"time\":{}}}", std::chrono::system_clock::now());
        self->response_.prepare_payload();
        self->AsyncWrite();
      }
    );
  }

  /**
   * @private
   * @internal
   */
  func AsyncWrite() -> void {
    http::async_write(
      socket_,
      response_,
      [self = shared_from_this()](boost::system::error_code error_code, [[maybe_unused]] size_t /* bytes */) -> void {
        if (error_code == net::error::eof) {
          LOG_INFO() << "End processing: " << self->socket_.remote_endpoint();
          return;
        } else if (error_code) {
          LOG_ERROR() << error_code.message();
          return;
        }
        self->request_.clear();
        self->AsyncRead();
      }
    );
  }

 public:
  /**
   * @public
   */
  func Start() -> void { AsyncRead(); }

 private:
  net::ip::tcp::socket socket_;
  beast::flat_buffer buffer_{kBufferCapacity};
  http::request<http::string_body> request_;
  http::response<http::string_body> response_{http::status::ok, kHTTPVersion};
};

START_EXPORT_SECTION

/**
 * @brief Namespace for Notification Service Implementation
 * @namespace web
 */
namespace web {

class [[nodiscard]] NotificationService final {
 public:
  /**
   * @public
   */
  explicit NotificationService(net::ip::port_type port) noexcept(IsNothrowConstructible<>)
    : acceptor_{io_ctx_, {net::ip::tcp::v4(), port}} { }

 private:
  /**
   * @private
   * @internal
   */
  func AsyncAccept() -> void {
    socket_.emplace(io_ctx_);
    acceptor_.async_accept(*socket_, [this](boost::system::error_code error_code) -> void {
      if (error_code) {
        LOG_ERROR() << error_code.message();
        return;
      }
      std::make_shared<Connection>(std::move(*socket_))->Start();
      AsyncAccept();
    });
  }

 public:
  /**
   * @public
   */
  func Run() -> void {
    signals_.async_wait([this] [[noreturn]] (boost::system::error_code error_code, int signal) -> void {
      if (error_code) {
        LOG_ERROR() << error_code.message();
        std::exit(EXIT_FAILURE);
      }
      LOG_INFO() << "Received: " << signal;
      io_ctx_.stop();
      std::exit(EXIT_SUCCESS);
    });
    AsyncAccept();
    LOG_INFO() << "[Notification-Service] Service started";
    io_ctx_.run();
  }

 private:
  net::io_context io_ctx_{1};
  std::optional<net::ip::tcp::socket> socket_;
  net::ip::tcp::acceptor acceptor_{io_ctx_, {net::ip::tcp::v4(), kDefaultServerPort}};
  net::signal_set signals_{io_ctx_, SIGINT, SIGTERM};
};

}  // namespace web

END_EXPORT_SECTION
