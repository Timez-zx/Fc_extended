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
        num_cores = num_cores_per_group * (eps_port_count - host_per_tor)
        print(num_tors, num_aggrs, num_cores)



if __name__ == "__main__":

    clos_temp = clos_basic(20000, 12000, 32)
    clos_temp.bisection_topo(24)
    # throughput = clos_temp.cost_equivalent_clos()
    # print(throughput)