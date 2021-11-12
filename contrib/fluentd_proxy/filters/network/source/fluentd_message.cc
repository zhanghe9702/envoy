#include "contrib/fluentd_proxy/filters/network/source/fluentd_message.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace FluentdProxy {
std::string Event::toString() const { return value_; }
} // namespace FluentdProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy
