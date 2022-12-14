import random

def change_base(basic, switches):
    if(basic == 0):
        basic = switches - 1
    else:
        basic = basic - 1
    return basic

def fc_topo_gene(switches, hosts, ports, vir_layer_degree, is_random):
    sw_ports = ports - hosts
    remain_ports = [[j for j in range(1, sw_ports+1)] for i in range(switches)]
    bipart_num = len(vir_layer_degree) - 1
    switch_list = [i for i in range(switches)]
    initial_sub = 0
    for bipart in range(bipart_num,0,-1):
        degree = vir_layer_degree[bipart] - initial_sub
        degrees = [degree for i in range(switches)]
        basic = switches - 1
        remain_list = []
        if(is_random):
            random.shuffle(switch_list)
        for sw in range(switches):
            src = switch_list[sw]
            for link in range(degree):
                if(basic == sw):
                    remain_list.append(sw)
                    basic = change_base(basic, switches)
                dst = switch_list[basic]
                while(degrees[dst] == 0):
                    basic = change_base(basic, switches)
                    dst = switch_list[basic]

                if(len(remain_list) > 0):
                    if(remain_list[0] != sw):
                        dst = switch_list[remain_list[0]]
                        basic = remain_list.pop(0)
                        basic = change_base(basic, switches)
                    else:
                        basic = change_base(basic, switches)
                else:
                    basic = change_base(basic, switches)
                degrees[dst] = degrees[dst] - 1
                src_port = remain_ports[src].pop(0)
                dst_port = remain_ports[dst].pop(0)
                print(src, dst, src_port, dst_port)
        initial_sub = degree
        print(bipart)



if __name__ == "__main__":
    switches = 10
    hosts = 24
    ports = 30
    vir_layer_degree = [1, 3, 2]
    is_random = 0
    fc_topo_gene(switches, hosts, ports, vir_layer_degree, is_random)
