/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006,2007 INRIA
 * Copyright (c) 2013 Dalian University of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 * Author: Junling Bu <linlinjavaer@gmail.com>
 *
 */
/**
 * This example shows basic construction of an 802.11p node.  Two nodes
 * are constructed with 802.11p devices, and by default, one node sends a single
 * packet to another node (the number of packets and interval between
 * them can be configured by command-line arguments).  The example shows
 * typical usage of the helper classes for this mode of WiFi (where "OCB" refers
 * to "Outside the Context of a BSS")."
 */

#include "MySimpleVanet.h"
#include "ns3/core-module.h"



using namespace ns3;

MySimpleVanet::MySimpleVanet() {
	// TODO Auto-generated constructor stub

}

MySimpleVanet::~MySimpleVanet() {
	// TODO Auto-generated destructor stub
}


NS_LOG_COMPONENT_DEFINE ("MySimpleVanet");

int
main(int argc, char *argv[])
{
	using namespace std;

	//Define parameters and default values
	uint32_t numNodes = 2;
	uint32_t packetSize = 1000;
	uint32_t numPackets = 1;
	double interval = 1.0; //Seconds
	bool verbose = false;
	string phyMode("0fdmRate6MbpsBW10MHz");

	//Define command line arguments (May be expanded level)
	CommandLine cmd;
	cmd.AddValue("numNodes", "Number of nodes in the simulation", numNodes);
	cmd.AddValue("verbose", "Set verbose to print all logs to console", verbose);

	//Parse command line arguments
	cmd.Parse(argc, argv);

	//Create container for nodes and create nodes
	NodeContainer vanetNodes;
	vanetNodes.Create(numNodes);

	//Add and initialize helps to create wifi NICs

	YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
	YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
	Ptr<YansWifiChannel> channel = wifiChannel.Create();
	wifiPhy.SetChannel(channel);

	//Set pcap tracing
	wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);

	//Data-link layer
	NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default ();

	//802.11p Helper
	Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();
	if (verbose)
	{
		wifi80211p.EnableLogComponents ();      // Turn on all Wifi 802.11p logging
	}

	wifi80211p.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
			"DataMode",StringValue (phyMode),
			"ControlMode",StringValue (phyMode));

	NetDeviceContainer devices = wifi80211p.Install (wifiPhy, wifi80211pMac, vanetNodes);

	//Enable tracing on all devices
	wifiPhy.EnablePcap ("wave-simple-80211p", devices);

	//Mobility helpers
  MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                   "MinX", DoubleValue (0.0),
                                   "MinY", DoubleValue (0.0),
                                   "DeltaX", DoubleValue (5.0),
                                   "DeltaY", DoubleValue (10.0),
                                   "GridWidth", UintegerValue (3),
                                   "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                               "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));

  mobility.Install (vanetNodes);

  //Network layer
  InternetStackHelper INetStack;
  INetStack.Install(vanetNodes);


  //IP Address helpers
  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (devices);

}

