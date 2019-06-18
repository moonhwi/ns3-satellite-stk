#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/olsr-routing-protocol.h"
#include "ns3/olsr-helper.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include "ns3/olsr-helper.h"
#include <ns3/mobility-module.h>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
 #include "ns3/netanim-module.h"
 #include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/olsr-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/spectrum-module.h"
#include "ns3/stats-module.h"
#include "ns3/pcap-file-wrapper.h"
#include "ns3/flow-monitor-helper.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace ns3;
int main (int argc, char *argv[])
{
std::string phyMode ("DsssRate1Mbps");

CommandLine cmd;
  cmd.Parse (argc, argv);
  // Convert to time object
GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));
LogComponentEnable("OlsrRoutingProtocol", LOG_LEVEL_ALL);
  // disable fragmentation for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
  // turn off RTS/CTS for frames below 2200 bytes
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
  // Fix non-unicast data rate to be the same as that of unicast
  Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode",
                      StringValue (phyMode));

  NodeContainer olsrNodes;
  olsrNodes.Create (2);

  NodeContainer csmaNodes;
  csmaNodes.Create (1);

  // The below set of helpers will help us to put together the wifi NICs we want
  WifiHelper wifi;
  wifi.SetStandard (WIFI_PHY_STANDARD_80211b);

  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  // This is one parameter that matters when using FixedRssLossModel
  // set it to zero; otherwise, gain will be added
  wifiPhy.Set ("RxGain", DoubleValue (0) );
  // ns-3 supports RadioTap and Prism tracing extensions for 802.11b


  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
  // The below FixedRssLossModel will cause the rss to be fixed regardless
  // of the distance between the two stations, and the transmit power
  double rss = -80;  // -dBm
  wifiChannel.AddPropagationLoss ("ns3::FixedRssLossModel","Rss",DoubleValue (rss));
  wifiPhy.SetChannel (wifiChannel.Create ());

  // Add a mac and disable rate control
  WifiMacHelper wifiMac;
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                "DataMode",StringValue (phyMode),
                                "ControlMode",StringValue (phyMode));
  // Set it to adhoc mode
  wifiMac.SetType ("ns3::AdhocWifiMac");
  NetDeviceContainer devices = wifi.Install (wifiPhy, wifiMac, olsrNodes);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
  NetDeviceContainer csmaDevices = csma.Install (NodeContainer (csmaNodes.Get (0), olsrNodes.Get (1)));

  // Note that with FixedRssLossModel, the positions below are not
  // used for received signal strength.
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  positionAlloc->Add (Vector (5.0, 0.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (olsrNodes);

  OlsrHelper olsr;

  // Specify Node B's csma device as a non-OLSR device.
  olsr.ExcludeInterface (olsrNodes.Get (1), 2);

  Ipv4StaticRoutingHelper staticRouting;

  Ipv4ListRoutingHelper list;
  list.Add (staticRouting, 0);
  list.Add (olsr, 10);

  InternetStackHelper internet_olsr;
  internet_olsr.SetRoutingHelper (list); // has effect on the next Install ()
  internet_olsr.Install (olsrNodes);

  InternetStackHelper internet_csma;
  internet_csma.Install (csmaNodes);

  Ipv4AddressHelper ipv4;
  //NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i1 =ipv4.Assign (devices);

  ipv4.SetBase ("172.16.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i2=ipv4.Assign (csmaDevices);
  
  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     InetSocketAddress (i1i2.GetAddress (0), 9));
  onoff.SetConstantRate (DataRate ("2kbps"));
  onoff.SetAttribute ("PacketSize", UintegerValue (200));

  ApplicationContainer apps = onoff.Install (olsrNodes.Get (0));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (30.0));
  Ptr<Ipv4> stack = olsrNodes.Get (1)->GetObject<Ipv4> ();
  Ptr<Ipv4RoutingProtocol> rp_Gw = (stack->GetRoutingProtocol ());
  Ptr<Ipv4ListRouting> lrp_Gw = DynamicCast<Ipv4ListRouting> (rp_Gw);

 Ptr<olsr::RoutingProtocol> olsrrp_Gw;
  for (uint32_t i = 0; i < lrp_Gw->GetNRoutingProtocols ();  i++)
    {
      int16_t priority;
      Ptr<Ipv4RoutingProtocol> temp = lrp_Gw->GetRoutingProtocol (i, priority);
      if (DynamicCast<olsr::RoutingProtocol> (temp))
        {
          olsrrp_Gw = DynamicCast<olsr::RoutingProtocol> (temp);
        }
    }
  olsrrp_Gw->AddHostNetworkAssociation (Ipv4Address ("172.16.1.0"), Ipv4Mask ("255.255.255.0"));
  AnimationInterface anim( "first.xml" );
  // Create an optional packet sink to receive these packets
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), 9)));
  apps = sink.Install (csmaNodes.Get (0));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (70.0));
  wifiPhy.EnablePcap ("olsr-hna", devices);
  csma.EnablePcap ("olsr-hna", csmaDevices, false);
Simulator::Stop (Seconds (20.0));
  Simulator::Run ();
  Simulator::Destroy ();
  
  }