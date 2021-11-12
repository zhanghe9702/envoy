#pragma once

#include "source/common/buffer/buffer_impl.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace FluentdProxy {

class Message {
  virtual ~Message() = default;
  virtual std::string toString() const PURE;
};

class Event {
  std::string toString() const;

private:
  std::string value_;
};

} // namespace FluentdProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
