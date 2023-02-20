import math

class clos_basic():

    def __init__(self, switches, servers, ports):
        self.switches = switches
        self.servers = servers
        self.ports = ports


    def cost_equivalent_clos_2_3layer(self):
        num_eps = self.switches
        eps_port_count = self.ports
        num_servers = self.servers
        print(num_eps, num_servers, eps_port_count)
        # check 2-layered clos
        host_per_tor = math.ceil(num_servers / eps_port_count)
        l2_throughput = 0
        if host_per_tor < eps_port_count and 2 * eps_port_count - host_per_tor < num_eps:
            l2_throughput = (eps_port_count - host_per_tor) / host_per_tor
        # check 3-layered clos
        l3_throughput = 0
        for host_per_tor in range(eps_port_count // 2, eps_port_count):
            num_tors = math.ceil(num_servers / host_per_tor)
            num_pods = math.ceil(num_tors / eps_port_count * 2)
            if num_pods > eps_port_count:
                continue
            num_aggrs = (eps_port_count - host_per_tor) * num_pods
            num_aggr_links_per_core = math.floor(eps_port_count / num_pods)
            num_cores_per_group = math.ceil(eps_port_count / 2 / num_aggr_links_per_core)
            num_cores = num_cores_per_group * (eps_port_count - host_per_tor)
            if num_tors + num_aggrs + num_cores <= num_eps:
                print(num_tors, num_aggrs, num_cores, host_per_tor)
                l3_throughput = (eps_port_count - host_per_tor) / host_per_tor
                break
        return max(l2_throughput, l3_throughput)
    

    def bisection_topo_3layer(self, Tor_hosts):
        eps_port_count = self.ports
        num_servers = self.servers
        mini_tor_host = eps_port_count // 2
        for host_per_tor in range(eps_port_count // 2, eps_port_count):
            num_tors = math.ceil(num_servers / host_per_tor)
            num_pods = math.ceil(num_tors / eps_port_count * 2)
            if num_pods > eps_port_count:
                mini_tor_host = host_per_tor
                continue
            mini_tor_host = host_per_tor
            break
        if(Tor_hosts < mini_tor_host):
            print("Tor hosts must be larger than", mini_tor_host)
            exit(1)
        num_tors = math.ceil(num_servers / Tor_hosts)
        num_pods = math.ceil(num_tors / eps_port_count * 2)
        num_aggrs = (eps_port_count - Tor_hosts) * num_pods
        num_aggr_links_per_core = math.floor(eps_port_count / num_pods)
        num_cores_per_group = math.ceil(eps_port_count / 2 / num_aggr_links_per_core)
        num_cores = num_cores_per_group * (eps_port_count - Tor_hosts)
        print(num_tors+num_aggrs+num_cores)
        print(num_tors, num_aggrs, num_cores)
        print(num_aggrs*eps_port_count/2/2)
        return num_aggrs*eps_port_count/2/2
    

    def bisection_topo_4layer(self, Tor_hosts):
        eps_port_count = self.ports
        num_servers = self.servers
        if(num_servers <= eps_port_count**3/4):
            print("Please use 3 layer to evaluate!")
            exit(1)
        mini_tor_host = eps_port_count // 2
        for host_per_tor in range(eps_port_count // 2, eps_port_count):
            num_tors = math.ceil(num_servers / host_per_tor)
            num_pods_layer2 = math.ceil(num_tors / eps_port_count * 2)
            num_aggrs = (eps_port_count - host_per_tor) * num_pods_layer2
            num_aggrs_per_pod = eps_port_count - host_per_tor
            num_pods_layer3 = math.ceil(num_pods_layer2/eps_port_count*2)
            if(num_pods_layer3 > eps_port_count):
                mini_tor_host = host_per_tor
                if(host_per_tor == eps_port_count - 1):
                    mini_tor_host += 1
                continue
            mini_tor_host = host_per_tor
            break
        if(mini_tor_host == eps_port_count):
            print("4 layer is not enough")
            exit(1)         
        if(Tor_hosts < mini_tor_host):
            print("Please use more ports for hosts, at least", mini_tor_host)
            exit(1)
        host_per_tor = Tor_hosts
        num_tors = math.ceil(num_servers / host_per_tor)
        num_pods_layer2 = math.ceil(num_tors / eps_port_count * 2)
        num_aggrs = (eps_port_count - host_per_tor) * num_pods_layer2
        num_aggrs_per_pod = eps_port_count - host_per_tor
        num_pods_layer3 = math.ceil(num_pods_layer2/eps_port_count*2)
        num_cores_per_pod = num_aggrs_per_pod*eps_port_count/2
        num_links_per_core = math.floor(eps_port_count / num_pods_layer3)
        num_cores_per_group = math.ceil(eps_port_count / 2 / num_links_per_core)
        num_cores_layer4 = num_cores_per_group * num_cores_per_pod
        print(num_tors+num_aggrs+num_cores_per_pod*num_pods_layer3+num_cores_layer4)
        print(num_tors, num_aggrs, num_cores_per_pod*num_pods_layer3, num_cores_layer4)
        print(num_cores_per_pod*num_pods_layer3*eps_port_count/2/2)
        

    def topo_cost_3layer(self, Tor_hosts, inter_distance):
        eps_port_count = self.ports
        num_servers = self.servers
        mini_tor_host = eps_port_count // 2
        for host_per_tor in range(eps_port_count // 2, eps_port_count):
            num_tors = math.ceil(num_servers / host_per_tor)
            num_pods = math.ceil(num_tors / eps_port_count * 2)
            if num_pods > eps_port_count:
                mini_tor_host = host_per_tor
                continue
            mini_tor_host = host_per_tor
            break
        if(Tor_hosts < mini_tor_host):
            print("Tor hosts must be larger than", mini_tor_host)
            exit(1)
        num_tors = math.ceil(num_servers / Tor_hosts)
        copper_length = num_servers*2
        num_pods = math.ceil(num_tors / eps_port_count * 2)
        num_aggrs = (eps_port_count - Tor_hosts) * num_pods
        num_aggr_links_per_core = math.floor(eps_port_count / num_pods)
        num_cores_per_group = math.ceil(eps_port_count / 2 / num_aggr_links_per_core)
        num_cores = num_cores_per_group * (eps_port_count - Tor_hosts)
        tranceiver_num = 0
        num_tors_pods = math.ceil(num_tors/num_pods)
        fiber_length = 0
        for pod in range(num_pods):
            for tor in range(num_tors_pods):
                tor_loc = (math.floor(tor/2)+1)*inter_distance
                fiber_length += tor_loc*(eps_port_count - Tor_hosts)
                tranceiver_num += 2*(eps_port_count - Tor_hosts)
        
        for core in range(num_cores):
            core_loc = math.floor((core+24)/48)*inter_distance
            for link_num in range(math.ceil(eps_port_count/2/num_cores_per_group)):
                for pod in range(num_pods):
                    aggre_loc = (math.floor(pod/2)+1)*inter_distance
                    link_len = core_loc + aggre_loc
                    fiber_length += link_len
                    tranceiver_num += 2
        print("The cable length of clos:", fiber_length)
        print("The copper length of clos:", copper_length)
        print("The number of tranceiver:", tranceiver_num)


    def topo_cost_4layer(self, Tor_hosts, inter_distance):
        eps_port_count = self.ports
        num_servers = self.servers
        if(num_servers <= eps_port_count**3/4):
            print("Please use 3 layer to evaluate!")
            exit(1)
        mini_tor_host = eps_port_count // 2
        for host_per_tor in range(eps_port_count // 2, eps_port_count):
            num_tors = math.ceil(num_servers / host_per_tor)
            num_pods_layer2 = math.ceil(num_tors / eps_port_count * 2)
            num_aggrs = (eps_port_count - host_per_tor) * num_pods_layer2
            num_aggrs_per_pod = eps_port_count - host_per_tor
            num_pods_layer3 = math.ceil(num_pods_layer2/eps_port_count*2)
            if(num_pods_layer3 > eps_port_count):
                mini_tor_host = host_per_tor
                if(host_per_tor == eps_port_count - 1):
                    mini_tor_host += 1
                continue
            mini_tor_host = host_per_tor
            break
        if(mini_tor_host == eps_port_count):
            print("4 layer is not enough")
            exit(1)         
        if(Tor_hosts < mini_tor_host):
            print("Please use more ports for hosts, at least", mini_tor_host)
            exit(1)

        host_per_tor = Tor_hosts
        copper_length = num_servers*2
        num_tors = math.ceil(num_servers / host_per_tor)
        num_pods_layer2 = math.ceil(num_tors / eps_port_count * 2)
        num_aggrs = (eps_port_count - host_per_tor) * num_pods_layer2
        num_aggrs_per_pod = eps_port_count - host_per_tor
        num_pods_layer3 = math.ceil(num_pods_layer2/eps_port_count*2)
        num_cores_per_pod = int(num_aggrs_per_pod*eps_port_count/2)
        num_links_per_core = math.floor(eps_port_count / num_pods_layer3)
        num_cores_per_group = math.ceil(eps_port_count / 2 / num_links_per_core)
        num_cores_layer4 = num_cores_per_group * num_cores_per_pod
        print(num_tors+num_aggrs+num_cores_per_pod*num_pods_layer3+num_cores_layer4)
        print(num_tors, num_aggrs, num_cores_per_pod*num_pods_layer3, num_cores_layer4)

        fiber_length = 0
        tranceiver_num = 0
        num_tors_pods = math.ceil(num_tors/num_pods_layer2)
        for pod in range(num_pods_layer2):
            for tor in range(num_tors_pods):
                tor_loc = (math.floor(tor/2)+1)*inter_distance
                fiber_length += tor_loc*(eps_port_count - Tor_hosts)
                tranceiver_num += 2*(eps_port_count - Tor_hosts)
        remain_pods = int(num_pods_layer2)
        for pod_layer3 in range(num_pods_layer3):
            for core in range(num_cores_per_pod):
                core_loc = math.floor(((core+pod_layer3*num_cores_per_pod)+16)/32)*inter_distance
                for pod in range(min(remain_pods, int(eps_port_count/2))):
                    aggre_loc = (math.floor((pod+pod_layer3*eps_port_count/2)/2)+1)*inter_distance
                    link_len = core_loc + aggre_loc
                    fiber_length += link_len
                    tranceiver_num += 2
            remain_pods -= int(eps_port_count/2)
        label3 = 0
        port_taken = 0
        for core4 in range(num_cores_layer4):
            core4_loc = math.floor((core4+8)/16)*inter_distance
            for link_num in range(math.ceil(eps_port_count/2/num_cores_per_group)):
                for pod in range(num_pods_layer3):
                    core3_loc = math.floor(((label3+pod*num_cores_per_pod)+16)/32)*inter_distance
                    if(core3_loc == core4_loc):
                        copper_length += 2
                    else:
                        link_len = abs(core4_loc - core3_loc)
                        fiber_length += link_len
                        tranceiver_num += 2
                port_taken += 1
                if(port_taken == eps_port_count/2):
                    port_taken = 0
                    label3 += 1
        print("The cable length of clos:", fiber_length)
        print("The copper length of clos:", copper_length)
        print("The number of tranceiver:", tranceiver_num)
        


        
        


if __name__ == "__main__":

    clos_temp = clos_basic(100000, 48000, 32)
    clos_temp.topo_cost_4layer(16, 2)
