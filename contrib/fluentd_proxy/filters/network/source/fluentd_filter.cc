#include "contrib/fluentd_proxy/filters/network/source/fluentd_filter.h"

#include "envoy/config/core/v3/base.pb.h"

#include "source/common/buffer/buffer_impl.h"
#include "source/common/common/assert.h"
#include "source/common/common/logger.h"
#include "source/extensions/filters/network/well_known_names.h"

#include "contrib/fluentd_proxy/filters/network/source/fluentd_decoder.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace FluentdProxy {

FluentdFilterConfig::FluentdFilterConfig(const std::string& stat_prefix, Stats::Scope& scope)
    : scope_(scope), stats_(generateStats(stat_prefix, scope)) {}

FluentdFilter::FluentdFilter(FluentdFilterConfigSharedPtr config) : config_(std::move(config)) {}

void FluentdFilter::initializeReadFilterCallbacks(Network::ReadFilterCallbacks& callbacks) {
  callbacks_ = &callbacks;
}

Network::FilterStatus FluentdFilter::onData(Buffer::Instance& data, bool) {
  frontend_buffer_.add(data);
  doDecode(frontend_buffer_);
  return Network::FilterStatus::Continue;
}

Network::FilterStatus FluentdFilter::onWrite(Buffer::Instance& data, bool) {
  // Safety measure just to make sure that if we have a decoding error we keep going and lose stats.
  // This can be removed once we are more confident of this code.
  backend_buffer_.add(data);
  doDecode(backend_buffer_);
  return Network::FilterStatus::Continue;
}

void FluentdFilter::doDecode(Buffer::Instance& buffer) {
  // Clear dynamic metadata.
  envoy::config::core::v3::Metadata& dynamic_metadata =
      callbacks_->connection().streamInfo().dynamicMetadata();
  auto& metadata =
      (*dynamic_metadata.mutable_filter_metadata())[NetworkFilterNames::get().FluentdProxy];
  metadata.mutable_fields()->clear();

  if (!decoder_) {
    decoder_ = createDecoder(*this);
  }

  try {
    decoder_->onData(buffer);
  } catch (EnvoyException& e) {
    ENVOY_LOG(info, "fluentd_proxy: decoding error: {}", e.what());
    frontend_buffer_.drain(frontend_buffer_.length());
    backend_buffer_.drain(backend_buffer_.length());
  }
}

DecoderPtr FluentdFilter::createDecoder(DecoderCallbacks& callbacks) {
  return std::make_unique<DecoderImpl>(callbacks);
}

void FluentdFilter::onProtocolError() { config_->stats_.protocol_errors_.inc(); }

void FluentdFilter::onBelowWriteBufferLowWatermark() { config_->stats_.protocol_errors_.inc(); }

void FluentdFilter::onAboveWriteBufferHighWatermark() { config_->stats_.protocol_errors_.inc(); }

void FluentdFilter::onEvent(Network::ConnectionEvent event) {
  if (event == Network::ConnectionEvent::RemoteClose) {
    config_->stats_.protocol_errors_.inc();
  }
  config_->stats_.protocol_errors_.inc();
}

void FluentdFilter::onHello() { config_->stats_.protocol_errors_.inc(); }

void FluentdFilter::onPing() { config_->stats_.protocol_errors_.inc(); }

void FluentdFilter::onPong() { config_->stats_.protocol_errors_.inc(); }

void FluentdFilter::onMessage() { config_->stats_.protocol_errors_.inc(); }

Network::FilterStatus FluentdFilter::onNewConnection() {
  config_->stats_.sessions_.inc();
  return Network::FilterStatus::Continue;
}

} // namespace FluentdProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
