NTRT: Network Traffic Rate Tracker
===

## What is it?  

Network Traffic Rate Tracker (NTRT) is yet another network
analyzing and monitoring tool for network interfaces.
It analyze packets being sent through the selected interface,
and tracks an accumulative statistics saving the observation
into a csv file in a regular interval.


## What is it good for?  

It is recommended to use for collecting measurements
of network traffics on specified interface(s). For an instance
we can capture how many UDP bytes goes through a network device
in one seconds by sampling it in every 100ms. NTRT accumulates the
UDP byte traffic over the last one second and record the
measurements into a csv file in 100ms resolution.


## What type of network traffic can be tracked?  

*For UDP/TCP traffic:* You can track the number of packets and the
accumulated size of bytes for UDP/TCP traffic. You can filter it by
restricting the source or destination port. You can track the number of parallel TCP flows either.  

*For RTP traffic:* You can track the number of packets and the
accumulated size of bytes for specified payload type on a source destination port you defined.
It is also implemented as features that lost packets and frames are tracked.    
_Note_: You are obligated to ensure only RTP traffic goes through on the selected port.    
_Note 2_: Lost frames and packets are based on a fact that sequence is not reordered. You should NOT rely
on this metric if high jitter is observed.   

## How can I configure NTRT to listen a network device?

All it comes from a config file obligatory for NTRT to work. In config file
you can define the interfaces you would like to listen and the list of features
you want to keep track on the selected devices. See the example config files later.


## Requirements

NTRT works under linux using *automake* and *libpcap*.  

 ```
sudo apt-get install libpcap-dev  
 ```

## Install

```
 git clone http://github.com/balazskreith/ntrt && \  
 cd ntrt && \
 ./confgiure && \
 autoreconf -vfi && \
 make  && \
 sudo make install  \
 ```

## Quick Start

**Example of a config file**

The following example filters UDP and TCP traffic parallely and
save the measurements of 1s accumulation in every 100ms into a  ```wlan0_stats.csv```.


__config.ini__:

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

We call the observed attribute of the packets as **features**.
The ```feature_num``` defines the number of features we observed starting from 0.
All of the features observed must follow the naming convention ```feature_[INDEX]```.

**Run ntrt**:

```
sudo ntrt -cconfig.ini  
```


## Available Features

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


## Add a feature.

To add your specific feature please look at the ```lib/lib_feature.[h,c]```, and ```sys/sys_confs.[h,c]```.
NTRT uses the [devclego](https://github.com/balazskreith/devclego) framework.

## Bugs and feature requests

All bugs and comments are welcome, please create an issue for each.

## Contributing

Contact the author.

## Authors

- [Balazs Kreith](http://balazs.kreith.hu)

## Versioning

In the ntrt.c there is a macro called VERSION.

# 8. License

[Apache 2.0 License](LICENSE.md) - &copy; Balazs Kreith
