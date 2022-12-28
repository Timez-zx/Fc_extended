from fc_topo_all_route import Fc_topo_all_route

class Fc_topo_disjoin_route(Fc_topo_all_route):

    def __init__(self, switches, hosts, ports, vir_layer_degree, is_random, random_seed):
        Fc_topo_all_route.__init__(self, switches, hosts, ports, vir_layer_degree, is_random, random_seed)

if __name__ == "__main__":
    switches = 5
    hosts = 24
    ports = 36
    vir_layer_degree = [2,4,4,2]
    is_random = 1
    random_seed = 3
    fc_demo = Fc_topo_disjoin_route(switches, hosts, ports, vir_layer_degree, is_random, random_seed)
    fc_demo.fc_topo_gene()
    fc_demo.route_infor_generate()
    thread_num = 2
    if_report = 1
    report_num = 2
    if_save = 1
    # fc_demo.route_gene(thread_num, if_report, report_num, if_save)
    fc_demo.route_read()