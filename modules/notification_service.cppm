module;

#include <simdjson.h>
#include <spdlog/spdlog.h>

#define BOOST_ASIO_NO_DEPRECATED
#define BOOST_ASIO_HAS_IO_URING
#define BOOST_ASIO_DISABLE_EPOLL

#include <fmt/chrono.h>
#include <fmt/format.h>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <chrono>
#include <cstdlib>
#include <format>
#include <memory>
#include <memory_resource>
#include <optional>
#include <type_traits>

export module notification_service;

#define START_EXPORT_SECTION export {
#define END_EXPORT_SECTION }

#define func auto

constexpr unsigned kDefaultServerPort{10'000U};
constexpr int kHTTPVersion{11};
constexpr int kBufferCapacity{1024};

namespace beast = boost::beast;
namespace net = beast::net;
namespace http = beast::http;

template<typename = void>
concept IsNothrowConstructible =
  std::is_nothrow_constructible_v<net::io_context, unsigned> &&
  std::is_nothrow_constructible_v<std::optional<net::ip::tcp::socket>> &&
  std::is_nothrow_constructible_v<net::ip::tcp::acceptor, net::io_context&, net::ip::tcp::endpoint> &&
  std::is_nothrow_constructible_v<net::signal_set, net::io_context&, decltype(SIGINT), decltype(SIGTERM)>;

template<typename T>
class [[nodiscard]] EnableLocalSharedFromThis : public boost::intrusive_ref_counter<T, boost::thread_unsafe_counter> {
 protected:
  constexpr EnableLocalSharedFromThis() noexcept = default;
  constexpr EnableLocalSharedFromThis(const EnableLocalSharedFromThis& /* other */) noexcept = default;
  constexpr auto operator=(const EnableLocalSharedFromThis& /* other */) noexcept
    -> EnableLocalSharedFromThis& = default;
  constexpr ~EnableLocalSharedFromThis() = default;

 public:
  [[nodiscard]] func LocalSharedFromThis() noexcept(std::is_nothrow_constructible_v<boost::intrusive_ptr<T>, T*>)
    -> boost::intrusive_ptr<T> {
    return static_cast<T*>(this);
  }

  [[nodiscard]] func LocalSharedFromThis() const noexcept(
    std::is_nothrow_constructible_v<boost::intrusive_ptr<const T>, const T*>
  ) -> boost::intrusive_ptr<const T> {
    return static_cast<const T*>(this);
  }
};
class [[nodiscard]] Connection final : public EnableLocalSharedFromThis<Connection> {
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
    http::async_read(
      socket_,
      buffer_,
      request_,
      [self =
         LocalSharedFromThis()](boost::system::error_code error_code, [[maybe_unused]] size_t /* bytes */) -> void {
        if (error_code) {
          spdlog::error(
            "[Notification-Service] message: {}",
            sizeof(beast::flat_buffer) + sizeof(http::response<http::string_body>) +
              sizeof(http::request<http::string_body>) + sizeof(net::ip::tcp::socket)
          );  // error_code.message());
          return;
        }
        self->response_.version(self->request_.version());
        self->response_.keep_alive(self->request_.keep_alive());
        const auto current_time{std::chrono::system_clock::now()};
        self->response_.body() = fmt::format("{{\"time\":{}}}\r\n", current_time);
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
      socket_,  //
      response_,
      [self = LocalSharedFromThis()](boost::system::error_code error_code, size_t bytes) -> void {
        if (error_code) {
          spdlog::error("[Notification-Service] message: {}", error_code.message());
          return;
        }
        self->request_.clear();
        self->buffer_.consume(bytes);
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
  http::request<http::string_body> request_;
  http::response<http::string_body> response_{http::status::ok, kHTTPVersion};
  beast::flat_buffer buffer_{kBufferCapacity};
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
        spdlog::error(error_code.message());
        return;
      }
      //      std::make_shared<Connection>(std::move(*socket_))->Start();
      boost::intrusive_ptr<Connection> connection{new Connection{std::move(*socket_)}};
      connection->Start();
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
        spdlog::error(error_code.message());
        std::exit(EXIT_FAILURE);
      }
      spdlog::info("Received signal: {}", signal);
      io_ctx_.stop();
      std::exit(EXIT_SUCCESS);
    });
    AsyncAccept();
    spdlog::info("[Notification-Service] Service started on: {}", acceptor_.local_endpoint().port());
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
