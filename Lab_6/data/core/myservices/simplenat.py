#
# CORE
# Copyright (c)2010-2012 the Boeing Company.
# See the LICENSE file included in this distribution.
#
''' Sample user-defined service.
'''

import os

from core.service import CoreService, addservice
from core.misc.ipaddr import IPv4Prefix, IPv6Prefix

class SimpleNAT(CoreService):
    ''' This is a sample user-defined service. 
    '''
    # a unique name is required, without spaces
    _name = "SimpleNAT"
    # you can create your own group here
    _group = "Utility"
    # list of other services this service depends on
    _depends = ()
    # per-node directories
    _dirs = ()
    # generated files (without a full path this file goes in the node's dir,
    #  e.g. /tmp/pycore.12345/n1.conf/)
    _configs = ('confnat.sh', )
    # this controls the starting order vs other enabled services
    _startindex = 10
    # list of startup commands, also may be generated during startup
    _startup = ('sh confnat.sh',)
    # list of shutdown commands
    _shutdown = ()

    @classmethod
    def generateconfig(cls, node, filename, services):
        ''' Configures NAT rules in the device and creates Private IP Pool.
            Return a string that will be written to filename, or sent to the
            GUI for user customization.
        '''
        cfg = "#!/bin/sh\n"
        cfg += "# auto-generated by SimpleNAT (simplenat.py)\n"
        for ifc in node.netifs():
            if hasattr(ifc, 'control') and ifc.control == True:
                continue
            cfg += "\n".join(map(cls.addrstr, ifc.addrlist))
            cfg += "\n"
        return cfg

    @staticmethod
    def addrstr(x):
        if x.find(":") >= 0:
            net = IPv6Prefix(x)
        else:
            net = IPv4Prefix(x)
            addr = x.split("/")[0]
        if net.maxaddr() == net.minaddr():
            return ""
        else:
            if os.uname()[0] == "Linux":
               natcmd = "iptables -t nat -A POSTROUTING -s 192.168.0.0/24 -o eth0 -j SNAT --to "
            else:
                raise Exception, "unknown platform"
            return "%s %s" % (natcmd, addr)

# this line is required to add the above class to the list of available services
addservice(SimpleNAT)

