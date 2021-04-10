from mininet.topo import Topo
from mininet.net import Mininet
from mininet.util import dumpNodeConnections
from mininet.log import setLogLevel
from mininet.link import TCLink
from mininet.cli import CLI

class SingleSwitchTopo(Topo):
    "Single switch connected to n hosts."
    def build(self):
        switch1 = self.addSwitch('s1')
        switch2 = self.addSwitch('s2')
        switch3 = self.addSwitch('s3')
        host1 = self.addHost('h1')
        host2 = self.addHost('h2')
        host3 = self.addHost('h3')
        host4 = self.addHost('h4')
        self.addLink(host1, switch1)
        self.addLink(host4, switch1)
        self.addLink(host2, switch2)
        self.addLink(host3, switch3)
        self.addLink(switch1, switch2, bw=10, loss=100)
        self.addLink(switch1, switch3, bw=10, loss=100)
	


def perfTest():
    topo = SingleSwitchTopo()
    net = Mininet(topo, link=TCLink)
    net.start()
    # print( "Dumping host connections" )
    # dumpNodeConnections(net.hosts)
    print( "Testing network connectivity" )
    net.pingAll()
    net.stop()

if __name__ == '__main__':
    # Tell mininet to print useful information
    setLogLevel('info')
    perfTest()
