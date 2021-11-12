#pragma once

#include "envoy/api/api.h"
#include "envoy/config/core/v3/base.pb.h"

#include "source/extensions/filters/network/common/factory_base.h"
#include "source/extensions/filters/network/well_known_names.h"

#include "contrib/envoy/extensions/filters/network/fluentd_proxy/v3alpha/fluentd_proxy.pb.h"
#include "contrib/envoy/extensions/filters/network/fluentd_proxy/v3alpha/fluentd_proxy.pb.validate.h"

namespace Envoy {
namespace Extensions {
namespace NetworkFilters {
namespace FluentdProxy {

/**
 * Config registration for the fluentd proxy filter. @see NamedNetworkFilterConfigFactory.
 */
class FluentdConfigFactory
    : public Common::FactoryBase<
          envoy::extensions::filters::network::fluentd_proxy::v3alpha::FluentdProxy> {
public:
  FluentdConfigFactory() : FactoryBase(NetworkFilterNames::get().FluentdProxy) {}

private:
  Network::FilterFactoryCb createFilterFactoryFromProtoTyped(
      const envoy::extensions::filters::network::fluentd_proxy::v3alpha::FluentdProxy& proto_config,
      Server::Configuration::FactoryContext& context) override;
};

} // namespace FluentdProxy

} // namespace NetworkFilters

} // namespace Extensions

} // namespace Envoy