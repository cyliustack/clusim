/** $lic$
 * Copyright (C) 2016-2017 by National Taiwan University
 *
 * This file is part of mancos.
 *
 * mancos is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * If you use this software in your research, we request that you reference
 * the mancos paper ("Mancos: Micro-Architecture and Network Co-Simulation",
 * Cheng-Yueh Liu, XXXX-50, Oct 2017) as the
 * source of the simulator in any publications that use this software, and that
 * you send us a citation of your work.
 *
 * mancos is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <libconfig.h++>
#include <limits.h>
#include <map>
#include <omp.h>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-nix-vector-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/packet-sink.h"
#include "ns3/pfifo-fast-queue-disc.h"
#include "ns3/traffic-control-module.h"
// TODO: To compile with the following header files
//#include "ns3/network-module.h"
//#include "ns3/internet-module.h"
//#include "ns3/point-to-point-module.h"
//#include "ns3/point-to-point-layout-module.h"

#include "ppacket.hpp"

using namespace libconfig;
using namespace std;

class Parameter {
public:
    uint32_t window_size;
    uint32_t num_nodes;
};

class PhaseStatistics {
public:
    uint32_t total_acks;
    uint64_t total_payloads;
    uint64_t total_hops;
    uint64_t total_latencies;
    uint64_t avg_queue_length;
};

class NetworkSimulation {
public:
    Parameter parameter;
    uint32_t num_phases;
    unique_ptr<PhaseStatistics[]> phases;
    void init(auto& pcapfile, auto const paramter);
};

void NetworkSimulation::init(auto& pcapfile, auto const parameter_in)
{
    parameter = parameter_in;
    num_phases = (int)((pcapfile.max_wct - pcapfile.min_wct) / parameter.window_size) + 1;
    phases = make_unique<PhaseStatistics[]>(num_phases);
    for (uint32_t i = 0; i < num_phases; i++) {
        phases[i].total_payloads = 0;
    }
}

//int pcap_demo(auto& packets, auto& path);
int parse_config(char* config_file, auto& vec_pcapfile, auto& parameter);
void ns3_simulate(auto& pcapfile, auto const parameter);

int main(int argc, char* argv[])
{
    std::vector<PcapFile> vec_pcapfile;
    Parameter parameter;
    if (argc < 2) {
        printf("Usage : ./clusim default.cfg\n");
        return -1;
    }

    parse_config(argv[1], vec_pcapfile, parameter);

    for (auto& pcapfile : vec_pcapfile) {
        pcapfile.parse_from_file();
    }

    ns3_simulate(vec_pcapfile.at(0), parameter);

    return 0;
}

void ns3_simulate(auto& pcapfile, auto const parameter)
{
    // Phase Begin
    printf("Max. Wall Clock Time: %.6lf\n", pcapfile.max_wct);
    printf("Min. Wall Clock Time: %.6lf\n", pcapfile.min_wct);
    auto const packets = pcapfile.packets;
    NetworkSimulation ns;
    ns.init(pcapfile, parameter);

    for (auto& packet : packets) {
        uint32_t phase_id = (int)((packet.timestamp - pcapfile.min_wct) / ns.parameter.window_size);
        ns.phases[phase_id].total_payloads += packet.payload;
    }
}

//void star(uint32_t* leaf_nodes, double phase_data[][col], double phase_data_a[][col], double* final_real, int* phase_data_a_number, int i, bool* lock, double window_info[][col])
void sim_star(const uint32_t n_leaf_nodes)
{
    ns3::Config::SetDefault("ns3::TcpL4Protocol::SocketType", ns3::StringValue("ns3::TcpNewReno"));
    ns3::Config::SetDefault("ns3::TcpSocket::SegmentSize", ns3::UintegerValue(1448));
    ns3::Config::SetDefault("ns3::TcpSocket::DelAckCount", ns3::UintegerValue(2));
    // LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);
    //double real = 0;
    //double nocputime = 0;

    ns3::NodeContainer nodes;
    nodes.Create((n_leaf_nodes) + 1);
}

int parse_config(char* config_file, auto& vec_pcapfile, auto& parameter)
{
    Config cfg;

    // Read the file. If there is an error, report it and exit.
    try {
        cfg.readFile(config_file);
    } catch (const FileIOException& fioex) {
        std::cerr << "I/O error while reading file." << std::endl;
        return (EXIT_FAILURE);
    } catch (const ParseException& pex) {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                  << " - " << pex.getError() << std::endl;
        return (EXIT_FAILURE);
    }

    // Get the store name.
    try {
        std::string name = cfg.lookup("name");
        std::cout << "Topic: " << name << std::endl
                  << std::endl;
    } catch (const SettingNotFoundException& nfex) {
        cerr << "No 'name' setting in configuration file." << std::endl;
        exit(-1);
    }

    try {
        parameter.window_size = cfg.lookup("window_size");
        std::cout << "window_size: " << parameter.window_size << std::endl
                  << std::endl;
    } catch (const SettingNotFoundException& nfex) {
        cerr << "No 'window_size' setting in configuration file." << std::endl;
        exit(-1);
    }

    try {
        parameter.num_nodes = cfg.lookup("num_nodes");
        std::cout << "num_nodes: " << parameter.num_nodes << std::endl
                  << std::endl;
    } catch (const SettingNotFoundException& nfex) {
        cerr << "No 'num_nodes' setting in configuration file." << std::endl;
        exit(-1);
    }

    const Setting& root = cfg.getRoot();

    // Read tcpdump traces.
    try {
        const Setting& stn_pcapfiles = root["pcapfiles"];
        int count = stn_pcapfiles.getLength();

        std::cout << setw(40) << left << "PcapFile Name"
                  << "  "
                  << setw(40) << left << "Color"
                  << "   "
                  << std::endl;

        for (int i = 0; i < count; ++i) {
            const Setting& stn_pcapfile = stn_pcapfiles[i];
            PcapFile pcapfile;
            if (!(stn_pcapfile.lookupValue("path", pcapfile.path)
                    && stn_pcapfile.lookupValue("color", pcapfile.color))) {
                continue;
            }

            //tcfile.path  = path;
            //tcfile.color = color;
            //char str_tmp[100] = { 0 };
            vec_pcapfile.push_back(pcapfile);
            std::cout << setw(40) << left << pcapfile.path << "  "
                      << setw(40) << left << pcapfile.color << "  "
                      << std::endl;
        }
        std::cout << std::endl;
    } catch (const SettingNotFoundException& nfex) {
        // Ignore.
    }

    return 0;
}
