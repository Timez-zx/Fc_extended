from random import choice

def fc_fix_topo_gene(switches, hosts, ports, vir_layer_degree):
    sw_ports = ports - hosts
    remain_ports = [[j for j in range(1, sw_ports+1)] for i in range(switches)]
    bipart_num = len(vir_layer_degree) - 1
    initial_sub = 0
    for bipart in range(bipart_num,0,-1):
        degree = vir_layer_degree[bipart] - initial_sub
        basic = switches - 1
        remain_list = []
        for sw in range(switches):
            src = sw
            for link in range(degree):
                if(basic == src):
                    remain_list.append(src)
                
                    if(basic == 0):
                        basic = switches - 1
                    else:
                        basic = basic - 1
                dst = basic
                if(len(remain_list) > 0):
                    if(remain_list[0] != src):
                        dst = remain_list[0]
                        remain_list.pop()
                    else:
                        if(basic == 0):
                            basic = switches - 1
                        else:
                            basic = basic - 1
                else:
                    if(basic == 0):
                        basic = switches - 1
                    else:
                        basic = basic - 1
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
    fc_fix_topo_gene(switches, hosts, ports, vir_layer_degree)
