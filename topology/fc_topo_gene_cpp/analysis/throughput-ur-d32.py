from array import array
import matplotlib as mpl 
mpl.rcParams['pdf.fonttype'] = 42
mpl.rcParams['ps.fonttype'] = 42
mpl.use('Agg')
mpl.rcParams['hatch.linewidth'] = 2

import matplotlib.pyplot as plt 
import numpy as np
import os
import math

def cost_equivalent_clos(num_eps, num_servers, eps_port_count):
  # print(num_eps, num_servers, eps_port_count)
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
      l3_throughput = (eps_port_count - host_per_tor) / host_per_tor
      break
  return max(l2_throughput, l3_throughput)

if __name__ == "__main__":
  to_hosts = 14
  switches = [50, 100, 150, 200, 250, 300, 400, 500]
  routings = ["ksp", "fc_ksp", "ed", "ksp_ununiform"]
  patterns = ["uniform random"]
  throughput_map = {}
  for pattern in patterns:
    throughput_map[pattern] = {}
    for routing in routings:
      throughput_map[pattern][routing] = {}
      for switch in switches:
        throughput_map[pattern][routing][switch] = 0.0
  th_f = open('../data/throughput/throughput_result_ur', mode='r')
  lines = th_f.read().splitlines()
  label = []
  count = 0
  for line in lines:
    data = line.split(' ')
    label.append(data[0])
    for i in range(1, 9):
      throughput_map[patterns[0]][routings[count]][switches[i-1]] = float(data[i])
    count += 1

  
  ksp_throughputs = []
  fc_ksp_throughputs = []
  ed_throughputs = []
  ununiform_ksp10_throughputs = []

  equal_cost_clos = []
  for i in switches:
    throughput = cost_equivalent_clos(i, i * to_hosts, 32)
    equal_cost_clos.append(throughput)
  print(equal_cost_clos)

  pattern = "uniform random"
  for switch in switches:
    ksp_throughputs.append(throughput_map[pattern]["ksp"][switch])
    fc_ksp_throughputs.append(throughput_map[pattern]["fc_ksp"][switch])
    ed_throughputs.append(throughput_map[pattern]["ed"][switch])
    ununiform_ksp10_throughputs.append(throughput_map[pattern]["ksp_ununiform"][switch])
    


  fc_ksp_throughputs = np.array(fc_ksp_throughputs, dtype=float)
  ed_throughputs = np.array(ed_throughputs, dtype=float)
  ksp_throughputs = np.array(ksp_throughputs, dtype=float)
  ununiform_ksp10_throughputs = np.array(ununiform_ksp10_throughputs, dtype=float)

  x = np.arange(len(switches))
  width = 0.2
  fig, ax = plt.subplots(figsize=(6,3))

  ax.plot(x , ksp_throughputs, marker='*', color='r',lw=1, label='KSP')
  ax.plot(x , ed_throughputs, marker='o', color='darkorange',lw=1, label='EDGE DIS')
  ax.plot(x , equal_cost_clos, marker='x', color='mediumpurple', lw=1, label='CLOS')
  ax.plot(x , fc_ksp_throughputs, marker='s', color='c', lw=1, label='FC-KSP')
  ax.plot(x , ununiform_ksp10_throughputs, marker='.', color='black', lw=1, label='KSP-UNUNI')

  ax.set_xticks(x)
  ax.set_xticklabels(np.array(switches) * to_hosts)
  ax.set_yticks([i * 0.4 for i in range(4)])
  ax.set_xlabel("Number of Servers", fontsize=12)
  ax.set_ylabel("Throughput", fontsize=20)
  ax.tick_params(axis='x', labelsize=12) 
  ax.tick_params(axis='y', labelsize=12) 
  
  
  ax.legend(loc='upper right', fontsize='large', ncol=3)
  
  plt.grid(ls='--')
  fig.tight_layout()


  fname = "images/" + os.path.basename(__file__).replace('.py', '.pdf')
  # print(fname)
  plt.savefig(fname, bbox_inches='tight')
