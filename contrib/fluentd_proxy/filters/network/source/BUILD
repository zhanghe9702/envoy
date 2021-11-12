load(
    "//bazel:envoy_build_system.bzl",
    "envoy_cc_contrib_extension",
    "envoy_cc_library",
    "envoy_contrib_package",
)

licenses(["notice"])  # Apache 2

envoy_contrib_package()

# Fluentd proxy L7 network filter.
# Public docs: docs/root/configuration/network_filters/fluentd_proxy_filter.rst

envoy_cc_library(
    name = "filter_lib",
    srcs = [
        "fluentd_decoder.cc",
        "fluentd_filter.cc",
        "fluentd_message.cc",
    ],
    hdrs = [
        "fluentd_decoder.h",
        "fluentd_filter.h",
        "fluentd_message.h",
        "fluentd_session.h",
    ],
    external_deps = [
        "msgpack",
    ],
    deps = [
        "//envoy/network:connection_interface",
        "//envoy/network:filter_interface",
        "//envoy/server:filter_config_interface",
        "//envoy/stats:stats_interface",
        "//envoy/stats:stats_macros",
        "//source/common/buffer:buffer_lib",
        "//source/common/network:filter_lib",
        "//source/extensions/filters/network:well_known_names",
        "@envoy_api//envoy/config/core/v3:pkg_cc_proto",
    ],
)

envoy_cc_contrib_extension(
    name = "config",
    srcs = ["config.cc"],
    hdrs = ["config.h"],
    deps = [
        ":filter_lib",
        "//source/extensions/filters/network:well_known_names",
        "//source/extensions/filters/network/common:factory_base_lib",
        "@envoy_api//contrib/envoy/extensions/filters/network/fluentd_proxy/v3alpha:pkg_cc_proto",
        "@envoy_api//envoy/config/core/v3:pkg_cc_proto",
    ],
)
