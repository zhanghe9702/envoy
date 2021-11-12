#pragma once

#include "envoy/network/connection.h"
#include "envoy/network/filter.h"
#include "envoy/stats/scope.h"

#include "contrib/fluentd_proxy/filters/network/source/fluentd_decoder.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace FluentdProxy {

/**
 * All fluentd proxy stats. @see stats_macros.h
 */
#define ALL_FLUENTD_PROXY_STATS(COUNTER)                                                           \
  COUNTER(sessions)                                                                                \
  COUNTER(protocol_errors)

struct FluntdStats {
  ALL_FLUENTD_PROXY_STATS(GENERATE_COUNTER_STRUCT)
};
class FluentdFilterConfig {
public:
  FluentdFilterConfig(const std::string& stat_prefix, Stats::Scope& scope);
  const FluntdStats& stats() { return stats_; }
  Stats::Scope& scope_;
  FluntdStats stats_;

private:
  FluntdStats generateStats(const std::string& prefix, Stats::Scope& scope) {
    return FluntdStats{ALL_FLUENTD_PROXY_STATS(POOL_COUNTER_PREFIX(scope, prefix))};
  }
};
using FluentdFilterConfigSharedPtr = std::shared_ptr<FluentdFilterConfig>;

/**
 * A fluentd multiplexing proxy filter.
 */
class FluentdFilter : public Network::Filter,
                      Network::ConnectionCallbacks,
                      DecoderCallbacks,
                      Logger::Loggable<Logger::Id::filter> {
public:
  FluentdFilter(FluentdFilterConfigSharedPtr config);
  ~FluentdFilter() override = default;
  // Network::ReadFilter
  void initializeReadFilterCallbacks(Network::ReadFilterCallbacks& callbacks) override;
  Network::FilterStatus onData(Buffer::Instance& data, bool end_stream) override;
  Network::FilterStatus onNewConnection() override;

  // Network::WriteFilter
  Network::FilterStatus onWrite(Buffer::Instance& data, bool end_stream) override;

  // Network::ConnectionCallbacks
  void onEvent(Network::ConnectionEvent event) override;
  void onAboveWriteBufferHighWatermark() override;
  void onBelowWriteBufferLowWatermark() override;

  // FluentdProxy::DecoderCallback
  void onProtocolError();
  void onHello();
  void onPing();
  void onPong();
  void onMessage();

  void doDecode(Buffer::Instance& buffer);
  DecoderPtr createDecoder(DecoderCallbacks& callbacks);

private:
  friend class FluentdProxyFilterTest;
  std::unique_ptr<Decoder> decoder_;
  FluentdFilterConfigSharedPtr config_{};
  Buffer::OwnedImpl frontend_buffer_{};
  Buffer::OwnedImpl backend_buffer_{};
  Network::ReadFilterCallbacks* callbacks_{};
};
} // namespace FluentdProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy