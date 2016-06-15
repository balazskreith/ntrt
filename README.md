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
1. Read the developer manual (A direct link is at the bottom of the page <- TODO).  
2. Clone the repo (or fork whatever you want)  
3. Edit.  

**The Latest Version**

Details of the latest version can be found at   
https://github.com/balazskreith/ntrt  

**Install and Run**

 1. Download or clone the source.  
 2. ./confgiure; make install   
 3. Setup config.ini (see below)  
 4. ./ntrt -cpath/to/config.ini  

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

More examples on TODO....  

**Available Features**

The following features are available:  

*UDP_PACKETS*   
                    
*UDP_BYTES*       
                 
*TCP_PACKETS*        
               
*TCP_BYTES*              
      
*PACKETS_SRC_PORT_X*      
           
*BYTES_SRC_PORT_X*        
      
*BYTES_RTP_SRC_PORT_X*       
   
*PACKETS_RTP_SRC_PORT_X*       
 
*PACKETS_DST_PORT_X*  

*BYTES_DST_PORT_X*   

*BYTES_RTP_DST_PORT_X_PAYLOAD_Y* 

*PACKETS_RTP_DST_PORT_X_PAYLOAD_Y*


    

