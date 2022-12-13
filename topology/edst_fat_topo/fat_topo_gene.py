from math import *


def search_root_path(root, topo_mat, node_list, layer_infor, layer):
    src_neighbor = topo_mat[root]
    path = []
    root_path = {}
    for i in range(len(src_neighbor)-1):
        root_path[i] = []
    if(layer == len(layer_infor) - 1):
        return root_path
    next_layer = layer + 1
    next_begin_index = 0
    for i in range(next_layer):
        next_begin_index = int(next_begin_index + layer_infor[(i+1)*-1])
    next_layer_num = int(layer_infor[(next_layer+1)*-1])
    next_layer_mem = node_list[next_begin_index:next_begin_index+next_layer_num]
    path.append(root)
    
    for i in range(len(src_neighbor)-1):
        if(src_neighbor[i] == 1):
            if(i in next_layer_mem):
                path.append(i)
                root_path[i].append([k for k in path])
                node_path = search_root_path(i, topo_mat, node_list, layer_infor, next_layer)
                for r_path in root_path[i]:
                    for key in node_path:
                        for l_path in node_path[key]:
                            root_path[key].append(r_path + l_path[1:])
                path.pop()
    return root_path


def span_fat_gene(switch_num, switch_port, host_port,tree_num, downlink_devision, min_port_ratio, filename):

    # Validafy if the parameters are correct
    tree_port = switch_port // tree_num
    if(tree_port*tree_num != switch_port):
        print("The port can't be devided uniformly")
        return

    layer_num = len(downlink_devision) + 2
    if(layer_num < 3):
        print("Layer number is at least 3")
        return
    
    max_node_list = []
    layer_node_infor = []
    layer_total_number = []
    layer_down_link = []
    layer_u = layer_d = 1

    layer_dnext = downlink_devision[0] * tree_port
    layer_node_infor.append(layer_dnext)
    layer_total_number.append(layer_dnext)
    layer_down_link.append(0)
    layer_node_infor.append(tree_port)
    layer_total_number.append(tree_port)
    layer_down_link.append(int(layer_dnext))
    pod_node_number = layer_d*layer_dnext + tree_port*layer_u
    max_node_number = pod_node_number
    max_node_list.append(max_node_number)

    for layer in range(len(downlink_devision)-1):
        layer_u = (tree_port - layer_dnext)*layer_u
        layer_node_infor.append(layer_u*tree_port)
        layer_dnext = downlink_devision[layer+1] * tree_port
        layer_down_link.append(int(layer_dnext))
        for i in range(len(layer_total_number)):
            layer_total_number[i] = layer_total_number[i]*layer_dnext
        max_node_number = pod_node_number*tree_port + tree_port*layer_u
        max_node_list.append(max_node_number)
        layer_total_number.append(tree_port*layer_u)
        pod_node_number = pod_node_number*layer_dnext + tree_port*layer_u

    layer_u = (tree_port - layer_dnext)*layer_u
    layer_node_infor.append(layer_u*tree_port)
    layer_down_link.append(tree_port)
    max_node_number = pod_node_number*tree_port + tree_port*layer_u
    max_node_list.append(max_node_number)
    if(switch_num > max_node_number):
        print("Only %d layers can't satisfied so many switches"%layer_num)
        return

    real_layers = layer_num
    for layer in range(len(max_node_list)-1):
        if((switch_num > max_node_list[layer]) & (switch_num <= max_node_list[layer+1])):
            real_layers = layer + 3
            if(real_layers != layer_num):
                print("Layer number can't satisfy the switch number. Just %d layers are needed"%real_layers)
                return
            break
    
    pod_num = 0
    switch_remain = switch_num
    total_port = 0
    while((switch_remain-pod_node_number)*tree_port > (total_port+layer_u*tree_port)*min_port_ratio):
        pod_num = pod_num + 1
        total_port = total_port + layer_u*tree_port
        switch_remain = switch_remain - pod_node_number
        if(pod_num == tree_port):
            break
    for i in range(len(layer_total_number)):
        layer_total_number[i] = layer_total_number[i]*pod_num

    core_port_pro = 1
    full_pod_num = pod_num
    if(switch_remain*tree_port <= total_port):
        layer_total_number.append(switch_remain)
        core_port_pro = switch_remain*tree_port / total_port
    else:
        sub_layer_link = downlink_devision[len(downlink_devision)-1]*tree_port
        top_layer_node_num = ceil((sub_layer_link*switch_remain+total_port)/(tree_port+sub_layer_link))
        switch_remain = switch_remain - top_layer_node_num
        layer_total_number[-1] = layer_total_number[-1] + switch_remain
        layer_total_number.append(top_layer_node_num)
        pod_num = pod_num + 1
    
    # Generate the topology
    print(layer_total_number)
    file_path = open(filename, "w")
    group_tree_num = switch_num/tree_num
    route_path = [[] for i in range(switch_num*switch_num)]
    total_link = 0
    for tree_label in range(tree_num):
        tree_map = [[0 for j in range(switch_num+1)] for i in range(switch_num+1)] 
        node_list = []
        node_list_cp = []
        port_list = {}
        for i in range(switch_num):
            node_label = i+tree_label*group_tree_num
            if(node_label >= switch_num):
                node_label = node_label - switch_num
            node_list.append(node_label)
            node_list_cp.append(int(node_label))
            port_list[node_label] = [j for j in range(tree_label*tree_port + 1, (tree_label+1)*tree_port + 1)]

        node_num = int(layer_total_number[-1])
        node_down_num = int(layer_total_number[-2])
        node_down_pod_num = int(layer_node_infor[-2])
        link_num = layer_down_link[-1]
        link_down_num = layer_down_link[-2]
        for i in range(node_num):
            node_core = int(node_list[i])
            tree_map[switch_num][node_core] = 1
            tree_map[node_core][switch_num] = 1
        if(core_port_pro == 1):
            # Connect the top layer with the subtop layers when there are enough ports on the top layer
            base_label = 0
            refresh = 0
            link_aware = 0
            for core_label in range(node_num):
                node_up = int(node_list[core_label])
                link_count = 0
                while(link_count < link_num):
                    node_down_index = base_label + node_num + node_down_pod_num*refresh
                    if(node_down_index >= node_num + node_down_num - node_down_pod_num):
                        refresh = 0
                    else:
                        refresh = refresh + 1
                    node_down = int(node_list[node_down_index])
                    if(len(port_list[node_down]) > link_down_num):
                        port_up = int(port_list[node_up].pop(0))
                        port_down = port_list[node_down].pop(0)
                        link_count = link_count + 1
                        link_aware = link_aware + 1
                        total_link = total_link + 2
                        tree_map[node_up][node_down] = 1
                        tree_map[node_down][node_up] = 1
                        file_path.write(str(node_up) + " " + str(node_down) + " " + str(port_up) + " " + str(port_down) + "\n")
                        file_path.write(str(node_down) + " " + str(node_up) + " " + str(port_down) + " " + str(port_up) + "\n")
                    else:
                        if((node_down_index >= node_num + node_down_num - node_down_pod_num) & (node_down_index < node_num + node_down_num)):
                            base_label = base_label + 1
                    if((link_aware >= link_num * node_num) | (link_aware >= (tree_port - link_down_num)*node_down_num)):
                        break
            for core_label in range(node_num):
                node_list.pop(0)
        else:
            # Connect the top layer with the subtop layers when there are not enough ports on the top layer
            base_label = 0
            refresh = 0
            for core_label in range(node_num):
                node_up = int(node_list[core_label])
                link_count = 0
                while(link_count < link_num):
                    node_down_index = base_label + node_num + node_down_pod_num*refresh
                    node_down = int(node_list[node_down_index])
                    port_up = int(port_list[node_up].pop(0))
                    port_down = port_list[node_down].pop(0)
                    link_count = link_count + 1
                    total_link = total_link + 2
                    tree_map[node_up][node_down] = 1
                    tree_map[node_down][node_up] = 1
                    file_path.write(str(node_up) + " " + str(node_down) + " " + str(port_up) + " " + str(port_down) + "\n")
                    file_path.write(str(node_down) + " " + str(node_up) + " " + str(port_down) + " " + str(port_up) + "\n")
                    if(refresh == full_pod_num-1):
                        refresh = 0
                        if(base_label == node_down_pod_num - 1):
                            base_label = 0
                        else:
                            base_label = base_label + 1
                    else:
                        refresh = refresh + 1
            for core_label in range(node_num):
                node_list.pop(0)
                    
        for layer in range(len(layer_total_number)-2):
            node_num = int(layer_total_number[-2-layer])
            node_down_num = int(layer_total_number[-3-layer])
            node_pod_num = int(layer_node_infor[-2-layer])
            node_down_pod_num = int(layer_node_infor[-3-layer])
            link_num = layer_down_link[-2-layer]
            link_down_num = layer_down_link[-3-layer]
            if(layer != len(layer_total_number)-3):
                base_label = 0
                assign_num = node_num
                if(layer == 0):
                    assign_num = node_pod_num*full_pod_num
                for node_label in range(assign_num):
                    node_up = int(node_list[node_label])
                    link_count = 0
                    while(link_count < link_num):
                        node_down_index = base_label + node_num + node_down_pod_num*link_count
                        node_down = int(node_list[node_down_index])
                        if(len(port_list[node_down]) > link_down_num):
                            port_up = int(port_list[node_up].pop(0))
                            port_down = port_list[node_down].pop(0)
                            link_count = link_count + 1
                            total_link = total_link + 2
                            tree_map[node_up][node_down] = 1
                            tree_map[node_down][node_up] = 1
                            file_path.write(str(node_up) + " " + str(node_down) + " " + str(port_up) + " " + str(port_down) + "\n")
                            file_path.write(str(node_down) + " " + str(node_up) + " " + str(port_down) + " " + str(port_up) + "\n")
                        else:
                            base_label = base_label + 1
            else:
                base_label = 0
                assign_num = node_num
                if(layer == 0):
                    assign_num = node_pod_num*full_pod_num
                for node_label in range(assign_num):
                    node_up = int(node_list[node_label])
                    link_count = 0
                    while(link_count < link_num):
                        node_down_index = base_label + node_num + link_count
                        node_down = int(node_list[node_down_index])
                        if(len(port_list[node_down]) > link_down_num):
                            port_up = int(port_list[node_up].pop(0))
                            port_down = port_list[node_down].pop(0)
                            link_count = link_count + 1
                            total_link = total_link + 2
                            tree_map[node_up][node_down] = 1
                            tree_map[node_down][node_up] = 1
                            file_path.write(str(node_up) + " " + str(node_down) + " " + str(port_up) + " " + str(port_down) + "\n")
                            file_path.write(str(node_down) + " " + str(node_up) + " " + str(port_down) + " " + str(port_up) + "\n")
                        else:
                            base_label = base_label + 1
            for core_label in range(node_num):
                node_list.pop(0)
        root_path = search_root_path(switch_num, tree_map, node_list_cp, layer_total_number, -1)
        for src in range(switch_num):
            for dst in range(switch_num):
                root_to_src = root_path[src]
                root_to_dst = root_path[dst]
                for src_path in root_to_src:
                    for dst_path in root_to_dst:
                        least_len = len(src_path) if(len(src_path) <= len(dst_path)) else len(dst_path)
                        location = 0
                        for loc in range(least_len):
                            if(src_path[loc] == dst_path[loc]):
                                location = loc
                        if(location == 0):
                            continue
                        else:
                            src_part = src_path[location:]
                            if(location >= len(dst_path) - 1):
                                dst_part = []
                            else:
                                dst_part = dst_path[location+1:]
                            sdpath = [src_part[i] for i in range(len(src_part)-1, -1, -1)] + dst_part
                            if(sdpath not in route_path[src*switch_num+dst]):
                                route_path[src*switch_num+dst].append(sdpath)
                        
    file_path.close()

    with open(filename, "r+") as f:
        old = f.read()
        f.seek(0)
        f.write(str(host_port) + " " + str(switch_num) + " " +  str(total_link) + " " + str(host_port*switch_num) + "\n")
        f.write(str(3) + " " + str(10) + " " +  str(20) + " " + str(10) + "\n")
        f.write(old)
    return route_path

        


if __name__ == "__main__":
    switches = 200
    hosts = 24
    route_path = span_fat_gene(switches, 40, hosts, 4, [0.5], 0.5, "out")
    print(switches*(switches-1), hosts, switches)
    min_length = 1000
    min_path = []
    src_dst = [0,0]
    list_sd = []
    for src in range(switches):
        for dst in range(switches):
            if(src != dst):
                all_path = route_path[src*switches+dst]
                # print(src, dst, len(all_path))
                if(min_length >= len(all_path)):
                    min_length = len(all_path)
                    min_path = all_path
                    if(min_length == 0):
                        list_sd.append((src, dst))
                    src_dst[0] = src
                    src_dst[1] = dst
                # for path in all_path:
                #     print(len(path), end = " ")
                #     for node in path:
                #         print(node, end = " ")
                #     print("")
    print(min_path, src_dst)
    print(list_sd)
            