.. _config_network_filters_fluentd_proxy:

Fluentd proxy
=============

The Fluentd proxy filter decodes the wire protocol between the Fluentd client
and server. It decodes the SQL queries in the payload (SQL99 format only).
The decoded info is emitted as dynamic metadata that can be combined with
access log filters to get detailed information on tables accessed as well
as operations performed on each table. It's a transparent filter, so it
won't affect the normal progress of client and server.

.. attention::

   The fluentd_proxy filter is experimental and is currently under active
   development. Capabilities will be expanded over time and the
   configuration structures are likely to change.

.. warning::

   The fluentd_proxy filter was tested with Fluentd v5.7. The filter may not work
   with other versions of Fluentd due to differences in the protocol implementation.
   The filter won't work when client turns on ssl communication.

.. _config_network_filters_fluentd_proxy_config:

Configuration
-------------

The Fluentd proxy filter should be chained with the TCP proxy filter as shown
in the configuration snippet below:

.. code-block:: yaml

  filter_chains:
  - filters:
    - name: envoy.filters.network.fluentd_proxy
      typed_config:
        "@type": type.googleapis.com/envoy.extensions.filters.network.fluentd_proxy.v3.FluentdProxy
        stat_prefix: fluentd
    - name: envoy.filters.network.tcp_proxy
      typed_config:
        "@type": type.googleapis.com/envoy.extensions.filters.network.tcp_proxy.v3.TcpProxy
        stat_prefix: tcp
        cluster: ...


.. _config_network_filters_fluentd_proxy_stats:

Statistics
----------

Every configured Fluentd proxy filter has statistics rooted at *fluentd.<stat_prefix>.* with the
following statistics:

.. csv-table::
  :header: Name, Type, Description
  :widths: 1, 1, 2

  auth_switch_request, Counter, Number of times the upstream server requested clients to switch to a different authentication method
  decoder_errors, Counter, Number of Fluentd protocol decoding errors
  login_attempts, Counter, Number of login attempts
  login_failures, Counter, Number of login failures
  protocol_errors, Counter, Number of out of sequence protocol messages encountered in a session
  queries_parse_error, Counter, Number of Fluentd queries parsed with errors
  queries_parsed, Counter, Number of Fluentd queries successfully parsed
  sessions, Counter, Number of Fluentd sessions since start
  upgraded_to_ssl, Counter, Number of sessions/connections that were upgraded to SSL

.. _config_network_filters_fluentd_proxy_dynamic_metadata:

Dynamic Metadata
----------------

The Fluentd filter emits the following dynamic metadata for each SQL query parsed:

.. csv-table::
  :header: Name, Type, Description
  :widths: 1, 1, 2

  <table.db>, string, The resource name in *table.db* format. The resource name defaults to the table being accessed if the database cannot be inferred.
  [], list, A list of strings representing the operations executed on the resource. Operations can be one of insert/update/select/drop/delete/create/alter/show.

.. _config_network_filters_fluentd_proxy_rbac:

RBAC Enforcement on Table Accesses
----------------------------------

The dynamic metadata emitted by the Fluentd filter can be used in conjunction
with the RBAC filter to control accesses to individual tables in a
database. The following configuration snippet shows an example RBAC filter
configuration that denies SQL queries with _update_ statements to the
_catalog_ table in the _productdb_ database.

.. code-block:: yaml

  filter_chains:
  - filters:
    - name: envoy.filters.network.fluentd_proxy
      typed_config:
        "@type": type.googleapis.com/envoy.extensions.filters.network.fluentd_proxy.v3.FluentdProxy
        stat_prefix: fluentd
    - name: envoy.filters.network.rbac
      typed_config:
        "@type": type.googleapis.com/envoy.extensions.filters.network.rbac.v3.RBAC
        stat_prefix: rbac
        rules:
          action: DENY
          policies:
            "product-viewer":
              permissions:
              - metadata:
                  filter: envoy.filters.network.fluentd_proxy
                  path:
                  - key: catalog.productdb
                  value:
                    list_match:
                      one_of:
                        string_match:
                          exact: update
              principals:
              - any: true
    - name: envoy.filters.network.tcp_proxy
      typed_config:
        "@type": type.googleapis.com/envoy.extensions.filters.network.tcp_proxy.v3.TcpProxy
        stat_prefix: tcp
        cluster: fluentd
