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
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"

#include "ppacket.hpp"

//using namespace ns3;
using namespace libconfig;
using namespace std;

uint32_t checkTimes[5000];
double avgQueueSize[5000];
double maxQueueSize[5000];
double tmpQueueSize[5000];
double queueTime[5000];
uint32_t qSize[5000];

uint32_t checkTimes_s[5000];
double avgQueueSize_s[5000];
double maxQueueSize_s[5000];
double tmpQueueSize_s[5000];
double queueTime_s[5000];
uint32_t qSize_s[5000];

double realQueuesize[5000];
double realQueuesize_s[5000];
double realCheckTimes[5000];
double realCheckTimes_s[5000];

double real_timesForward = 0;
double real_rxPackets = 0;

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

char * toString(int a,int b, int c, int d){

	int first = a;
	int second = b;
	int third = c;
	int fourth = d;

	char *address =  new char[30];
	char firstOctet[30], secondOctet[30], thirdOctet[30], fourthOctet[30];	
	//address = firstOctet.secondOctet.thirdOctet.fourthOctet;

	bzero(address,30);

	snprintf(firstOctet,10,"%d",first);
	strcat(firstOctet,".");
	snprintf(secondOctet,10,"%d",second);
	strcat(secondOctet,".");
	snprintf(thirdOctet,10,"%d",third);
	strcat(thirdOctet,".");
	snprintf(fourthOctet,10,"%d",fourth);

	strcat(thirdOctet,fourthOctet);
	strcat(secondOctet,thirdOctet);
	strcat(firstOctet,secondOctet);
	strcat(address,firstOctet);

	return address;
}

void
CheckQueueSize (ns3::Ptr<ns3::QueueDisc> queue,double *queuel,double *queuem,int j,double *queuet)
{
  double timestamp=0.001;
  qSize[j] = ns3::StaticCast<ns3::PfifoFastQueueDisc> (queue)->GetInternalQueue(0)->GetNPackets ();
  qSize[j]+= ns3::StaticCast<ns3::PfifoFastQueueDisc> (queue)->GetInternalQueue(1)->GetNPackets ();
  qSize[j]+= ns3::StaticCast<ns3::PfifoFastQueueDisc> (queue)->GetInternalQueue(2)->GetNPackets ();

  if(qSize[j]!=0){
   checkTimes[j]++;
   realCheckTimes[j]++;
   queueTime[j]+=timestamp;  
   tmpQueueSize[j]+= qSize[j];
   realQueuesize[j]+= qSize[j];
   avgQueueSize[j]=tmpQueueSize[j]/checkTimes[j];
   if(qSize[j]>maxQueueSize[j]){
      maxQueueSize[j]=qSize[j];
     }
  } 

  // check queue size every 1/100 of a second
  queuel[j]=avgQueueSize[j];
  queuem[j]=maxQueueSize[j];
  queuet[j]=queueTime[j];
  ns3::Simulator::Schedule (ns3::Seconds (timestamp), &CheckQueueSize,queue,queuel,queuem,j,queuet);
 //cout << Simulator::Now ().GetSeconds () <<" j:"<<j<<" checkTimes"<<checkTimes[j] << endl;
 //cout << Simulator::Now ().GetSeconds () <<" j:"<<j<<" qSize:" <<qSize[j] << endl;
 //cout << Simulator::Now ().GetSeconds () << "ReceiveBytes:" << qSize2 << endl;
 //cout << Simulator::Now ().GetSeconds () <<" j:"<<j<<" avgQueueSize:"<< avgQueueSize[j] << endl;
 
}

void
CheckQueueSize_s (ns3::Ptr<ns3::QueueDisc> queue,double *queuel,double *queuem,int j,double *queuet)
{
  double timestamp_s=0.001;
  qSize_s[j] = ns3::StaticCast<ns3::PfifoFastQueueDisc> (queue)->GetInternalQueue(0)->GetNPackets ();
  qSize_s[j]+= ns3::StaticCast<ns3::PfifoFastQueueDisc> (queue)->GetInternalQueue(1)->GetNPackets ();
  qSize_s[j]+= ns3::StaticCast<ns3::PfifoFastQueueDisc> (queue)->GetInternalQueue(2)->GetNPackets ();

  if(qSize_s[j]!=0){
   checkTimes_s[j]++;
   realCheckTimes_s[j]++;
   queueTime_s[j]+=timestamp_s;  
   tmpQueueSize_s[j]+= qSize_s[j];
   realQueuesize_s[j]+= qSize_s[j];
   avgQueueSize_s[j]=tmpQueueSize_s[j]/checkTimes_s[j];
   if(qSize_s[j]>maxQueueSize_s[j]){
      maxQueueSize_s[j]=qSize_s[j];
     }
  } 

  // check queue size every 1/100 of a second
  queuel[j]=avgQueueSize_s[j];
  queuem[j]=maxQueueSize_s[j];
  queuet[j]=queueTime_s[j];
  ns3::Simulator::Schedule (ns3::Seconds (timestamp_s), &CheckQueueSize_s,queue,queuel,queuem,j,queuet);
 //cout << Simulator::Now ().GetSeconds () <<" j:"<<j<<" checkTimes"<<checkTimes[j] << endl;
 //cout << Simulator::Now ().GetSeconds () <<" j:"<<j<<" qSize:" <<qSize[j] << endl;
 //cout << Simulator::Now ().GetSeconds () << "ReceiveBytes:" << qSize2 << endl;
 //cout << Simulator::Now ().GetSeconds () <<" j:"<<j<<" avgQueueSize:"<< avgQueueSize[j] << endl;
 
}

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
void ns3_simulate(char* topology, auto& pcapfile, auto const parameter);
void star (uint32_t *leaf_nodes,double **ptr2_phase_data,double **ptr2_phase_data_a,double* final_real,int* phase_data_a_number,int i,bool *lock,double **ptr2_window_info);
void fat_tree (uint32_t *leaf_nodes,double **ptr2_phase_data,double **ptr2_phase_data_a,double* final_real,int* phase_data_a_number,int i,bool *lock,double **ptr2_window_info,double **ptr2_switch);
void DD_tour (uint32_t *leaf_nodes,double **ptr2_phase_data,double **ptr2_phase_data_a,double* final_real,int* phase_data_a_number,int i,bool *lock,double **ptr2_window_info,double **ptr2_switch);

int main(int argc, char* argv[])
{
    vector<PcapFile> vec_pcapfile;
    Parameter parameter;
    if (argc < 3) {
        printf("Usage : ./clusim default.cfg -topology(-s:star -f:fattree )\n");
        return -1;
    }

    parse_config(argv[1], vec_pcapfile, parameter);

    for (auto& pcapfile : vec_pcapfile) {
        pcapfile.parse_from_file();
    }

    ns3_simulate(argv[2],vec_pcapfile.at(0), parameter);

    return 0;
}

void ns3_simulate(char* topology, auto& pcapfile, auto const parameter)
{
    // Phase Begin
    printf("Max. Wall Clock Time: %.6lf\n", pcapfile.max_wct);
    printf("Max. Wall Clock Time: %.6lf\n", pcapfile.max_wct);
    printf("Min. Wall Clock Time: %.6lf\n", pcapfile.min_wct);
    auto const packets = pcapfile.packets;
    NetworkSimulation ns;
    ns.init(pcapfile, parameter);
    uint32_t max = pcapfile.max_wct;
    uint32_t leaf_nodes = 4;//star use 4 nodes fattree and 3d-tours use 64nodes
    uint32_t min = pcapfile.min_wct;
    uint64_t total_payloads=0;
    uint32_t phase_size=400;
    uint32_t number_phase = (max-min)/phase_size+1;
    double **ptr2_phase_data = NULL;
    double **ptr2_window_info = NULL;
    double **ptr2_phase_data_a= NULL;
    double **ptr2_switch= NULL;

    ptr2_phase_data = new double*[1000];
    ptr2_window_info = new double*[1000];
    ptr2_phase_data_a = new double*[1000];

    ptr2_switch = new double*[1000];


    for(int i =0; i<1000;i++){

    	ptr2_phase_data[i]= new double[50000];
    	ptr2_window_info[i]= new double[50000];
    	ptr2_phase_data_a[i]= new double[50000];
    }

    for(int i =0; i<1000;i++){
  	ptr2_switch[i] = new double[100]; 
    }


    for(int i =0; i<1000;i++){
      for(int j =0;j<100;j++){
    	ptr2_switch[i][j] = 0; 
      }
    }

    for(int i =0; i<1000;i++){
        for(int j=0;j<50000;j++){
    		ptr2_phase_data[i][j]= 0;
    		ptr2_window_info[i][j]= 0;
    		ptr2_phase_data_a[i][j]= 0;
        }
    }

    for(int i = 0;i<number_phase;i++){
      	ptr2_phase_data[i][0]=min;
      	if(min+phase_size<max){min+=phase_size;}
      	else{min = max;}
      	ptr2_phase_data[i][1]=min;
      	min+=1;
     }


   int nu =0;
   int mark =0;
   double ack=0;
   double size_normal=0;
   double size_small=0;
   double normal_size=0;
   double small_size=0;
   double total_data_size=0;


   for (auto& packet : packets){
       // find a the event's local phase
         for(int j = 0;j<number_phase;j++)
           {  
             if((packet.timestamp<ptr2_phase_data[j][1])&&(packet.timestamp>=ptr2_phase_data[j][0])){
              mark=j;
              break;
            }
         }
       // save the payload in the phase data
         if(packet.payload!=66){
            ptr2_phase_data[mark][(2+leaf_nodes*(packet.nid_src-2)+(packet.nid_dst-2))]+=packet.payload-42;
            mark=0;
            total_data_size+=packet.payload;
            if(packet.payload==66){
              ack++;
            }
            if(packet.payload<1514){
              size_small++;
              small_size+=packet.payload;
            }
            if(packet.payload==1514){
              size_normal++;
              normal_size+=packet.payload;
            }
         }
 }


double phase_total =0;


// window's total payload
for(uint64_t i =0;i<number_phase;i++){
    for(int j =0;j<leaf_nodes*leaf_nodes;j++)
        {
          phase_total+= ptr2_phase_data[i][2+j];
          //cout<<ptr2_phase_data[i][2+j]<<endl;
    	}
    
    cout<<"phase_total: "<<phase_total<<endl;
    cout<<"＠1＠"<<endl;
    phase_total=0;
   }
total_data_size=total_data_size/1048576;
cout<<"total size:"<<total_data_size<<"MB"<<endl;
total_data_size=total_data_size/ack;
cout<<"message size:"<<total_data_size<<"MB"<<endl;
cout<<"ack number: "<<ack<<endl;
cout<<"normal number: "<<size_normal<<endl;
cout<<"small number: "<<size_small<<endl;
cout<<"normal size: "<<normal_size<<endl;
cout<<"small size: "<<small_size<<endl;
cout<<"number_phase: "<<number_phase<<endl;


int phase_data_a_number = 0;
double cos=0;
double w1 =0;
double w2 =0;
double compare =0;
double NoCpuTime =0;
double congestion_time=0;

double switch_congestion_time =0;
double avg_queue_length=0;
double avg_node_queue_length =0;
double avg_switch_queue_length=0;

double AvgHops =0;
double each_queue_congestion_time[2*leaf_nodes+1];
double each_queue_avg_queue_length[2*leaf_nodes+1];
double each_switch_queue_congestion_time[100];
double each_switch_queue_avg_queue_length[100];

for(int i =0;i<2*leaf_nodes;i++){
	each_queue_congestion_time[i]=0;
	each_queue_avg_queue_length[i]=0;

}
for(int i =0;i<100;i++){
	each_switch_queue_congestion_time[i]=0;
	each_switch_queue_avg_queue_length[i]=0;

}
double final_real=0 ;
bool   lock = true; 
bool   dcn = true;


      for(uint64_t i=0; i<number_phase;i++){
            cos=0;
            w1=0;
            w2=0;
            for(int j = 0 ;j<phase_data_a_number/*0*/;j++){
             //compute the cos 
                     
                for(int p =2;p<leaf_nodes*leaf_nodes+2;p++){
                      cos+=(double)(ptr2_phase_data_a[j][p]*ptr2_phase_data[i][p]);
                      w1+=pow(ptr2_phase_data_a[j][p],2);
                      w2+=pow(ptr2_phase_data[i][p],2);
                }
       
                w1=sqrt(w1);
                w2=sqrt(w2);
                if((w1*w2)==0||cos==0)
                {
                   cos=0; 
                }
                else
                {
                   cos =cos/(w1*w2);
                }
                
                cout<<"cos:"<<cos<<endl;
                if(w1==0||w2==0)
                {
                  compare=-1;
                }else
                {
                  compare=w1/w2;
                } 
                cout<<"compare:"<<compare<<endl;
               
             //check the RMS < ?  //ori cos>1.9&&compare>0.8&&compare<1.2 
             if(cos>2.4&&compare>0.5&&compare<1.5){   
                final_real+=ptr2_phase_data_a[j][2+leaf_nodes*leaf_nodes];    
                ptr2_phase_data[i][2+leaf_nodes*leaf_nodes]=ptr2_phase_data_a[j][2+leaf_nodes*leaf_nodes];
                ptr2_phase_data[i][3+leaf_nodes*leaf_nodes]=ptr2_phase_data_a[j][3+leaf_nodes*leaf_nodes];
                ptr2_phase_data[i][4+leaf_nodes*leaf_nodes]=ptr2_phase_data_a[j][4+leaf_nodes*leaf_nodes];
                ptr2_phase_data[i][5+leaf_nodes*leaf_nodes]=ptr2_phase_data_a[j][5+leaf_nodes*leaf_nodes];
                ptr2_phase_data[i][6+leaf_nodes*leaf_nodes]=ptr2_phase_data_a[j][6+leaf_nodes*leaf_nodes];
                ptr2_phase_data[i][7+leaf_nodes*leaf_nodes]=ptr2_phase_data_a[j][7+leaf_nodes*leaf_nodes];
                 

                for(int k =0;k<(2*2*leaf_nodes);k++){
                  ptr2_window_info[i][k]=ptr2_window_info[j][k];
                }      

                /*if( strcmp(argv[1],"-f") == 0 ){
		  for(int k=0;k<100;k++){
                     ptr2_switch[i][k]=ptr2_switch[j][k];
                  }
                }else if(strcmp(argv[1],"-d") == 0){
		  for(int k=0;k<100;k++){
                     ptr2_switch[i][k]=ptr2_switch[j][k];
                  }
                }*/

                lock=true;
                break;
            //else simulate
              }else{ 
                 lock=false;
               }
             }
         //lock=false; //no compare    
         cout<<"@@@@@@@@@"<<endl;          
         cout<<"lock:"<<lock<<endl;          

        if(lock == false||i==0){
          for(int j =0;j<1000;j++){
             avgQueueSize[j] = 0;
             maxQueueSize[j] = 0;
             tmpQueueSize[j] = 0;
             checkTimes[j] = 0;
             queueTime[j] = 0;
             avgQueueSize_s[j] = 0;
             maxQueueSize_s[j] = 0;
             tmpQueueSize_s[j] = 0;
             checkTimes_s[j] = 0;
             queueTime_s[j] = 0;
        }
 
//NS3 start    
        //star (&leaf_nodes,ptr2_phase_data,ptr2_phase_data_a,&final_real,&phase_data_a_number,i,&lock,ptr2_window_info);

           if( strcmp(topology,"-s") == 0 ){
                cout<<"star"<<endl;
		star (&leaf_nodes,ptr2_phase_data,ptr2_phase_data_a,&final_real,&phase_data_a_number,i,&lock,ptr2_window_info);
	   }else if(strcmp(topology,"-f") == 0 ){
                cout<<"fat-tree"<<endl;
         	fat_tree(&leaf_nodes,ptr2_phase_data,ptr2_phase_data_a,&final_real,&phase_data_a_number,i,&lock,ptr2_window_info,ptr2_switch);
           }else if(strcmp(topology,"-d") == 0 ){
                cout<<"3D-torus"<<endl;
           	DD_tour(&leaf_nodes,ptr2_phase_data,ptr2_phase_data_a,&final_real,&phase_data_a_number,i,&lock,ptr2_window_info,ptr2_switch);
           }
           //leaf_nodes=64;   
           //tree (&leaf_nodes,phase_data,phase_data_a,&final_real,&phase_data_a_number,i,&lock);
           //point2point(&leaf_nodes,phase_data,phase_data_a,&final_real,&phase_data_a_number,i,&lock);
            //
           //

          } 
       }

for(uint64_t i =0;i<number_phase;i++){
        cout<<"windows "<<i<<":"<<endl;
      /*for(int j =0;j<leaf_nodes*leaf_nodes;j++){
    	cout<<"phase["<<i<<"]["<<j<<"]:"<<ptr2_phase_data[i][2+j]<<endl;
        }*/

        cout<<"avg hops: "<<ptr2_phase_data[i][leaf_nodes*leaf_nodes+3]<<endl;
        cout<<"avg queue length: "<<ptr2_phase_data[i][leaf_nodes*leaf_nodes+4]<<endl;
        cout<<"max queue length: "<<ptr2_phase_data[i][leaf_nodes*leaf_nodes+5]<<endl;  
        cout<<"congestion time(s): "<<ptr2_phase_data[i][leaf_nodes*leaf_nodes+6]<<endl; 
        NoCpuTime+=ptr2_phase_data[i][leaf_nodes*leaf_nodes+7];
        congestion_time+=ptr2_phase_data[i][leaf_nodes*leaf_nodes+6];
        AvgHops+=ptr2_phase_data[i][leaf_nodes*leaf_nodes+3];
        cout<<"-----------------------------------------------"<<endl;

        for(int j=0;j<2*leaf_nodes;j++){
           each_queue_avg_queue_length[j]+=ptr2_window_info[i][j];
           each_queue_congestion_time[j]+=ptr2_window_info[i][2*leaf_nodes+j];
        }

       if( strcmp(topology,"-f") == 0 ){
         for(int j=0;j<16;j++){
           each_switch_queue_avg_queue_length[j]+=ptr2_switch[i][j];
           each_switch_queue_congestion_time[j]+=ptr2_switch[i][16+j];   
           //cout<<"phase["<<i<<"]["<<j<<"]: congestion time"<<ptr2_switch[i][16+j]<<endl;
           switch_congestion_time+=ptr2_switch[i][16+j];
        }
       }else if(strcmp(topology,"-d") == 0){
         for(int j=0;j<48;j++){
           each_switch_queue_avg_queue_length[j]+=ptr2_switch[i][j];
           each_switch_queue_congestion_time[j]+=ptr2_switch[i][48+j];
           switch_congestion_time+=ptr2_switch[i][48+j];
         }    
       }
}


        cout<<"@@@@@  final result  @@@@@"<<endl;

        for(int j=0;j<2*leaf_nodes;j++){
          each_queue_avg_queue_length[j]=each_queue_avg_queue_length[j]/number_phase;
          avg_node_queue_length+=each_queue_avg_queue_length[j];
         
          if(realCheckTimes[j]!=0){
            realQueuesize[j]=realQueuesize[j]/realCheckTimes[j];
          }else{
            realQueuesize[j]=0;
          }
          
          cout<<"QUEUE "<<j<<" Real avg_queue_length :"<<realQueuesize[j]<<endl; 
         // cout<<"QUEUE "<<j<<" Real realCheckTimes :"<<realCheckTimes[j]<<endl; 
         // cout<<"QUEUE "<<j<<" avg_queue_length :"<<each_queue_avg_queue_length[j]<<endl;
          cout<<"QUEUE "<<j<<" queue_congestion_time :"<<each_queue_congestion_time[j]<<endl;
        }

 
       if( strcmp(topology,"-f") == 0 ){
        for(int j=0;j<16;j++){
          each_switch_queue_avg_queue_length[j]=each_switch_queue_avg_queue_length[j]/number_phase;
	  avg_switch_queue_length+=each_switch_queue_avg_queue_length[j];
         
          if(realCheckTimes_s[j]!=0){ 
            realQueuesize_s[j]=realQueuesize_s[j]/realCheckTimes_s[j];
          }else{
            realQueuesize_s[j]=0;
          }

          cout<<"Switch QUEUE "<<j<<" Real avg_queue_length :"<< realQueuesize_s[j] <<endl;
         // cout<<"Switch QUEUE "<<j<<" Real realCheckTimes :"<<realCheckTimes_s[j]<<endl;
         // cout<<"Switch QUEUE "<<j<<" avg_queue_length :"<<each_switch_queue_avg_queue_length[j]<<endl;
          cout<<"Switch QUEUE "<<j<<" queue_congestion_time :"<<each_switch_queue_congestion_time[j]<<endl;
        }
       }else if(strcmp(topology,"-d") == 0){
        for(int j=0;j<48;j++){
          each_switch_queue_avg_queue_length[j]=each_switch_queue_avg_queue_length[j]/number_phase;
	  avg_switch_queue_length+=each_switch_queue_avg_queue_length[j];

          if(realCheckTimes_s[j]!=0){
            realQueuesize_s[j]=realQueuesize_s[j]/realCheckTimes_s[j];
          }else{
            realQueuesize_s[j]=0;
          }

          cout<<"Switch QUEUE "<<j<<" Real avg_queue_length :"<<realQueuesize_s[j]<<endl;
         // cout<<"Switch QUEUE "<<j<<" Real realCheckTimes :"<<realCheckTimes_s[j]<<endl;
         // cout<<"Switch QUEUE "<<j<<" avg_queue_length :"<<each_switch_queue_avg_queue_length[j]<<endl;
          cout<<"Switch QUEUE "<<j<<" queue_congestion_time :"<<each_switch_queue_congestion_time[j]<<endl;
        } 
       }
      
       
     
       if( strcmp(topology,"-f") == 0 ){
         avg_queue_length+=avg_switch_queue_length+avg_node_queue_length;
	 avg_switch_queue_length=avg_switch_queue_length/16;
         avg_queue_length=avg_queue_length/144;
       }else if(strcmp(topology,"-d") == 0){
         avg_queue_length+=avg_switch_queue_length+avg_node_queue_length;
	 avg_switch_queue_length=avg_switch_queue_length/48;
         avg_queue_length=avg_queue_length/176;
       }

        avg_node_queue_length=avg_node_queue_length/128;

        AvgHops=AvgHops/number_phase;
        double final_final_time = final_real/1000000000; 
        double final_real_AvgHops = real_timesForward/real_rxPackets +1;  
        cout<<"Real Avg hops: "<<final_real_AvgHops<<endl;
        //cout<<"Avg hops: "<<AvgHops<<endl;
        cout<<"number of windos: "<<number_phase<<endl;
        cout<<"Avg node queue length: "<<avg_node_queue_length<<endl;
        cout<<"Avg Switch queue length: "<<avg_switch_queue_length<<endl;
        cout<<"Total avg queue length: "<<avg_queue_length<<endl;
        cout<<"Switch Congestion time: "<<switch_congestion_time<<endl;
        cout<<"Node Congestion time: "<<congestion_time<<endl;
        congestion_time+=switch_congestion_time;
        cout<<"Total Congestion time: "<<congestion_time<<endl;
        cout<<"total lantency: "<<final_real<<endl; 
        cout<<"total lantency: "<<final_final_time<<endl;
        cout<<"number of phase: "<<phase_data_a_number<<endl;
        cout<<"No CPU TIME: "<<NoCpuTime<<endl;   
   
      NS_LOG_INFO ("Done.");

//NS3-End
  for(int i=0;i<1000;i++){

    delete ptr2_phase_data[i];
    delete ptr2_window_info[i];
    delete ptr2_phase_data_a[i];
    delete ptr2_switch[i];


  }





  delete ptr2_phase_data;
  delete ptr2_window_info;
  delete ptr2_phase_data_a;
  delete ptr2_switch;

 

}

//void star(uint32_t* leaf_nodes, double phase_data[][col], double phase_data_a[][col], double* final_real, int* phase_data_a_number, int i, bool* lock, double window_info[][col])
void star (uint32_t *leaf_nodes,double **ptr2_phase_data,double **ptr2_phase_data_a,double* final_real,int* phase_data_a_number,int i,bool *lock,double **ptr2_window_info){
  ns3::Config::SetDefault ("ns3::TcpL4Protocol::SocketType", ns3::StringValue ("ns3::TcpNewReno"));
  ns3::Config::SetDefault ("ns3::TcpSocket::SegmentSize", ns3::UintegerValue (1472));
  ns3::Config::SetDefault ("ns3::TcpSocket::DelAckCount", ns3::UintegerValue (2));
  //LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);
  double real =0;
  double nocputime =0;
          
  ns3::NodeContainer nodes;
  nodes.Create((*leaf_nodes)+1);

  ns3::NodeContainer n[(*leaf_nodes)];

  for(int j =0;j<(*leaf_nodes);j++){        
    n[j]=ns3::NodeContainer (nodes.Get(j+1),nodes.Get(0)); 
  }
  
  ns3::PointToPointHelper pointToPoint;
  ns3::NetDeviceContainer dev[(*leaf_nodes)];
  pointToPoint.SetDeviceAttribute ("DataRate", ns3::StringValue ("1Gbps"));
  pointToPoint.SetDeviceAttribute ("InterframeGap", ns3::StringValue("96ns"));
  pointToPoint.SetDeviceAttribute ("Mtu", ns3::UintegerValue (1526));
  pointToPoint.SetChannelAttribute ("Delay", ns3::StringValue ("0.0ns"));
  pointToPoint.SetQueue ("ns3::DropTailQueue");
  
  for(int j = 0;j<(*leaf_nodes);j++){
        dev[j]=pointToPoint.Install(n[j]);
  }
 
  // Ipv4NixVectorHelper nixRouting;
  ns3::InternetStackHelper internet;
  // stack.SetRoutingHelper (nixRouting);
  internet.Install(nodes);

  ns3::TrafficControlHelper tch;
  uint16_t handle = tch.SetRootQueueDisc ("ns3::PfifoFastQueueDisc");
  tch.AddInternalQueues (handle, 3, "ns3::DropTailQueue","MaxPackets", ns3::UintegerValue (1000));

  ns3::QueueDiscContainer qdiscs[(*leaf_nodes)];
  for(int j=0 ; j<(*leaf_nodes);j++){
         qdiscs[j]=tch.Install(dev[j]);
      }

  ns3::Ipv4AddressHelper address;   
  ns3::Ipv4InterfaceContainer ii[(*leaf_nodes)];
  char * base;
  for(int j = 0;j<(*leaf_nodes);j++){
        base = toString(0,0,j,0); 
        address.SetBase(base,"255.255.255.0");
        ii[j]=address.Assign(dev[j]);
     }    
  uint16_t port = 50000;
            

  ns3::ApplicationContainer hubApp[(*leaf_nodes)+1];
  ns3::Ptr<ns3::PacketSink> sink[(*leaf_nodes)+1]; 
           
  for(int j =0;j<(*leaf_nodes);j++){
        ns3::Address hubLocalAddress (ns3::InetSocketAddress (ns3::Ipv4Address::GetAny(), port));
        ns3::PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", hubLocalAddress);
        hubApp[j] = packetSinkHelper.Install (nodes.Get(j+1));
        hubApp[j].Start (ns3::Seconds (0.0));
        hubApp[j].Stop (ns3::Seconds (200.0));
        sink[j]=ns3::DynamicCast<ns3::PacketSink> (hubApp[j].Get(0)); 
     }

  ns3::ApplicationContainer spokeApps[16];

  for(uint64_t j = 0;j<(*leaf_nodes)*(*leaf_nodes);j++){
      if(ptr2_phase_data[i][j+2]==0){
         cout<<"j:"<<j<<endl;
               continue;
        }
      ns3::BulkSendHelper bsHelper ("ns3::TcpSocketFactory" , ns3::Ipv4Address::GetAny());
      bsHelper.SetAttribute("SendSize",ns3::UintegerValue(1472));
      uint64_t maxby = (uint64_t)ptr2_phase_data[i][j+2]; 
      bsHelper.SetAttribute ("MaxBytes",ns3::UintegerValue(maxby));
      ns3::AddressValue remoteAddress (ns3::InetSocketAddress (ii[j%4].GetAddress (0), port));
      bsHelper.SetAttribute ("Remote", remoteAddress);
            
      spokeApps[j].Add (bsHelper.Install (nodes.Get(j/4+1)));
      spokeApps[j].Start (ns3::Seconds (0.0));
      spokeApps[j].Stop (ns3::Seconds (200.0));
           
      }                  
       
  ns3::FlowMonitorHelper flowmon;
  ns3::Ptr<ns3::FlowMonitor> monitor = flowmon.InstallAll();


  ns3::Ptr<ns3::QueueDisc> queue[2*(*leaf_nodes)];
  double *queuel = NULL;
  queuel = new double[2*(*leaf_nodes)+1]; 
  double *queuem = NULL;
  queuem = new double[2*(*leaf_nodes)+1];
  double *queuet = NULL;
  queuet = new double[2*(*leaf_nodes)+1];
  
  for(int j=0;j<2*(*leaf_nodes);j++){
      queuel[j]=0;
      queuem[j]=0;
      queuet[j]=0;
  }

  for(int j =0 ;j<2*(*leaf_nodes);j++){
      queue[j]=qdiscs[j/2].Get(j%2);
      ns3::Simulator::ScheduleNow (&CheckQueueSize, queue[j],queuel,queuem,j,queuet);
     }
                                    

  NS_LOG_INFO ("Enable static global routing.");
  
  ns3::Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  ns3::Simulator::Stop(ns3::Seconds(200.0));
  //pointToPoint.EnablePcapAll("star");
  ns3::Simulator::Run (); 
            
  // latency of the window
  double tmp_nocputime=0;
  for(int j=0;j<(*leaf_nodes);j++){
       for(int q =0;q<sink[j]->GetIdNumber();q++){
           if(sink[j]->GetSimTime(q)>tmp_nocputime){
             tmp_nocputime = sink[j]->GetSimTime(q);   
           }
           cout<<"Leaf:"<<j<<" flow:"<<q<<" time:"<<sink[j]->GetSimTime(q)<<endl;
           real+=sink[j]->GetSimTime(q);
        }
     }
  nocputime+=tmp_nocputime;           
  cout<<"Time:"<<real<<" ns"<<endl;
  (*final_real)+=real;
  cout<<"Final_Time:"<<(*final_real)<<" ns"<<endl;  
          
  ns3::Ptr<ns3::Ipv4FlowClassifier> classifier = ns3::DynamicCast<ns3::Ipv4FlowClassifier> (flowmon.GetClassifier());
  std::map<ns3::FlowId, ns3::FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();


  double avghopcount = 0.0;
  double tmp_timesForward = 0;
  double tmp_rxPackets = 0;

for (std::map<ns3::FlowId, ns3::FlowMonitor::FlowStats>::const_iterator j = stats.begin(); j != stats.end(); ++j){
      	 ns3::Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (j->first);
         tmp_timesForward+=j->second.timesForwarded;
         real_timesForward+=j->second.timesForwarded;
         tmp_rxPackets+=j->second.rxPackets;
         real_rxPackets+=j->second.rxPackets;
         avghopcount = (j->second.timesForwarded)/j->second.rxPackets+1;
         cout<<j->first<<" From "<<t.sourceAddress<<" to "<<t.destinationAddress<<" avg hop count:"<<avghopcount<<endl;
      }

  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+3] = tmp_timesForward/tmp_rxPackets+1;
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+3] = tmp_timesForward/tmp_rxPackets+1;
  //cout<<"Total avg hops :"<<  phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+3] << endl;

  for(int j=0;j<2*(*leaf_nodes);j++){
    ptr2_window_info[i][j]=queuel[j];
    queuel[2*(*leaf_nodes)]+=queuel[j];
    
    ptr2_window_info[i][2*(*leaf_nodes)+j]=queueTime[j];
    ptr2_window_info[i][2*2*(*leaf_nodes)+2]+=queueTime[j];
    
    if(queuem[j]>queuem[2*(*leaf_nodes)]){
      ptr2_window_info[i][2*2*(*leaf_nodes)]=queuem[j];
      queuem[2*(*leaf_nodes)]=queuem[j];
    }
    
    if(queuel[2*(*leaf_nodes)]>=1){ 
      ptr2_window_info[i][2*2*(*leaf_nodes)+1]=queuel[2*(*leaf_nodes)]/(j+1);  
    }else{
      ptr2_window_info[i][2*2*(*leaf_nodes)+1]=0;
    }
  
    cout<<"avg queue length :"<<ptr2_window_info[i][2*2*(*leaf_nodes)+1]<<endl;
    cout<<"max queue length :"<<ptr2_window_info[i][2*2*(*leaf_nodes)]<<endl;
    cout<<"total queue congestion time :"<<ptr2_window_info[i][2*(*leaf_nodes)+j]<<endl;
    //cout<<"total queue congestion time :"<<window_info[i][2*2*(*leaf_nodes)+2]<<endl;
  }
  
  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+4] = ptr2_window_info[i][2*2*(*leaf_nodes)+1];
  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+5] = ptr2_window_info[i][2*2*(*leaf_nodes)];
  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+6] = ptr2_window_info[i][2*2*(*leaf_nodes)+2];
  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+7] = nocputime;
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+4] = ptr2_window_info[i][2*2*(*leaf_nodes)+1];
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+5] = ptr2_window_info[i][2*2*(*leaf_nodes)];
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+6] = ptr2_window_info[i][2*2*(*leaf_nodes)+2];
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+7] = nocputime;
  //cout<<"total avg queue length"<<queuel[9]/8<<endl;
  //cout<<"total max queue length"<<queuem[9]<<endl;
  
  ns3::Simulator::Destroy ();
  //  NS_LOG_INFO ("Done.");
  cout<<"--------------------"<<endl; 

      
    ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+2]=real;
    ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+2]=real;

  for(int j =0 ;j < (*leaf_nodes)*(*leaf_nodes)+2;j++){
          ptr2_phase_data_a[*(phase_data_a_number)][j]=ptr2_phase_data[i][j];
     }
         
     delete  queuel;
     delete  queuem;
     delete  queuet;

     (*phase_data_a_number)++;
     (*lock) =true;
    
 
}
void fat_tree (uint32_t *leaf_nodes,double **ptr2_phase_data,double **ptr2_phase_data_a,double* final_real,int* phase_data_a_number,int i,bool *lock,double **ptr2_window_info,double **ptr2_switch){
  ns3::Config::SetDefault ("ns3::TcpL4Protocol::SocketType", ns3::StringValue ("ns3::TcpNewReno"));
  ns3::Config::SetDefault ("ns3::TcpSocket::SegmentSize", ns3::UintegerValue (1472));
  ns3::Config::SetDefault ("ns3::TcpSocket::DelAckCount", ns3::UintegerValue (2));
  //ns3::LogComponentEnable ("PacketSink", ns3::LOG_LEVEL_INFO);
  double real =0;
  double nocputime =0;
  


  int k = 4;        
  int num_agg = 2;
  int num_host = (*leaf_nodes);
  int num_edge = 4;
  
  
  ns3::InternetStackHelper internet;
  //Ipv4NixVectorHelper nixRouting;

  //internet.SetRoutingHelper (nixRouting);
  //internet.Install(nodes);

  ns3::NodeContainer agg;
  agg.Create(num_agg);
  internet.Install(agg);
  ns3::NodeContainer edge;
  edge.Create(num_edge);
  internet.Install(edge);	
  ns3::NodeContainer host;
  host.Create((*leaf_nodes));	
  internet.Install(host);

  ns3::PointToPointHelper pointToPoint_agg_edge;
  pointToPoint_agg_edge.SetDeviceAttribute ("DataRate", ns3::StringValue ("120Gbps"));
  pointToPoint_agg_edge.SetDeviceAttribute ("InterframeGap", ns3::StringValue("0.8ns"));
  pointToPoint_agg_edge.SetDeviceAttribute ("Mtu", ns3::UintegerValue (1526));
  pointToPoint_agg_edge.SetChannelAttribute ("Delay", ns3::StringValue ("0.0ms"));
  pointToPoint_agg_edge.SetQueue ("ns3::DropTailQueue");
   
  ns3::PointToPointHelper pointToPoint_edge_host;
  pointToPoint_edge_host.SetDeviceAttribute ("DataRate", ns3::StringValue ("40Gbps"));
  pointToPoint_edge_host.SetDeviceAttribute ("InterframeGap", ns3::StringValue("2.4ns"));
  pointToPoint_edge_host.SetDeviceAttribute ("Mtu", ns3::UintegerValue (1526));
  pointToPoint_edge_host.SetChannelAttribute ("Delay", ns3::StringValue ("0.0ms"));
  pointToPoint_edge_host.SetQueue ("ns3::DropTailQueue");
   
           
  //set queue

  ns3::TrafficControlHelper tch;
  uint16_t handle = tch.SetRootQueueDisc ("ns3::PfifoFastQueueDisc");
  tch.AddInternalQueues (handle, 3, "ns3::DropTailQueue","MaxPackets", ns3::UintegerValue (1000));
  
  //uint16_t handle = tch.SetRootQueueDisc ("ns3::RedQueueDisc");
  //tch.AddInternalQueues (handle, 1, "ns3::DropTailQueue","MaxPackets", UintegerValue (10000));  
 
 

  
 
//////edge-agg/////  
  ns3::NodeContainer agg_edge[8];
  ns3::NetDeviceContainer agg_edge_dev[8];
  ns3::QueueDiscContainer agg_edge_qdiscs[8];
  ns3::Ipv4AddressHelper agg_edge_address;   
  ns3::Ipv4InterfaceContainer agg_edge_ip[8];

  char* base_agg_edge;
     for(int h=0;h<2;h++){
         for(int p=0;p<4;p++){
           agg_edge[h*4+p]=ns3::NodeContainer(agg.Get(h),edge.Get(p));
           agg_edge_dev[h*4+p]=pointToPoint_agg_edge.Install(agg_edge[h*4+p]);
           agg_edge_qdiscs[h*4+p]=tch.Install(agg_edge_dev[h*4+p]);
           base_agg_edge = toString(10,1,h*4+p,0); 
           agg_edge_address.SetBase(base_agg_edge,"255.255.255.0");
           agg_edge_ip[h*4+p] = agg_edge_address.Assign(agg_edge_dev[h*4+p]);
      }
    }
 

//////core-edge/////

  ns3::NodeContainer edge_host[(*leaf_nodes)+1];	
  ns3::NetDeviceContainer edge_host_dev[(*leaf_nodes)+1];
  ns3::QueueDiscContainer edge_host_qdiscs[(*leaf_nodes)+1];
  ns3::Ipv4AddressHelper edge_host_address;   
  ns3::Ipv4InterfaceContainer edge_host_ip[(*leaf_nodes)+1];
  char * base;

  int edge_host_number =0;	
     for(int h=0;h<(*leaf_nodes)/4;h++){
         for(int p=0;p<4;p++){
           edge_host[edge_host_number]=ns3::NodeContainer(edge.Get(edge_host_number%4),host.Get(edge_host_number));
           edge_host_dev[edge_host_number]=pointToPoint_edge_host.Install(edge_host[edge_host_number]);
           edge_host_qdiscs[edge_host_number]=tch.Install(edge_host_dev[edge_host_number]);
           base = toString(10,0,edge_host_number,0); 
           edge_host_address.SetBase(base,"255.255.255.0");
           edge_host_ip[edge_host_number] = edge_host_address.Assign(edge_host_dev[edge_host_number]);
           edge_host_number++;
      }
    }
  


 ns3::FlowMonitorHelper flowmon;
 ns3::Ptr<ns3::FlowMonitor> monitor = flowmon.InstallAll();
 
 ns3::Ptr<ns3::QueueDisc> queue[2*(*leaf_nodes)];
 double *queuel = NULL;
 queuel = new double[2*(*leaf_nodes)+1]; 
 double *queuem = NULL;
 queuem = new double[2*(*leaf_nodes)+1];
 double *queuet = NULL;
 queuet = new double[2*(*leaf_nodes)+1];

 ns3::Ptr<ns3::QueueDisc> queue_s[101];
 double *queuel_s = NULL;
 queuel_s = new double[101]; 
 double *queuem_s = NULL;
 queuem_s = new double[101];
 double *queuet_s = NULL;
 queuet_s = new double[101];


  
 for(int j=0;j<2*(*leaf_nodes);j++){
     queuel[j]=0;
     queuem[j]=0;
     queuet[j]=0;
 }
  cout<<"!!"<<endl;
 for(int j=0;j<100;j++){
     queuel_s[j]=0;
     queuem_s[j]=0;
     queuet_s[j]=0;
 }


 for(int j=0;j < 2*(*leaf_nodes);j++){
     queue[j]=edge_host_qdiscs[j/2].Get(j%2);
     ns3::Simulator::ScheduleNow (&CheckQueueSize, queue[j],queuel,queuem,j,queuet);
    }


 for(int j=0;j < 16;j++){
     queue_s[j]=agg_edge_qdiscs[j/2].Get(j%2);
     ns3::Simulator::ScheduleNow (&CheckQueueSize_s, queue_s[j],queuel_s,queuem_s,j,queuet_s);
    }

 uint16_t port = 50000;
            

  ns3::ApplicationContainer hubApp[(*leaf_nodes)+1];
  ns3::Ptr<ns3::PacketSink> sink[(*leaf_nodes)+1]; 
  
  int host_numbers = 0;         
     for(int h=0;h<(*leaf_nodes/4);h++){
         for(int p=0;p<4;p++){			 	
        ns3::Address hubLocalAddress (ns3::InetSocketAddress (ns3::Ipv4Address::GetAny(), port));
        ns3::PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", hubLocalAddress);
        hubApp[host_numbers] = packetSinkHelper.Install (host.Get(host_numbers));
        hubApp[host_numbers].Start (ns3::Seconds (0.0));
        hubApp[host_numbers].Stop (ns3::Seconds (200.0));
        sink[host_numbers]=ns3::DynamicCast<ns3::PacketSink> (hubApp[host_numbers].Get(0)); 
        host_numbers++;
       }
     }
  







  

  ns3::ApplicationContainer spokeApps[(*leaf_nodes)*(*leaf_nodes)];   
               
  for(int q =0;q<(*leaf_nodes)*(*leaf_nodes);q++){
     if(ptr2_phase_data[i][q+2]<=0){
        //cout<<"q:"<<q<<endl;
        continue;
     }
        ns3::BulkSendHelper bsHelper ("ns3::TcpSocketFactory" , ns3::Ipv4Address::GetAny()); 
        bsHelper.SetAttribute("SendSize",ns3::UintegerValue(1472));
        uint64_t maxby =(uint64_t)ptr2_phase_data[i][q+2]; 
        bsHelper.SetAttribute ("MaxBytes",ns3::UintegerValue(maxby));     
          
        ns3::AddressValue remoteAddress (ns3::InetSocketAddress (edge_host_ip[q%(*leaf_nodes)].GetAddress (1), port));
        bsHelper.SetAttribute ("Remote", remoteAddress);            

        spokeApps[q].Add (bsHelper.Install (host.Get(q/(*leaf_nodes))));//
        spokeApps[q].Start (ns3::Seconds (0.0));
        spokeApps[q].Stop (ns3::Seconds (200.0));
       
  }  

  // pointToPoint.EnablePcapAll("star");

 ns3::Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 ns3::Simulator::Stop(ns3::Seconds(200.0));
 ns3::Simulator::Run ();       

  // latency of the window
  double tmp_nocputime=0;

  for(int j=0;j<(*leaf_nodes);j++){
       for(int q =0;q<sink[j]->GetIdNumber();q++){
           if(sink[j]->GetSimTime(q)>tmp_nocputime){
             tmp_nocputime = sink[j]->GetSimTime(q);   
           }
           //cout<<"Leaf:"<<j<<" flow:"<<q<<" time:"<<sink[j]->GetSimTime(q)<<endl;
           real+=sink[j]->GetSimTime(q);
        }
     }

 
  nocputime+=tmp_nocputime;           
  cout<<"Time:"<<real<<" ns"<<endl;
  (*final_real)+=real;
  cout<<"Final_Time:"<<(*final_real)<<" ns"<<endl;  
          
  ns3::Ptr<ns3::Ipv4FlowClassifier> classifier = ns3::DynamicCast<ns3::Ipv4FlowClassifier> (flowmon.GetClassifier());
  std::map<ns3::FlowId, ns3::FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();

  double avghopcount = 0.0;
  double tmp_timesForward = 0;
  double tmp_rxPackets = 0;
            

  for (std::map<ns3::FlowId, ns3::FlowMonitor::FlowStats>::const_iterator j = stats.begin(); j != stats.end(); ++j){
      	 ns3::Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (j->first);
         tmp_timesForward+=j->second.timesForwarded;
         real_timesForward+=j->second.timesForwarded;
         tmp_rxPackets+=j->second.rxPackets;
         real_rxPackets+=j->second.rxPackets;
         avghopcount = (j->second.timesForwarded)/j->second.rxPackets+1;
         //cout<<j->first<<" From "<<t.sourceAddress<<" to "<<t.destinationAddress<<" avg hop count:"<<avghopcount<<endl;
      }

  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+3] = tmp_timesForward/tmp_rxPackets+1;
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+3] = tmp_timesForward/tmp_rxPackets+1;
  //cout<<"Total avg hops :"<<  phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+3] << endl;
  for(int j=0;j<2*(*leaf_nodes);j++){
    ptr2_window_info[i][j]=queuel[j];
    queuel[2*(*leaf_nodes)]+=queuel[j];
    
    ptr2_window_info[i][2*(*leaf_nodes)+j]=queueTime[j];
    ptr2_window_info[i][2*2*(*leaf_nodes)+2]+=queueTime[j];
    
    if(queuem[j]>queuem[2*(*leaf_nodes)]){
      ptr2_window_info[i][2*2*(*leaf_nodes)]=queuem[j];
      queuem[2*(*leaf_nodes)]=queuem[j];
    }
    
    if(queuel[2*(*leaf_nodes)]>=1){ 
      ptr2_window_info[i][2*2*(*leaf_nodes)+1]=queuel[2*(*leaf_nodes)]/(j+1);  
    }else{
      ptr2_window_info[i][2*2*(*leaf_nodes)+1]=0;
    }
  
   /* cout<<"avg queue length :"<<ptr2_window_info[i][2*2*(*leaf_nodes)+1]<<endl;
    cout<<"max queue length :"<<ptr2_window_info[i][2*2*(*leaf_nodes)]<<endl;
    cout<<"Q :"<<j<<" total queue congestion time :"<<ptr2_window_info[i][2*(*leaf_nodes)+j]<<endl;
    cout<<"total queue congestion time :"<<ptr2_window_info[i][2*2*(*leaf_nodes)+2]<<endl;*/
  }

  for(int j=0;j<16;j++){
    ptr2_switch[i][j]=queuel_s[j];
    queuel_s[16]+=queuel_s[j];
    
    ptr2_switch[i][16+j]=queueTime_s[j];
    ptr2_switch[i][34]+=queueTime_s[j];
    
    if(queuem_s[j]>queuem_s[16]){
      ptr2_switch[i][32]=queuem_s[j];
      queuem_s[16]=queuem_s[j];
    }
    
    if(queuel_s[16]>=1){ 
      ptr2_switch[i][33]=queuel_s[16]/(j+1);  
    }else{
      ptr2_switch[i][33]=0;
    }
  
  /*  cout<<"Switch avg queue length :"<<ptr2_switch[i][33]<<endl;
    cout<<"Switch max queue length :"<<ptr2_switch[i][32]<<endl;
    cout<<"Switch Q :"<<j<<" total queue congestion time :"<<ptr2_switch[i][16+j]<<endl;
    cout<<"Switch total queue congestion time :"<<ptr2_switch[i][34]<<endl;*/
  }
  
  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+4] = ptr2_window_info[i][2*2*(*leaf_nodes)+1];
  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+5] = ptr2_window_info[i][2*2*(*leaf_nodes)];
  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+6] = ptr2_window_info[i][2*2*(*leaf_nodes)+2];
  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+7] = nocputime;
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+4] = ptr2_window_info[i][2*2*(*leaf_nodes)+1];
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+5] = ptr2_window_info[i][2*2*(*leaf_nodes)];
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+6] = ptr2_window_info[i][2*2*(*leaf_nodes)+2];
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+7] = nocputime;

  //cout<<"total avg queue length"<<queuel[9]/8<<endl;
  //cout<<"total max queue length"<<queuem[9]<<endl;
 
  ns3::Simulator::Destroy ();
  //  NS_LOG_INFO ("Done.");
  cout<<"--------------------"<<endl; 

    
    ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+2]=real;
    ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+2]=real;

    for(int j =0 ;j < (*leaf_nodes)*(*leaf_nodes)+2;j++){
          ptr2_phase_data_a[*(phase_data_a_number)][j]=ptr2_phase_data[i][j];
    }
  
    (*phase_data_a_number)++;
    (*lock) =true;
  
 

         
     delete  queuel;
     delete  queuem;
     delete  queuet;
     delete  queuel_s;
     delete  queuem_s;
     delete  queuet_s;

}

void DD_tour (uint32_t *leaf_nodes,double **ptr2_phase_data,double **ptr2_phase_data_a,double* final_real,int* phase_data_a_number,int i,bool *lock,double **ptr2_window_info,double **ptr2_switch){
  ns3::Config::SetDefault ("ns3::TcpL4Protocol::SocketType", ns3::StringValue ("ns3::TcpNewReno"));
  ns3::Config::SetDefault ("ns3::TcpSocket::SegmentSize", ns3::UintegerValue (1472));//ori:1472
  ns3::Config::SetDefault ("ns3::TcpSocket::DelAckCount", ns3::UintegerValue (2));
  //LogComponentEnable ("PacketSink", LOG_LEVEL_INFO);
  double real =0;
  double nocputime =0;
  


  int k = 4;        
  int num_agg = 2;
  int num_host = (*leaf_nodes);
  int num_edge = 8;


  ns3::InternetStackHelper internet;
  //Ipv4NixVectorHelper nixRouting;

  //internet.SetRoutingHelper (nixRouting);
  //internet.Install(nodes);


  ns3::NodeContainer edge;
  edge.Create(num_edge);
  internet.Install(edge);	
  ns3::NodeContainer host;
  host.Create((*leaf_nodes));	
  internet.Install(host);

  ns3::PointToPointHelper pointToPoint_edge_edge;
  pointToPoint_edge_edge.SetDeviceAttribute ("DataRate", ns3::StringValue ("120Gbps"));
  pointToPoint_edge_edge.SetDeviceAttribute ("InterframeGap", ns3::StringValue("0.8ns"));
  pointToPoint_edge_edge.SetDeviceAttribute ("Mtu", ns3::UintegerValue (1526));
  pointToPoint_edge_edge.SetChannelAttribute ("Delay", ns3::StringValue ("0.0ms"));
  pointToPoint_edge_edge.SetQueue ("ns3::DropTailQueue");
   
  ns3::PointToPointHelper pointToPoint_edge_host;
  pointToPoint_edge_host.SetDeviceAttribute ("DataRate", ns3::StringValue ("40Gbps"));
  pointToPoint_edge_host.SetDeviceAttribute ("InterframeGap", ns3::StringValue("2.4ns"));
  pointToPoint_edge_host.SetDeviceAttribute ("Mtu", ns3::UintegerValue (1526));
  pointToPoint_edge_host.SetChannelAttribute ("Delay", ns3::StringValue ("0.0ms"));
  pointToPoint_edge_host.SetQueue ("ns3::DropTailQueue");
   
           
  //set queue

  ns3::TrafficControlHelper tch;
  uint16_t handle = tch.SetRootQueueDisc ("ns3::PfifoFastQueueDisc");
  tch.AddInternalQueues (handle, 3, "ns3::DropTailQueue","MaxPackets", ns3::UintegerValue (1000));
  
  //uint16_t handle = tch.SetRootQueueDisc ("ns3::RedQueueDisc");
  //tch.AddInternalQueues (handle, 1, "ns3::DropTailQueue","MaxPackets", UintegerValue (10000));  
 
 

  
 
//////edge-agg/////  
  ns3::NodeContainer edge_edge[12];
  ns3::NodeContainer edge_edge_t[12];
  ns3::NetDeviceContainer edge_edge_dev[12];
  ns3::NetDeviceContainer edge_edge_dev_t[12];
  ns3::QueueDiscContainer edge_edge_qdiscs[12];
  ns3::QueueDiscContainer edge_edge_qdiscs_t[12];
  ns3::Ipv4AddressHelper edge_edge_address;  
  ns3::Ipv4AddressHelper edge_edge_address_1; 
  ns3::Ipv4AddressHelper edge_edge_address_t;  
  ns3::Ipv4AddressHelper edge_edge_address_1_t; 
  ns3::Ipv4InterfaceContainer edge_edge_ip[12];
  ns3::Ipv4InterfaceContainer edge_edge_ip_t[12];

  char* base_edge_edge;
  char* base_edge_edge_1;
     for(int h=0;h<4;h++){
           edge_edge[h]=ns3::NodeContainer(edge.Get(h),edge.Get(h+4));
           edge_edge_dev[h]=pointToPoint_edge_edge.Install(edge_edge[h]);
           edge_edge_qdiscs[h]=tch.Install(edge_edge_dev[h]);
           base_edge_edge = toString(10,1,h,0); 
           edge_edge_address.SetBase(base_edge_edge,"255.255.255.0");
           edge_edge_ip[h] = edge_edge_address.Assign(edge_edge_dev[h]);
    }
    for(int h=4;h<8;h++){
           edge_edge[h]=ns3::NodeContainer(edge.Get(2*(h-4)),edge.Get(2*(h-4)+1));
           edge_edge_dev[h]=pointToPoint_edge_edge.Install(edge_edge[h]);
           edge_edge_qdiscs[h]=tch.Install(edge_edge_dev[h]);
           base_edge_edge = toString(10,1,h,0); 
           edge_edge_address.SetBase(base_edge_edge,"255.255.255.0");
           edge_edge_ip[h] = edge_edge_address.Assign(edge_edge_dev[h]);
    }
    for(int h=8;h<10;h++){
           edge_edge[h]=ns3::NodeContainer(edge.Get(h-8),edge.Get(h-6));
           edge_edge[h+2]=ns3::NodeContainer(edge.Get(h-4),edge.Get(h-2));
           edge_edge_dev[h]=pointToPoint_edge_edge.Install(edge_edge[h]);
           edge_edge_dev[h+2]=pointToPoint_edge_edge.Install(edge_edge[h+2]);
           edge_edge_qdiscs[h]=tch.Install(edge_edge_dev[h]);
           edge_edge_qdiscs[h+2]=tch.Install(edge_edge_dev[h+2]);
           base_edge_edge = toString(10,1,h,0); 
           base_edge_edge_1 = toString(10,1,(h+2),0);
           edge_edge_address.SetBase(base_edge_edge,"255.255.255.0");
           edge_edge_ip[h] = edge_edge_address.Assign(edge_edge_dev[h]);   
           edge_edge_address_1.SetBase(base_edge_edge_1,"255.255.255.0");
           edge_edge_ip[h+2] = edge_edge_address_1.Assign(edge_edge_dev[h+2]);   
    }

     for(int h=0;h<4;h++){
           edge_edge_t[h]=ns3::NodeContainer(edge.Get(h),edge.Get(h+4));
           edge_edge_dev_t[h]=pointToPoint_edge_edge.Install(edge_edge_t[h]);
           edge_edge_qdiscs_t[h]=tch.Install(edge_edge_dev_t[h]);
           base_edge_edge = toString(10,1,h+13,0); 
           edge_edge_address_t.SetBase(base_edge_edge,"255.255.255.0");
           edge_edge_ip_t[h] = edge_edge_address_t.Assign(edge_edge_dev_t[h]);
    }
    for(int h=4;h<8;h++){
           edge_edge_t[h]=ns3::NodeContainer(edge.Get(2*(h-4)),edge.Get(2*(h-4)+1));
           edge_edge_dev_t[h]=pointToPoint_edge_edge.Install(edge_edge_t[h]);
           edge_edge_qdiscs_t[h]=tch.Install(edge_edge_dev_t[h]);
           base_edge_edge = toString(10,1,h+13,0); 
           edge_edge_address_t.SetBase(base_edge_edge,"255.255.255.0");
           edge_edge_ip_t[h] = edge_edge_address_t.Assign(edge_edge_dev_t[h]);
    }
    for(int h=8;h<10;h++){
           edge_edge_t[h]=ns3::NodeContainer(edge.Get(h-8),edge.Get(h-6));
           edge_edge_t[h+2]=ns3::NodeContainer(edge.Get(h-4),edge.Get(h-2));
           edge_edge_dev_t[h]=pointToPoint_edge_edge.Install(edge_edge_t[h]);
           edge_edge_dev_t[h+2]=pointToPoint_edge_edge.Install(edge_edge_t[h+2]);
           edge_edge_qdiscs_t[h]=tch.Install(edge_edge_dev_t[h]);
           edge_edge_qdiscs_t[h+2]=tch.Install(edge_edge_dev_t[h+2]);
           base_edge_edge = toString(10,1,h+13,0); 
           base_edge_edge_1 = toString(10,1,(h+15),0);
           edge_edge_address_t.SetBase(base_edge_edge,"255.255.255.0");
           edge_edge_ip_t[h] = edge_edge_address_t.Assign(edge_edge_dev_t[h]);   
           edge_edge_address_1_t.SetBase(base_edge_edge_1,"255.255.255.0");
           edge_edge_ip_t[h+2] = edge_edge_address_1_t.Assign(edge_edge_dev_t[h+2]);   
    }


//////core-edge/////

  ns3::NodeContainer edge_host[(*leaf_nodes)+1];	
  ns3::NetDeviceContainer edge_host_dev[(*leaf_nodes)+1];
  ns3::QueueDiscContainer edge_host_qdiscs[(*leaf_nodes)+1];
  ns3::Ipv4AddressHelper edge_host_address;   
  ns3::Ipv4InterfaceContainer edge_host_ip[(*leaf_nodes)+1];
  char * base;

  int edge_host_number =0;	
     for(int h=0;h<(*leaf_nodes)/8;h++){
         for(int p=0;p<8;p++){
           edge_host[edge_host_number]=ns3::NodeContainer(edge.Get(edge_host_number%8),host.Get(edge_host_number));
           edge_host_dev[edge_host_number]=pointToPoint_edge_host.Install(edge_host[edge_host_number]);
           edge_host_qdiscs[edge_host_number]=tch.Install(edge_host_dev[edge_host_number]);
           base = toString(10,0,edge_host_number,0); 
           edge_host_address.SetBase(base,"255.255.255.0");
           edge_host_ip[edge_host_number] = edge_host_address.Assign(edge_host_dev[edge_host_number]);
           edge_host_number++;
      }
    }
  



 ns3::FlowMonitorHelper flowmon;
 ns3::Ptr<ns3::FlowMonitor> monitor = flowmon.InstallAll();
 
 ns3::Ptr<ns3::QueueDisc> queue[2*(*leaf_nodes)];
 double *queuel = NULL;
 queuel = new double[2*(*leaf_nodes)+1]; 
 double *queuem = NULL;
 queuem = new double[2*(*leaf_nodes)+1];
 double *queuet = NULL;
 queuet = new double[2*(*leaf_nodes)+1];

 ns3::Ptr<ns3::QueueDisc> queue_s[200];
 double *queuel_s = NULL;
 queuel_s = new double[200]; 
 double *queuem_s = NULL;
 queuem_s = new double[200];
 double *queuet_s = NULL;
 queuet_s = new double[200];

   
 for(int j=0;j<2*(*leaf_nodes);j++){
     queuel[j]=0;
     queuem[j]=0;
     queuet[j]=0;
 }
  cout<<"!!"<<endl;
 for(int j=0;j<100;j++){
     queuel_s[j]=0;
     queuem_s[j]=0;
     queuet_s[j]=0;
 }

 for(int j=0;j < 2*(*leaf_nodes);j++){
     queue[j]=edge_host_qdiscs[j/2].Get(j%2);
     ns3::Simulator::ScheduleNow (&CheckQueueSize, queue[j],queuel,queuem,j,queuet);
    }

 for(int j=0;j < 24;j++){
     queue_s[j]=edge_edge_qdiscs[j/2].Get(j%2);
     ns3::Simulator::ScheduleNow (&CheckQueueSize_s, queue_s[j],queuel_s,queuem_s,j,queuet_s);
    }

 for(int j=24;j < 48;j++){
     queue_s[j]=edge_edge_qdiscs_t[(j-24)/2].Get((j-24)%2);
     ns3::Simulator::ScheduleNow (&CheckQueueSize_s, queue_s[j],queuel_s,queuem_s,j,queuet_s);
    }
 uint16_t port = 50000;
            

  ns3::ApplicationContainer hubApp[(*leaf_nodes)+1];
  ns3::Ptr<ns3::PacketSink> sink[(*leaf_nodes)+1]; 
  
  int host_numbers = 0;         
     for(int h=0;h<(*leaf_nodes/8);h++){
         for(int p=0;p<8;p++){			 	
        ns3::Address hubLocalAddress (ns3::InetSocketAddress (ns3::Ipv4Address::GetAny(), port));
        ns3::PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", hubLocalAddress);
        hubApp[host_numbers] = packetSinkHelper.Install (host.Get(host_numbers));
        hubApp[host_numbers].Start (ns3::Seconds (0.0));
        hubApp[host_numbers].Stop (ns3::Seconds (200.0));
        sink[host_numbers]=ns3::DynamicCast<ns3::PacketSink> (hubApp[host_numbers].Get(0)); 
        host_numbers++;
       }
     }
  


  

  ns3::ApplicationContainer spokeApps[(*leaf_nodes)*(*leaf_nodes)];   
               
  for(int q =0;q<(*leaf_nodes)*(*leaf_nodes);q++){
     if(ptr2_phase_data[i][q+2]<=0){
        //cout<<"q:"<<q<<endl;
        continue;
     }
        ns3::BulkSendHelper bsHelper ("ns3::TcpSocketFactory" , ns3::Ipv4Address::GetAny()); 
        bsHelper.SetAttribute("SendSize",ns3::UintegerValue(1472));
        uint64_t maxby =(uint64_t)ptr2_phase_data[i][q+2]; 
        bsHelper.SetAttribute ("MaxBytes",ns3::UintegerValue(maxby));     
          
        ns3::AddressValue remoteAddress (ns3::InetSocketAddress (edge_host_ip[q%(*leaf_nodes)].GetAddress (1), port)); //
        bsHelper.SetAttribute ("Remote", remoteAddress);            

        spokeApps[q].Add (bsHelper.Install (host.Get(q/(*leaf_nodes))));//
        spokeApps[q].Start (ns3::Seconds (0.0));
        spokeApps[q].Stop (ns3::Seconds (200.0));
       
  }  

  // pointToPoint.EnablePcapAll("star");

 ns3::Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
 ns3::Simulator::Stop(ns3::Seconds(200.0));
 ns3::Simulator::Run ();       

  // latency of the window
  double tmp_nocputime=0;

  for(int j=0;j<(*leaf_nodes);j++){
       for(int q =0;q<sink[j]->GetIdNumber();q++){
           if(sink[j]->GetSimTime(q)>tmp_nocputime){
             tmp_nocputime = sink[j]->GetSimTime(q);   
           }
           //cout<<"Leaf:"<<j<<" flow:"<<q<<" time:"<<sink[j]->GetSimTime(q)<<endl;
           real+=sink[j]->GetSimTime(q);
        }
     }

 
  nocputime+=tmp_nocputime;           
  cout<<"Time:"<<real<<" ns"<<endl;
  (*final_real)+=real;
  cout<<"Final_Time:"<<(*final_real)<<" ns"<<endl;  
          
  ns3::Ptr<ns3::Ipv4FlowClassifier> classifier = ns3::DynamicCast<ns3::Ipv4FlowClassifier> (flowmon.GetClassifier());
  std::map<ns3::FlowId, ns3::FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();

  double avghopcount = 0.0;
  double tmp_timesForward = 0;
  double tmp_rxPackets = 0;
            

  for (std::map<ns3::FlowId, ns3::FlowMonitor::FlowStats>::const_iterator j = stats.begin(); j != stats.end(); ++j){
      	 ns3::Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (j->first);
         tmp_timesForward+=j->second.timesForwarded;
         real_timesForward+=j->second.timesForwarded;
         tmp_rxPackets+=j->second.rxPackets;
         real_rxPackets+=j->second.rxPackets;
         avghopcount = (j->second.timesForwarded)/j->second.rxPackets+1;
        // cout<<j->first<<" From "<<t.sourceAddress<<" to "<<t.destinationAddress<<" avg hop count:"<<avghopcount<<endl;
      }

  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+3] = tmp_timesForward/tmp_rxPackets+1;
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+3] = tmp_timesForward/tmp_rxPackets+1;
  //cout<<"Total avg hops :"<<  phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+3] << endl;
  for(int j=0;j<2*(*leaf_nodes);j++){
    ptr2_window_info[i][j]=queuel[j];
    queuel[2*(*leaf_nodes)]+=queuel[j];
    
    ptr2_window_info[i][2*(*leaf_nodes)+j]=queueTime[j];
    ptr2_window_info[i][2*2*(*leaf_nodes)+2]+=queueTime[j];
    
    if(queuem[j]>queuem[2*(*leaf_nodes)]){
      ptr2_window_info[i][2*2*(*leaf_nodes)]=queuem[j];
      queuem[2*(*leaf_nodes)]=queuem[j];
    }
    
    if(queuel[2*(*leaf_nodes)]>=1){ 
      ptr2_window_info[i][2*2*(*leaf_nodes)+1]=queuel[2*(*leaf_nodes)]/(j+1);  
    }else{
      ptr2_window_info[i][2*2*(*leaf_nodes)+1]=0;
    }
  
   /* cout<<"avg queue length :"<<ptr2_window_info[i][2*2*(*leaf_nodes)+1]<<endl;
    cout<<"max queue length :"<<ptr2_window_info[i][2*2*(*leaf_nodes)]<<endl;
    cout<<"total queue congestion time :"<<ptr2_window_info[i][2*(*leaf_nodes)+j]<<endl;
    //cout<<"total queue congestion time :"<<window_info[i][2*2*(*leaf_nodes)+2]<<endl;*/
  }

  for(int j=0;j<48;j++){
    ptr2_switch[i][j]=queuel_s[j];
    queuel_s[48]+=queuel_s[j];
    
    ptr2_switch[i][48+j]=queueTime_s[j];
    ptr2_switch[i][98]+=queueTime_s[j];
    
    if(queuem_s[j]>queuem_s[48]){
      ptr2_switch[i][96]=queuem_s[j];
      queuem_s[48]=queuem_s[j];
    }
    
    if(queuel_s[48]>=1){ 
      ptr2_switch[i][97]=queuel_s[48]/(j+1);  
    }else{
      ptr2_switch[i][97]=0;
    }
  
   /* cout<<"Switch avg queue length :"<<ptr2_switch[i][49]<<endl;
    cout<<"Switch max queue length :"<<ptr2_switch[i][48]<<endl;
    cout<<"Switch Q :"<<j<<" total queue congestion time :"<<ptr2_switch[i][24+j]<<endl;
    cout<<"Switch total queue congestion time :"<<ptr2_switch[i][50]<<endl;*/
  }
  
  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+4] = ptr2_window_info[i][2*2*(*leaf_nodes)+1];
  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+5] = ptr2_window_info[i][2*2*(*leaf_nodes)];
  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+6] = ptr2_window_info[i][2*2*(*leaf_nodes)+2];
  ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+7] = nocputime;
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+4] = ptr2_window_info[i][2*2*(*leaf_nodes)+1];
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+5] = ptr2_window_info[i][2*2*(*leaf_nodes)];
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+6] = ptr2_window_info[i][2*2*(*leaf_nodes)+2];
  ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+7] = nocputime;

  //cout<<"total avg queue length"<<queuel[9]/8<<endl;
  //cout<<"total max queue length"<<queuem[9]<<endl;
 
  ns3::Simulator::Destroy ();
  //  NS_LOG_INFO ("Done.");
  cout<<"--------------------"<<endl; 

    
    ptr2_phase_data_a[*(phase_data_a_number)][(*leaf_nodes)*(*leaf_nodes)+2]=real;
    ptr2_phase_data[i][(*leaf_nodes)*(*leaf_nodes)+2]=real;

    for(int j =0 ;j < (*leaf_nodes)*(*leaf_nodes)+2;j++){
          ptr2_phase_data_a[*(phase_data_a_number)][j]=ptr2_phase_data[i][j];
    }
  
    (*phase_data_a_number)++;
    (*lock) =true;
  
 

         
     delete  queuel;
     delete  queuem;
     delete  queuet;
     delete  queuel_s;
     delete  queuem_s;
     delete  queuet_s;

}


int parse_config(char* config_file, auto& vec_pcapfile, auto& parameter)
{
    libconfig::Config cfg;

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
