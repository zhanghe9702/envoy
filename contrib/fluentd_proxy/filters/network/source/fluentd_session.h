#pragma once
#include "source/common/common/logger.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace FluentdProxy {
/**
 * implementation of  https://github.com/fluent/fluentd/wiki/Forward-Protocol-Specification-v1
 */
class FluentdSession : Logger::Loggable<Logger::Id::filter> {
public:
  enum class State {
    Establish = 1,
    Hello = 2,
    PingPong = 3,
    Transfer = 4,
  };
  State getState() { return state_; };
  void setState(State state) { state_ = state; };

private:
  State state_{State::Establish};
};
}; // namespace FluentdProxy
}; // namespace NetworkFilters
}; // namespace Extensions
}; // namespace Envoy