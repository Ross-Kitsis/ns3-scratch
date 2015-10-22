/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 */

#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/core-module.h"

#include "ns3/wave-mac-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wifi-80211p-helper.h"

#include "ns3/mobility-model.h"
#include "ns3/mobility-helper.h"

using namespace ns3;
using namespace std;

void CourseChange(string context, Ptr<const MobilityModel> model)
{
	Vector position = model->GetPosition();
	NS_LOG_UNCOND(context << " x = " << position.x << " , y = " << position.y);
}

NS_LOG_COMPONENT_DEFINE("Vanet2");
int
main(int argc, char *argv[])
{
	uint32_t numNodes = 2; //Number of nodes (Minimum 2)
	bool verbose =  false;
	std::string phyMode("OfdmRate6MbpsBW10MHz");


	CommandLine cmd;




	NodeContainer vanetNodes;
	vanetNodes.Create(numNodes);


	//Setup Layer 1 channel
	YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default();
	YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
	Ptr<YansWifiChannel> channel = wifiChannel.Create ();
	wifiPhy.SetChannel (channel);

	//Setup Pcap Tracing
	wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);

	//Setup layer 2
	NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default ();
	Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();
	if (verbose)
	{
		wifi80211p.EnableLogComponents ();      // Turn on all Wifi 802.11p logging
	}

	wifi80211p.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
			"DataMode",StringValue (phyMode),
			"ControlMode",StringValue (phyMode));
	NetDeviceContainer devices = wifi80211p.Install (wifiPhy, wifi80211pMac, vanetNodes);

	//Enable PCAP tracing
	wifiPhy.EnablePcap ("Vanet2", devices);

	// Enable mobility
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (5.0, 0.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (vanetNodes);

  //Setup Layer 3
  InternetStackHelper internet;
  internet.Install (vanetNodes);

  //Setup IP Addressing
  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (devices);


  Simulator::Run ();
  Simulator::Destroy ();
}
