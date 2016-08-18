# ntrt
Network Traffic Rate Tracker

**What is it?**  
   
Network Traffic Rate Tracker is yet another network 
analyzing and monitoring tool for network interfaces. 
It analyze packets send through the selected interface in or out.
By analyzing them it tracks an accumulative statistics about the 
analization results (TCP flownum, UDP bytes for specific port, etc...).
By defining a sampling period ntrt records the results of the accumulative statistics 
into a csv file. 


**What is it good for?**  
  
It is recommended to use for creating accumulative statistics 
for network traffics on specified interface(s). For an instance 
we can capture how many UDP bytes goes through a network device 
in one seconds by sampling it in every 100ms. NTRT accumulates the 
UDP byte traffic over the last one second in a sliding window works inside 
of the program and record the measurement results after each 100ms. 
By using the csv output directly we can monitor the traffic in 
plots real-time. 


**What type of network traffic can be tracked?**  
  
*For UDP/TCP traffic:* You can track the number of packets and the 
accumulated size of bytes for UDP/TCP traffic. You can filter it by 
restricting the source or destination port. You can track the number of parallel TCP flows either.  
  
*For RTP traffic:* You can track the number of packets and the 
accumulated size of bytes for specified payload type on a source destination port you defined. 
It is also implemented as features that lost packets and frames are tracked.    
_Note_: You are obligated to ensure only RTP traffic goes through on the selected port.    
_Note 2_: Lost frames and packets are lies on a fact that sequence is not reordered. Please NOT rely 
on this metric if the high jitter is observed.   

**How can I configure NTRT to listen a network device?**

All it comes from a config file obligatory for ntrt to work. In config file 
you can define the interfaces you would like to listen and the list of features 
you want to keep track on the selected devices. See the example config files later.


**Requirements** 
  
NTRT works under linux. *libpcap* is obligated to be installed.  
(sudo apt-get install libpcap-dev)  
  
**Install**

```
 clone http://github.com/balazskreith/ntrt  
 ./confgiure && make  
 sudo make install  
 ```
 
**Example of config file**

The following example filter udp and tcp traffic parallely. It keeps track of 1s sliding window (accumulation time) and sampling it with a 100ms period.  

```
[global]  
accumulation_time  = 1000  
sampling_rate      = 100  
pcap_listeners_num = 1  

[pcap_0]  
device              = wlan0  
output              = wlan0_stats.csv  
feature_num         = 4  
feature_0           = UDP_PACKETS  
feature_1           = TCP_PACKETS  
feature_2           = TCP_BYTES  
feature_3           = UDP_BYTES  
```

**Example of running ntrt**

./ntrt -cconfig.ini  

The output is a comma separated list in wlan0_stats.csv. 



**Available Features**

The following features are available:  

*IP_PACKETS*  
Track the number of ip packets.

*IP_BYTES*  
Track the length of the ip packets

*TCP_PACKETS*  
Track the number of TCP packets.

*TCP_BYTES*  
Track the length of TCP packets.

*UDP_PACKETS*  
Track the number of UDP packets.

*UDP_BYTES*  
Track the length of UDP packets.

*RTP_PACKETS_X*  
Track the number of RTP packets with a given payload type. (Only useful if only RTP packets goes through the device).

*RTP_BYTES_X*  
Track the length of RTP packets with a given payload type. (Only useful if only RTP packets goes through the device).

*LOST_RTP_PACKETS_X*  
Track the number of gaps experienced for a given payload type. (Only useful if only RTP packets goes through the device).

*SRC_UDP_PACKETS_X*  
Track the number of UDP packets originated from port X.

*SRC_UDP_BYTES_X*  
Track the length of UDP packets originated from port X.

*SRC_TCP_PACKETS_X*  
Track the number of TCP packets originated from port X.

*SRC_TCP_BYTES_X*  
Track the length of TCP packets originated from port X.

*SRC_RTP_PACKETS_X_Y*  
Track the number of RTP packets originated from port X with a given payload type Y.

*SRC_RTP_BYTES_X_Y*  
Track the length of RTP packets originated from port X with a given payload type Y.

*SRC_LOST_RTP_PACKETS_X_Y*  
Track the number of gaps experienced on packets originated from port X with a given payload type Y.

*DST_UDP_PACKETS_X*  
Track the number of UDP packets sent to port X.

*DST_UDP_BYTES_X*  
Track the length of UDP packets sent to port X.

*DST_TCP_PACKETS_X*  
Track the number of TCP packets sent to port X.

*DST_TCP_BYTES_X*  
Track the length of TCP packets sent to port X.

*DST_RTP_PACKETS_X_Y*  
Track the number of RTP packets sent to port X with a given payload type Y.

*DST_RTP_BYTES_X_Y*  
Track the length of RTP packets sent to port X with a given payload type Y.

*DST_LOST_RTP_PACKETS_X_Y*  
Track the number of gaps experienced for RTP packets sent to port X with a given payload type Y.


**Can I extend it and add my own wonderful tools?**

Yes.   
0. Get familiar with devclego framework (link: https://github.com/balazskreith/devclego)  
1. Read the developer manual (A direct link is at the bottom of the page <- TODO).  
2. Clone the repo (or fork whatever you want)  
3. Edit.  

**The Latest Version**

Details of the latest version can be found at   
https://github.com/balazskreith/ntrt      

