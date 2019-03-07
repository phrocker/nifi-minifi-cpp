#!/bin/sh -e
# (c) 2018 Hortonworks, Inc. All rights reserved.
#
#  This code is provided to you pursuant to your written agreement with Hortonworks, which may be the terms of the
#  Affero General Public License version 3 (AGPLv3), or pursuant to a written agreement with a third party authorized
#  to distribute this code.  If you do not have a written agreement with Hortonworks or with an authorized and
#  properly licensed third party, you do not have any rights to this code.
#
#  If this code is provided to you under the terms of the AGPLv3:
#   (A) HORTONWORKS PROVIDES THIS CODE TO YOU WITHOUT WARRANTIES OF ANY KIND;
#   (B) HORTONWORKS DISCLAIMS ANY AND ALL EXPRESS AND IMPLIED WARRANTIES WITH RESPECT TO THIS CODE, INCLUDING BUT NOT
#       LIMITED TO IMPLIED WARRANTIES OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE;
#   (C) HORTONWORKS IS NOT LIABLE TO YOU, AND WILL NOT DEFEND, INDEMNIFY, OR HOLD YOU HARMLESS FOR ANY CLAIMS ARISING
#       FROM OR RELATED TO THE CODE; AND
#   (D) WITH RESPECT TO YOUR EXERCISE OF ANY RIGHTS GRANTED TO YOU FOR THE CODE, HORTONWORKS IS NOT LIABLE FOR ANY
#       DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, PUNITIVE OR CONSEQUENTIAL DAMAGES INCLUDING, BUT NOT LIMITED
#       TO, DAMAGES RELATED TO LOST REVENUE, LOST PROFITS, LOSS OF INCOME, LOSS OF BUSINESS ADVANTAGE OR
#       UNAVAILABILITY, OR LOSS OR CORRUPTION OF DATA.

# Incorporate helper functions
. /opt/commons/commons.sh

# Example config block for C2 Heartbeating and Reporting
# This block serves as the basis for the below transforms
#
# nifi.c2.root.classes=DeviceInfoNode,AgentInformation,FlowInformation,RepositoryMetrics
# nifi.c2.root.class.definitions.DeviceInfo.name=deviceinfo
# nifi.c2.root.class.definitions.DeviceInfo.classes=DeviceInfoNode
# nifi.flow.metrics.class.definitions=15
# nifi.flow.metrics.class.definitions.15=GetFileMetrics
# c2.rest.url=http://localhost:10080/c2/api/c2-protocol/heartbeat
# c2.rest.url.ack=http://localhost:10080/c2/api/c2-protocol/acknowledge
# c2.agent.heartbeat.period=1000
# c2.agent.heartbeat.reporter.classes=RESTReceiver
# nifi.flow.engine.threads=10
# nifi.c2.enable=true
# c2.agent.listen=false
# c2.rest.listener.port=8765
# c2.rest.listener.heartbeat.rooturi=/path
# controller.socket.local.any.interface=true
# controller.socket.host=localhost
# #controller.socket.host=10.200.31.223
# controller.socket.port=9997
# c2.rest.heartbeat.minimize.updates=true

export properties_file='/opt/minifi/nifi-minifi-cpp-0.6.0/conf/minifi.properties'

prop_replace_or_add 'nifi.c2.agent.class'                                 "${AGENT_CLASS:-default}"
prop_replace_or_add 'nifi.c2.root.classes'                                'DeviceInfoNode,AgentInformation,FlowInformation,RepositoryMetrics'
prop_replace_or_add 'nifi.c2.root.class.definitions.DeviceInfo.name'      'deviceinfo'
prop_replace_or_add 'nifi.c2.root.class.definitions.DeviceInfo.classes'   'DeviceInfoNode'
prop_replace_or_add 'nifi.flow.metrics.class.definitions'                 '15'
prop_replace_or_add 'nifi.flow.metrics.class.definitions.15'              'GetFileMetrics'
prop_replace_or_add 'c2.rest.url'                                         'http://127.0.0.1:10080/c2/api/c2-protocol/heartbeat'
prop_replace_or_add 'c2.rest.url.ack'                                     'http://127.0.0.1:10080/c2/api/c2-protocol/acknowledge'
prop_replace_or_add 'c2.agent.heartbeat.period'                           '2000'
prop_replace_or_add 'c2.agent.protocol.class' 		                  'RESTSender'
prop_replace_or_add 'nifi.c2.agent.coap.host' 		                  '127.0.0.1'
prop_replace_or_add 'nifi.c2.agent.coap.port' 		                  '8787'
prop_replace_or_add 'nifi.flow.engine.threads'                            '1'
prop_replace_or_add 'nifi.c2.enable'                                      'true'
prop_replace_or_add 'c2.agent.listen'                                     'false'
prop_replace_or_add 'c2.rest.listener.port'                               '8765'
prop_replace_or_add 'c2.rest.listener.heartbeat.rooturi'                  '/path'
prop_replace_or_add 'controller.socket.local.any.interface'               'true'
prop_replace_or_add 'controller.socket.host'                              'localhost'
prop_replace_or_add 'controller.socket.port'                              '9997'
prop_replace_or_add '#c2.rest.heartbeat.minimize.updates'                 'true'
prop_replace_or_add 'nifi.provenance.repository.max.storage.size'         '1 MB'
#prop_replace_or_add 'appender.stdout'   'stdout' '/opt/minifi/nifi-minifi-cpp-0.6.0/conf/minifi-log.properties'
#prop_replace_or_add 'logger.root'	'TRACE,rolling' '/opt/minifi/nifi-minifi-cpp-0.6.0/conf/minifi-log.properties'
#0rop_replace_or_add 'logger.org.apache.nifi.minifi'	'DEBUG,stdout' '/opt/minifi/nifi-minifi-cpp-0.6.0/conf/minifi-log.properties'
# prop_replace_or_add 'nifi.c2.agent.identifier'                            'testidentifier'  # set this to use a hard-coded, fixed ID instead of a random ID
