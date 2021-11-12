#include "contrib/fluentd_proxy/filters/network/source/fluentd_decoder.h"

#include "source/common/common/logger.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace FluentdProxy {

void DecoderImpl::parseMessage(Buffer::Instance& message, uint32_t len) {
  ENVOY_LOG(trace, "fluentd_proxy: parsing message, len {}", len);
  switch (session_.getState()) {
  case FluentdSession::State::Establish: {
    break;
  }
  case FluentdSession::State::Hello: {
    break;
  }

  default:
    message.drain(len);
    break;
  }

  ENVOY_LOG(trace, "fluentd_proxy: msg parsed, session in state {}",
            static_cast<int>(session_.getState()));
}

bool DecoderImpl::decode(Buffer::Instance& data) {
  ENVOY_LOG(trace, "fluentd_proxy: decoding {} bytes", data.length());
  uint32_t len = 0;

  ENVOY_LOG(trace, "fluentd_proxy:  len {}", len);
  // If message is split over multiple packets, hold off until the entire message is available.
  // Consider the size of the header here as it's not consumed yet.
  if (sizeof(uint32_t) + len > data.length()) {
    return false;
  }

  callbacks_.onMessage();

  const ssize_t data_len = data.length();
  parseMessage(data, len);
  const ssize_t consumed_len = data_len - data.length();
  data.drain(len - consumed_len); // Ensure that the whole message was consumed

  ENVOY_LOG(trace, "fluentd_proxy: {} bytes remaining in buffer", data.length());
  return true;
}

void DecoderImpl::onData(Buffer::Instance& data) {
  while (decode(data)) {
  }
}

} // namespace FluentdProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
