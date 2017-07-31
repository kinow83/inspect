
#ifndef _WIFI_CHANNEL_DEF_
#define _WIFI_CHANNEL_DEF_

#define MAX_NUM_2G_CHANNEL 14
// 1,2,3,4,5,6,7,8,9,10,11,12,13,14 - 14

#define MAX_NUM_5G_CHANNEL 42
// 36,40,44,48,52,56,60,64,100,104,108,112,116,120,124,128,132,136,140,149,153,157,161,165 - 24
//     ,37,38,39                                                                                                                                     - 3
//          ,41,42,43                                                                                                                                - 3
//               ,45,46,47,                                                                                                                          - 3
//                         ,53,54,55                                                                                                                 - 3
//                               ,57,58,59,                                                                                                          - 3
//                                                                                                                             ,150,152              - 2
//                                                                                                                                           ,160       - 1
#define MAX_NUM_CHANNEL (MAX_NUM_2G_CHANNEL+MAX_NUM_5G_CHANNEL)

struct wif_ch
{
	unsigned char ch;
	unsigned short freq;
	unsigned char available;
};

#endif
