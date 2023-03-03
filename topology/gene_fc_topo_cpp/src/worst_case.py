import sys
import networkx as nx
import numpy as np
import string
sys.path.append('src/TUB')
from topo_repo import topology

def get_worst_flow():
    file = open("data/worst_flow_infor/temp_infor", "rb")
    data = file.read(4)
    switches = int.from_bytes(data, byteorder='little', signed=True)
    data = file.read(4)
    hosts = int.from_bytes(data, byteorder='little', signed=True)
    G = nx.Graph()
    
    for src in range(switches):
        for dst in range(switches):
            data = file.read(4)
            has_link = int.from_bytes(data, byteorder='little', signed=True)
            if has_link == 1:
                G.add_edge(src, dst)
    file.close()
    tor_list = [tor_sid for tor_sid in range(switches)]
    demand_list = {}
    for sid in range(switches):
        demand_list[sid] = hosts
    topo = topology.Topology(G, tor_list, demand_list)
    worst_case_tm = np.zeros((switches, switches))
    tm, _ = topo.get_near_worst_case_traffic_matrix()
    for key in tm.keys():
        src = key[0]
        dst = key[1]
        worst_case_tm[src][dst] = tm[key]
    file = open("data/worst_flow_infor/temp_infor_flow", "w")
    for src in range(switches):
        for dst in range(switches):
            file.write(f"{worst_case_tm[src][dst]} ")
    file.close()
    
    

if __name__ == "__main__":
    get_worst_flow()