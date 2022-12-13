
from os import link


def search_root_path(root, topo_mat):
    src_neighbor = topo_mat[root]
    path = []
    root_path = {}
    path.append(root)
    
    for i in range(len(src_neighbor)):
        if(src_neighbor[i] == 1):
            topo_mat[root][i] = 0
            topo_mat[i][root] = 0
            path.append(i)
            root_path[i] = [k for k in path]
            node_path = search_root_path(i, topo_mat)
            for key in node_path:
                root_path[key] = root_path[i] + node_path[key][1:]
            path.pop()
    return root_path
    

def seperate_node_type(group_index, member_num, switches):
    inter_node = [i for i in range(group_index*member_num, (group_index+1)*member_num)]
    leaf_node = [i for i in range(switches)]
    for i in inter_node:
        leaf_node.remove(i)
    return inter_node, leaf_node


def edst_topo_generate(hosts, ports, switches):
    sw_ports = ports - hosts
    group_number = sw_ports // 2
    max_node_sons = group_number
    member_num = switches // group_number
    remain_ports = [[j for j in range(1, sw_ports+1)] for i in range(switches)]
    route_path = [[] for i in range(switches*switches)]
    link_num = 0
    
    for k in range(group_number):
        tree_map = [[0 for j in range(switches)] for i in range(switches)] 
        inter_node, leaf_node = seperate_node_type(k, member_num, switches)
        node_son_remain = [max_node_sons for i in range(member_num)]
        root = inter_node.pop(0)
        origin_root = root
        while(len(inter_node) > 0):
            son = inter_node.pop(0)
            port_pair = (remain_ports[root].pop(0), remain_ports[son].pop(0))
            # print(root, son, port_pair[0], port_pair[1])
            # print(son, root, port_pair[1], port_pair[0])
            link_num += 2
            tree_map[root][son] += 1
            tree_map[son][root] += 1
            node_son_remain[root-k*member_num] -= 1
            if(node_son_remain[root-k*member_num] == 0):
                root = root + 1
        while(len(leaf_node) > 0):
            son = leaf_node.pop(0)
            port_pair = (remain_ports[root].pop(0), remain_ports[son].pop(0))
            # print(root, son, port_pair[0], port_pair[1])
            # print(son, root, port_pair[1], port_pair[0])
            link_num += 2
            tree_map[root][son] += 1
            tree_map[son][root] += 1
            node_son_remain[root-k*member_num] -= 1
            if(node_son_remain[root-k*member_num] == 0):
                root = root + 1
        
        root_path = search_root_path(origin_root, tree_map)
        root_path[origin_root] = [0]
        for src in range(switches):
            for dst in range(switches):
                if(src != dst):
                    root_to_src = root_path[src]
                    root_to_dst = root_path[dst]
                    least_len = len(root_to_src) if(len(root_to_src) <= len(root_to_dst)) else len(root_to_dst)
                    location = 0
                    for i in range(least_len):
                        if(root_to_dst[i] == root_to_src[i]):
                            location = i
                    src_part = root_to_src[location:]
                    if(location >= len(root_to_dst) - 1):
                        dst_part = []
                    else:
                        dst_part = root_to_dst[location+1:]
                    path = [src_part[i] for i in range(len(src_part)-1, -1, -1)] + dst_part
                    route_path[src*switches+dst].append(path)
                        
    return remain_ports, route_path



if __name__ == "__main__":
    switches = 100
    hosts = 24
    ports = 64
    remain_ports, route_path = edst_topo_generate(24, 64, 100)
    print(switches*(switches-1), hosts, switches)
    for src in range(switches):
        for dst in range(switches):
            if(src != dst):
                all_path = route_path[src*switches+dst]
                print(src, dst, len(all_path))
                for path in all_path:
                    print(len(path), end = " ")
                    for node in path:
                        print(node, end = " ")
                    print("")

