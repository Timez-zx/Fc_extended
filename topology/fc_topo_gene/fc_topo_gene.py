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
    topo_index = [[[] for i in range(switches)] for j in range(bipart_num)]
    for bipart in range(bipart_num,0,-1):
        degree = vir_layer_degree[bipart] - initial_sub
        degrees = [degree for i in range(switches)]
        basic = switches - 1
        remain_list = []
        if(is_random):
            random.shuffle(switch_list)
        for sw in range(switches):
            src = switch_list[sw]
            topo_index[bipart-1][src].append(src)
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
                topo_index[bipart-1][src].append(dst)
                src_port = remain_ports[src].pop(0)
                dst_port = remain_ports[dst].pop(0)
        initial_sub = degree
    return topo_index

def search_path(topo_index, root, topo_dic):
    root_path = [[root]]
    bipart_num = len(topo_index)
    drop_num = 0
    for bipart in range(bipart_num-1,-1,-1):
        root_num = len(root_path)
        for path_num in range(root_num):
            path = root_path.pop(0)
            layer_root = path[-1]
            tp_index = topo_index[bipart][layer_root]
            for link in tp_index:
                if((bipart != 0) | (link != path[-1])):
                    if((link not in path) | (link == path[-1])):
                        path.append(link)
                        root_path.append([i for i in path])
                        path.pop()
                    else:
                        if(bipart == 0):
                            drop_num += 1
                else:
                    if(bipart == 0):
                        drop_num += 1
            if(drop_num == len(tp_index)):
                root_path.append([i for i in path])
            drop_num = 0
    # 构造两层字典比单纯列表消耗更大
    for i in range(len(root_path)):
        path = root_path[i]
        for node_num in range(len(path)):
            if(path[node_num] != root):
                if(root not in topo_dic[path[node_num]].keys()):
                    topo_dic[path[node_num]][root] = [i,node_num*(-1)]
                else:
                    if(i not in topo_dic[path[node_num]][root]):
                        topo_dic[path[node_num]][root].append(i)
                        topo_dic[path[node_num]][root].append(node_num*(-1))
    return root_path,topo_dic

def find_route_path(src, dst, all_path, topo_dic, layers):
    route_path = []
    src_path_infor = topo_dic[dst][src]
    for path_index in range(0,len(src_path_infor),2):
        aroute_path = []
        path = all_path[src][src_path_infor[path_index]]
        loca = src_path_infor[path_index+1]*(-1)
        aroute_path.append((path[loca],layers+loca*-1))
        loca -= 1
        while(path[loca] != src):
            aroute_path.append((path[loca], layers+loca*-1))
            loca -= 1
        aroute_path.append((path[loca], layers+loca*-1))
        if(aroute_path not in route_path):
            route_path.append(aroute_path)

    dst_path_infor = topo_dic[src][dst]
    for path_index in range(0,len(dst_path_infor),2):
        aroute_path = []
        path = all_path[dst][dst_path_infor[path_index]]
        loca = dst_path_infor[path_index+1]*(-1)
        aroute_path.append((path[loca],layers+loca*-1))
        loca -= 1
        while(path[loca] != dst):
            aroute_path.append((path[loca], layers+loca*-1))
            loca -= 1
        aroute_path.append((path[loca], layers+loca*-1))
        if(aroute_path not in route_path):
            route_path.append(aroute_path)
    src_keys = set(topo_dic[src].keys())
    dst_keys = set(topo_dic[dst].keys())
    if(dst in src_keys):
        src_keys.remove(dst)
    if(src in dst_keys):
        dst_keys.remove(src)
    inter_node = src_keys & dst_keys
    print(inter_node)
    return route_path

def route_generate(topo_index, switches):
    all_path = []
    topo_dic = {}
    for i in range(switches):
        topo_dic[i] = {}
    for sw in range(switches):
        root_path,topo_dic= search_path(topo_index, sw, topo_dic)
        all_path.append(root_path)
        print(root_path)
    print(topo_dic)
    layers = len(all_path)
    route_path = find_route_path(0, 2, all_path, topo_dic, layers)
    print(route_path)
    


if __name__ == "__main__":
    switches = 3
    hosts = 24
    ports = 30
    vir_layer_degree = [1,2,2,1]
    is_random = 0
    # switches = 5000
    # hosts = 24
    # ports = 64
    # vir_layer_degree = [5,10,10,10,5]
    # is_random = 1
    topo_index = fc_topo_gene(switches, hosts, ports, vir_layer_degree, is_random)
    route_generate(topo_index, switches)

