

def fc_topo_gene(switches, hosts, ports, vir_layer_degree):
    sw_ports = ports - hosts
    remain_ports = [[j for j in range(1, sw_ports+1)] for i in range(switches)]











if __name__ == "__main__":
    switches = 3
    hosts = 24
    ports = 28
    vir_layer_degree = [1, 2, 1]
    fc_topo_gene(switches, hosts, ports, vir_layer_degree)
