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

    def get_fiber_price(self, fiber_len):
        if(fiber_len <= 1):
            return 48.5/12
        elif(fiber_len > 1 and fiber_len <= 2):
            return 51.5/12
        elif(fiber_len > 2 and fiber_len <= 3):
            return 53.5/12
        elif(fiber_len > 3 and fiber_len <= 5):
            return 56.5/12
        elif(fiber_len > 5 and fiber_len <= 10):
            return 63.5/12
        elif(fiber_len > 10 and fiber_len <= 15):
            return 68.5/12
        elif(fiber_len > 15 and fiber_len <= 20):
            return 76.5/12
        elif(fiber_len > 20 and fiber_len <= 30):
            return 88.5/12
        elif(fiber_len > 30 and fiber_len <= 50):
            return 113.5/12
        elif(fiber_len > 50 and fiber_len <= 100):
            return 198.5/12
        else:
            return (198.5/12)+150/(12*99)*50*((fiber_len-100)//50+1)
    

    def get_tranceiver_price(self, fiber_len):
        if(fiber_len <= 100):
            return 499
        else:
            return 799

        
    def topo_cost_3layer(self, Tor_hosts, inter_distance):
        print("Infor", self.servers, Tor_hosts)
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
        total_cost = 0
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
                total_cost += 2*self.get_tranceiver_price(tor_loc)*(eps_port_count - Tor_hosts)
                total_cost += self.get_fiber_price(tor_loc)*(eps_port_count - Tor_hosts)
        
        for core in range(num_cores):
            core_loc = math.floor((core+48)/96)*inter_distance
            for link_num in range(math.ceil(eps_port_count/2/num_cores_per_group)):
                for pod in range(num_pods):
                    aggre_loc = (math.floor(pod/2)+1)*inter_distance
                    link_len = core_loc + aggre_loc
                    fiber_length += link_len
                    total_cost += 2*self.get_tranceiver_price(link_len)
                    total_cost += self.get_fiber_price(link_len)
                    tranceiver_num += 2
        
        # total_cost += tranceiver_num*500
        total_cost += num_servers*189
        total_cost += (num_cores+num_aggrs+num_tors)*59099
        print("sw", num_cores+num_aggrs+num_tors)
        print("The cable length of clos:", fiber_length/1000)
        print("The copper length of clos:", copper_length/1000)
        print("The number of tranceiver:", tranceiver_num)
        print("The total cost:", math.ceil(total_cost)/10**6)
        print()


    def topo_cost_4layer(self, Tor_hosts, inter_distance):
        print("Infor", self.servers, Tor_hosts)
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

        total_cost = 0
        host_per_tor = Tor_hosts
        copper_length = num_servers*2
        num_tors = math.ceil(num_servers / host_per_tor)
        num_pods_layer2 = math.ceil(num_tors / eps_port_count * 2)
        num_aggrs = (eps_port_count - host_per_tor) * num_pods_layer2
        num_aggrs_per_pod = eps_port_count - host_per_tor
        num_ports_per_pod = int(num_aggrs_per_pod*eps_port_count/2)
        num_aggr_links_per_core = math.floor(512/num_pods_layer2)
        num_cores_per_group = math.ceil(num_ports_per_pod / num_aggr_links_per_core)
        num_cores = num_cores_per_group * 48
        copper_length += num_cores_per_group*16*32*2
        fiber_length = 0
        tranceiver_num = 0
        num_tors_pods = math.ceil(num_tors/num_pods_layer2)
        for pod in range(num_pods_layer2):
            for tor in range(num_tors_pods):
                tor_loc = (math.floor(tor/2)+1)*inter_distance
                fiber_length += tor_loc*(eps_port_count - Tor_hosts)
                tranceiver_num += 2*(eps_port_count - Tor_hosts)
                total_cost += 2*self.get_tranceiver_price(tor_loc)*(eps_port_count - Tor_hosts)
                total_cost += self.get_fiber_price(tor_loc)*(eps_port_count - Tor_hosts)

        for core in range(num_cores_per_group):
            core_loc = math.floor((core+48)/96)*inter_distance
            for link_num in range(num_aggr_links_per_core):
                for pod in range(num_pods_layer2):
                    aggre_loc = (math.floor(pod/2)+1)*inter_distance
                    link_len = core_loc + aggre_loc
                    total_cost += 2*self.get_tranceiver_price(link_len)
                    total_cost += self.get_fiber_price(link_len)
                    fiber_length += link_len
                    tranceiver_num += 2
        # total_cost += tranceiver_num*500
        total_cost += (copper_length/2)*189
        total_cost += (num_cores+num_aggrs+num_tors)*59099
        print("switch_num",num_tors+num_aggrs+num_cores)
        print("The cable length of clos:", fiber_length/1000)
        print("The copper length of clos:", copper_length/1000)
        print("The number of tranceiver:", tranceiver_num)
        print("The total cost:", math.ceil(total_cost)/10**6)
        print()
        


        
        


if __name__ == "__main__":
    servers = [1152, 2400, 2400, 2400, 4800, 4800, 4800, 7200, 7200, 7200]
    servers2 = [24000, 24000, 24000, 48000, 48000, 48000, 72000, 72000, 72000]
    ports1 = [18, 16, 22, 25, 16, 21, 25, 16, 21 ,25]
    ports = [16, 22, 25, 16, 21, 25, 16, 21 ,25]
    for i in range(10):
        clos_temp = clos_basic(100000, servers[i], 32)
        clos_temp.topo_cost_3layer(ports1[i], 2)
    
    for i in range(9):
        clos_temp = clos_basic(100000, servers2[i], 32)
        clos_temp.topo_cost_4layer(ports[i], 2)
