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
1. Read the developer manual (A direct link is at the bottom of the page).
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

The following example filter udp and tcp traffic parallely with a 100ms rate

TODO: The config.ini:
TODO: The script

**Available Features**
TODO: List the available features

