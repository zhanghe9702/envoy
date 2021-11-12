#include "contrib/fluentd_proxy/filters/network/source/config.h"

#include <string>

#include "envoy/registry/registry.h"
#include "envoy/server/filter_config.h"

#include "source/common/common/logger.h"

#include "contrib/envoy/extensions/filters/network/fluentd_proxy/v3alpha/fluentd_proxy.pb.h"
#include "contrib/envoy/extensions/filters/network/fluentd_proxy/v3alpha/fluentd_proxy.pb.validate.h"
#include "contrib/fluentd_proxy/filters/network/source/fluentd_filter.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace FluentdProxy {

/**
 * Config registration for the Fluentd proxy filter. @see NamedNetworkFilterConfigFactory.
 */
Network::FilterFactoryCb
NetworkFilters::FluentdProxy::FluentdConfigFactory::createFilterFactoryFromProtoTyped(
    const envoy::extensions::filters::network::fluentd_proxy::v3alpha::FluentdProxy& proto_config,
    Server::Configuration::FactoryContext& context) {

  ASSERT(!proto_config.stat_prefix().empty());

  const std::string stat_prefix = fmt::format("fluentd.{}", proto_config.stat_prefix());

  FluentdFilterConfigSharedPtr filter_config(
      std::make_shared<FluentdFilterConfig>(stat_prefix, context.scope()));
  return [filter_config](Network::FilterManager& filter_manager) -> void {
    filter_manager.addFilter(std::make_shared<FluentdFilter>(filter_config));
  };
}

/**
 * Static registration for the Fluentd proxy filter. @see RegisterFactory.
 */
REGISTER_FACTORY(FluentdConfigFactory, Server::Configuration::NamedNetworkFilterConfigFactory);

} // namespace FluentdProxy
} // namespace NetworkFilters
} // namespace Extensions
} // namespace Envoy