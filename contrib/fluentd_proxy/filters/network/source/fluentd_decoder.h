#pragma once
#include "envoy/common/platform.h"

#include "contrib/fluentd_proxy/filters/network/source/fluentd_message.h"
#include "contrib/fluentd_proxy/filters/network/source/fluentd_session.h"
#include "include/msgpack.hpp"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace FluentdProxy {

class DecoderCallbacks {
public:
  virtual ~DecoderCallbacks() = default;
  virtual void onProtocolError() PURE;
  virtual void onHello() PURE;
  virtual void onPing() PURE;
  virtual void onPong() PURE;
  virtual void onMessage() PURE;
};
/**
 *  Fluentd message decoder
 */

class Decoder {
public:
  virtual ~Decoder() = default;
  virtual void onData(Buffer::Instance& data) PURE;
  virtual FluentdSession& getSession() PURE;

protected:
  std::map<std::string, std::string> options_;
};

using DecoderPtr = std::unique_ptr<Decoder>;

class DecoderImpl : public Decoder, public Logger::Loggable<Logger::Id::filter> {
public:
  DecoderImpl(DecoderCallbacks& callbacks) : callbacks_(callbacks) {}

  // FluentdProxy::Decoder
  void onData(Buffer::Instance& data) override;
  FluentdSession& getSession() { return session_; }

private:
  bool decode(Buffer::Instance& data);
  void parseMessage(Buffer::Instance& message, uint32_t len);

  DecoderCallbacks& callbacks_;
  FluentdSession session_;
};

} // namespace FluentdProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
