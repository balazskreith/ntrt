# ntrt
Network Traffic Rate Tracker

**What is it?**
  
Network Traffic Rate Tracker is yet another network 
analyzing and monitoring tool using pcaps. 
The main purpose of the program, aside that I 
fed up using the combination of many others, 
is that it accumulates and measure network traffic on 
specified interfaces and export it into a csv output. 


**What is it good for?**

I used it to measure the traffic throughput 
over a second long sliding window by a 100ms sampling period 
and then analyzing and creating nice plot from the output
so Profs. are understand. 

**What is the difference between this and thousands of others?**

Thats mine. Besides, I was fed up to create shell scripts and 
configure tcpdump, tcprate and sar and awk to get what I want. 
It has a slightly (or highly) chance that somebody else did 
something very similar in order to measure the traffic rate 
on network devices (No, not iperf, since it generates it also), 
but that wonderful tool was hidden from my eyes before. 
Next to it, I made it scalable and (for me) easily programmable 
so you can measure your specific traffic if you read the developers 
manual page. 

**Can I extend it and add my own wonderful tools?**

Yes.   
0. Get familiar with devclego framework (link: https://github.com/balazskreith/devclego)  
1. Read the developer manual (A direct link is at the bottom of the page <- TODO).  
2. Clone the repo (or fork whatever you want)  
3. Edit.  

**The Latest Version**

Details of the latest version can be found at   
https://github.com/balazskreith/ntrt  

**Install and Run**

 1. Download or clone the source.  
 2. ./confgiure
 3. sudo make install
 4. Setup config.ini (see below)  
 5. ./ntrt -cpath/to/config.ini  

**Example**

The following example filter udp and tcp traffic parallely with a 100ms resolution and a one seconds accumulation

*The contents of the config.ini file:*

[global]  
sampling_rate      = 100  
pcap_listeners_num = 1  

[pcap_0]  
accumulation_length = 10  
device              = wlan0  
output              = wlan0_stats.csv  
feature_num         = 4  
feature_0           = UDP_PACKETS  
feature_1           = TCP_PACKETS  
feature_2           = TCP_BYTES  
feature_3           = UDP_BYTES  


*The script file*

./ntrt -cconfig.ini  

The output is a comma separated list in wlan0_stats.csv. The sampling period determines the resolution of the measure and the accumulation length determines how many sampling period is accumulated in one measure. The example above sampling is 100ms and accumulates the last 10 samples.  

More examples at https://goo.gl/RIqGFO  

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

*MAPPED_VAR_X*  
Track a value mapped to an identifier X. (variable X can be changed from a command line)


    

