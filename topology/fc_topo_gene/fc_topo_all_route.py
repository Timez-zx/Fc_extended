import random
import multiprocessing
import math

class Fc_topo_all_route():
    topo_index = []
    all_path = []
    topo_dic = {}

    def __init__(self, switches, hosts, ports, vir_layer_degree, is_random, random_seed):
        self.switches = switches
        self.hosts = hosts
        self.ports = ports
        self.vir_layer_degree = vir_layer_degree
        self.is_random = is_random
        self.random_seed = random_seed
    

    def change_base(self, basic):
        if(basic == 0):
            basic = self.switches - 1
        else:
            basic = basic - 1
        return basic


    def fc_topo_gene(self):
        sw_ports = self.ports - self.hosts
        remain_ports = [[j for j in range(1, sw_ports+1)] for i in range(self.switches)]
        bipart_num = len(self.vir_layer_degree) - 1
        switch_list = [i for i in range(self.switches)]
        initial_sub = 0
        self.topo_index = [[[] for i in range(self.switches)] for j in range(bipart_num)]
        for bipart in range(bipart_num,0,-1):
            degree = self.vir_layer_degree[bipart] - initial_sub
            degrees = [degree for i in range(self.switches)]
            basic = self.switches - 1
            remain_list = []
            if(self.is_random):
                random.seed(self.random_seed)
                random.shuffle(switch_list)
            for sw in range(self.switches):
                src = switch_list[sw]
                self.topo_index[bipart-1][src].append(src)
                for link in range(degree):
                    if(basic == sw):
                        remain_list.append(sw)
                        basic = self.change_base(basic)
                    dst = switch_list[basic]
                    while(degrees[dst] == 0):
                        basic = self.change_base(basic)
                        dst = switch_list[basic]
                    if(len(remain_list) > 0):
                        if(remain_list[0] != sw):
                            dst = switch_list[remain_list[0]]
                            basic = remain_list.pop(0)
                            basic = self.change_base(basic)
                        else:
                            basic = self.change_base(basic)
                    else:
                        basic = self.change_base(basic)
                    degrees[dst] = degrees[dst] - 1
                    self.topo_index[bipart-1][src].append(dst)
                    src_port = remain_ports[src].pop(0)
                    dst_port = remain_ports[dst].pop(0)
            initial_sub = degree


    def search_path(self, root):
        root_path = [[root]]
        bipart_num = len(self.topo_index)
        drop_num = 0
        for bipart in range(bipart_num-1,-1,-1):
            root_num = len(root_path)
            for path_num in range(root_num):
                path = root_path.pop(0)
                layer_root = path[-1]
                tp_index = self.topo_index[bipart][layer_root]
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
                    if(root not in self.topo_dic[path[node_num]].keys()):
                        self.topo_dic[path[node_num]][root] = [i,node_num*(-1)]
                    else:
                        if(i not in self.topo_dic[path[node_num]][root]):
                            self.topo_dic[path[node_num]][root].append(i)
                            self.topo_dic[path[node_num]][root].append(node_num*(-1))
        return root_path


    def route_infor_generate(self):
        for i in range(self.switches):
            self.topo_dic[i] = {}
        for sw in range(self.switches):
            root_path = self.search_path(sw)
            self.all_path.append(root_path)


    def find_route_path(self, src, dst, all_path, topo_dic):
        layers = len(self.vir_layer_degree)
        route_path = []
        if(src in topo_dic[dst].keys()):
            src_path_infor = topo_dic[dst][src]
            for path_index in range(0,len(src_path_infor),2):
                aroute_path = []
                path = all_path[src][src_path_infor[path_index]]
                loca = src_path_infor[path_index+1]*(-1)
                aroute_path.append(path[loca] + layers*10+loca*-10)
                loca -= 1
                while(path[loca] != src):
                    aroute_path.append(path[loca] + layers*10+loca*-10)
                    loca -= 1
                aroute_path.append(path[loca] + layers*10+loca*-10)
                if(aroute_path not in route_path):
                    route_path.append(aroute_path)

        if(dst in topo_dic[src].keys()):
            dst_path_infor = topo_dic[src][dst]
            for path_index in range(0,len(dst_path_infor),2):
                aroute_path = []
                path = all_path[dst][dst_path_infor[path_index]]
                loca = dst_path_infor[path_index+1]*(-1)
                aroute_path.append(path[loca] + layers*10+loca*-10)
                loca -= 1
                while(path[loca] != dst):
                    aroute_path.append(path[loca] + layers*10+loca*-10)
                    loca -= 1
                aroute_path.append(path[loca] + layers*10+loca*-10)
                if(aroute_path not in route_path):
                    route_path.append(aroute_path)

        src_keys = set(topo_dic[src].keys())
        dst_keys = set(topo_dic[dst].keys())
        if(dst in src_keys):
            src_keys.remove(dst)
        if(src in dst_keys):
            dst_keys.remove(src)
        inter_nodes = list(src_keys & dst_keys)

        for inter in inter_nodes:
            src_inter = topo_dic[src][inter]
            dst_inter = topo_dic[dst][inter]
            src_index = set(src_inter[::2])
            dst_index = set(dst_inter[::2])
            src_valid = list(src_index - dst_index)
            dst_valid = list(dst_index - src_index)
            judging_index = list(src_index & dst_index)
            for judge in judging_index:
                src_loca = -1*src_inter[src_inter.index(judge)+1]
                dst_loca = -1*dst_inter[dst_inter.index(judge)+1]
                if(src_loca < dst_loca):
                    src_valid.append(judge)
                elif(src_loca > dst_loca):
                    dst_valid.append(judge)
            src_revalid_path = []
            src_revalid = []
            for src_index in src_valid:
                src_path = all_path[inter][src_index]
                path_temp = []
                for node in src_path:
                    if(node != src):
                        path_temp.append(node)
                        continue
                    else:
                        break
                if(path_temp not in src_revalid_path):
                    src_revalid_path.append(path_temp)
                    src_revalid.append(src_index)
            dst_revalid_path = []
            dst_revalid = []
            for dst_index in dst_valid:
                dst_path = all_path[inter][dst_index]
                path_temp = []
                for node in dst_path:
                    if(node != dst):
                        path_temp.append(node)
                        continue
                    else:
                        break
                if(path_temp not in dst_revalid_path):
                    dst_revalid_path.append(path_temp)
                    dst_revalid.append(dst_index)

            for src_index in src_revalid:
                for dst_index in dst_revalid:
                    src_path = all_path[inter][src_index]
                    dst_path = all_path[inter][dst_index]
                    begin_src = 0
                    begin_dst = 0
                    while(src_path[begin_src] == dst_path[begin_dst]):
                        if(src_path[begin_src] == inter):
                            begin_src += 1
                            begin_dst += 1
                        else:
                            break
                    begin_src = begin_src - 1
                    aroute_src = []
                    aroute_dst = []
                    pass_node = []
                    pass_flag = 0
                    while(src_path[begin_src] != src):
                        aroute_src = [src_path[begin_src] - 10*begin_src+layers*10] + aroute_src
                        pass_node.append(src_path[begin_src])
                        begin_src += 1
                    aroute_src = [src_path[begin_src] - 10*begin_src+layers*10] + aroute_src
                    while(dst_path[begin_dst] != dst):
                        if(dst_path[begin_dst] in pass_node):
                            pass_flag = 1
                            break
                        aroute_dst.append(dst_path[begin_dst]-10*begin_dst+layers*10)
                        begin_dst += 1
                    if(pass_flag):
                        continue
                    aroute_dst.append(dst_path[begin_dst]-10*begin_dst+layers*10)
                    aroute_path = aroute_src+aroute_dst
                    if(aroute_path not in route_path):  
                        route_path.append(aroute_path)
        return route_path


    def route_find_thread(self, pairs, all_path, topo_dict):
        count = 0
        for pair in pairs:
            route_path = self.find_route_path(pair[0], pair[1], all_path, topo_dict)
            print(pair[0], pair[1], len(route_path))
            print(route_path)
            # print(len(route_path))
            # if(count%1000 == 0):
            #     print(count/len(pairs),len(route_path))
            # count += 1


    def route_gene(self, thread_num):
        pair_num = self.switches*(self.switches-1)/2
        average_num = int(math.ceil(pair_num/thread_num))
        pair_list = [[] for i in range(thread_num)]
        count = 0
        allo_index = 0
        for i in range(self.switches):
            for j in range(i+1, self.switches):
                pair_list[allo_index].append((i,j))
                if(count < average_num-1):
                    count += 1
                else:
                    count = 0
                    allo_index += 1
        thread_list = []
        for i in range(thread_num):
            thread = multiprocessing.Process(target = self.route_find_thread, args = (pair_list[i], self.all_path, self.topo_dic))
            thread.start()
            thread_list.append(thread)
        for th in thread_list:
            th.join()



if __name__ == "__main__":
    switches = 5
    hosts = 24
    ports = 36
    vir_layer_degree = [2,4,4,2]
    is_random = 1
    random_seed = 3
    fc_demo = Fc_topo_all_route(switches, hosts, ports, vir_layer_degree, is_random, random_seed)
    fc_demo.fc_topo_gene()
    fc_demo.route_infor_generate()
    fc_demo.route_gene(1)