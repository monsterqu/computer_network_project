#! /bin/bash
ovs-ofctl del-flows s1
ovs-ofctl del-flows s2
ovs-ofctl del-flows s3
sudo ovs-ofctl add-flow s3 in_port=s3-eth2,actions=drop
sudo ovs-ofctl add-flow s2 in_port=s2-eth2,actions=drop
