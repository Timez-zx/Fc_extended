import math

class clos_basic():
    def __init__(self, switches, servers, ports):
        self.switches = switches
        self.servers = servers
        self.ports = ports

    def cost_equivalent_clos(self):
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
                print(num_tors, num_aggrs, num_cores)
                l3_throughput = (eps_port_count - host_per_tor) / host_per_tor
                break
        return max(l2_throughput, l3_throughput)
    
    def bisection_topo(self, Tor_hosts):
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
        print(num_tors, num_aggrs, num_cores, num_pods)
        return num_cores*num_pods*num_aggr_links_per_core/2
    
    def topo_cost(self, Tor_hosts, inter_distance):
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

        total_cost = 0
        total_cost += num_servers*150
        num_tors_pods = math.ceil(num_tors/num_pods)
        fiber_length = 0
        for pod in range(num_pods):
            for tor in range(num_tors_pods):
                tor_loc = (math.floor(tor/2)+1)*inter_distance
                total_cost += tor_loc*2*(eps_port_count - Tor_hosts)
                fiber_length += tor_loc*(eps_port_count - Tor_hosts)
                if(tor_loc <= 100):
                    total_cost += 2*(eps_port_count-Tor_hosts)*500
                elif(tor_loc > 100 and tor_loc <= 500):
                    total_cost += 2*(eps_port_count-Tor_hosts)*800
                else:
                    total_cost += 2*(eps_port_count-Tor_hosts)*1000

        for core in range(num_cores):
            core_loc = math.floor((core+32)/64)*inter_distance
            for link_num in range(num_aggr_links_per_core):
                for pod in range(num_pods):
                    aggre_loc = (math.floor(pod/2)+1)*inter_distance
                    link_len = core_loc + aggre_loc
                    total_cost += link_len*2
                    fiber_length += link_len
                    if(link_len <= 100):
                        total_cost += 2*500
                    elif(link_len > 100 and link_len <= 500):
                        total_cost += 2*800
                    else:
                        total_cost += 2*1000
        print(total_cost, fiber_length)

        


if __name__ == "__main__":
    clos_temp = clos_basic(490, 4900, 32)
    # bisection_band = clos_temp.bisection_topo(16)
    # print(bisection_band)
    # clos_temp.topo_cost(16, 2)
    throughput = clos_temp.cost_equivalent_clos()
    print(throughput)