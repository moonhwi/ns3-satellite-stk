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

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DynamicGlobalRoutingExample");

int
main (int argc, char *argv[])
{
  // The below value configures the default behavior of global routing.
  // By default, it is disabled.  To respond to interface events, set to true
  //Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));

  // Allow the user to override any of the defaults and the above
  // Bind ()s at run-time, via command-line arguments
  CommandLine cmd;
  cmd.Parse (argc, argv);
  uint32_t nAdHoc=15;
  GlobalValue::Bind ("ChecksumEnabled", BooleanValue (true));
  LogComponentEnable("OlsrRoutingProtocol", LOG_LEVEL_ALL);
  //std::cin>>nAdHoc;
  //NS_LOG_UNCOND ("Create nodes.");
  NodeContainer c;
  c.Create (15);
 
  NodeContainer n0n1 = NodeContainer (c.Get (0), c.Get (1));
  NodeContainer n0n2 = NodeContainer (c.Get (0), c.Get (2));
  NodeContainer n1n3 = NodeContainer (c.Get (1), c.Get (3));
  NodeContainer n2n3 = NodeContainer (c.Get (2), c.Get (3));
  NodeContainer n3n4 = NodeContainer (c.Get (3), c.Get (4));
  NodeContainer n3n5 = NodeContainer (c.Get (3), c.Get (5));
  NodeContainer n4n6 = NodeContainer (c.Get (4), c.Get (6));
  NodeContainer n5n6 = NodeContainer (c.Get (5), c.Get (6));
  NodeContainer n6n7 = NodeContainer (c.Get (6), c.Get (7));
  NodeContainer n6n8 = NodeContainer (c.Get (6), c.Get (8));
  NodeContainer n7n9 = NodeContainer (c.Get (7), c.Get (9));
  NodeContainer n8n9 = NodeContainer (c.Get (8), c.Get (9));
  NodeContainer n9n10 = NodeContainer (c.Get (9), c.Get (10));
  NodeContainer n9n11 = NodeContainer (c.Get (9), c.Get (11));
  NodeContainer n10n12 = NodeContainer (c.Get (10), c.Get (12));
  NodeContainer n11n12 = NodeContainer (c.Get (11), c.Get (12));
  NodeContainer n12n13 = NodeContainer (c.Get (12), c.Get (13));
  NodeContainer n12n14 = NodeContainer (c.Get (12), c.Get (14));
  NodeContainer n13n0 = NodeContainer (c.Get (13), c.Get (0));
  NodeContainer n14n0 = NodeContainer (c.Get (14), c.Get (0));
  
  InternetStackHelper internet;
  OlsrHelper      olsr;
  Ipv4StaticRoutingHelper staticRouting;
  Ipv4ListRoutingHelper   list;
  list.Add( staticRouting, 0 );
  list.Add( olsr, 10 );
  //InternetStackHelper internet;
  internet.SetRoutingHelper( list );
  internet.Install (c);

  MobilityHelper mobility;
  mobility.SetMobilityModel( "ns3::GaussMarkovMobilityModel",
                  "Bounds", BoxValue( Box( -150000000, 150000000, -150000000, 150000000, -150000000, 150000000 ) ),
                  "TimeStep", TimeValue( Seconds( 5 ) ));
  mobility.Install(c);

  //std::fstream infile( "/mnt/hgfs/share/erw.txt" );
  //char initialposvector[256];
   for ( uint32_t initialnumber = 0; initialnumber < nAdHoc; initialnumber++ )
   {
       Ptr<GaussMarkovMobilityModel> mob = c.Get( initialnumber )->GetObject<GaussMarkovMobilityModel>();
       mob->Setnodenumber( nAdHoc );
   }
   //infile.close();}


  // We create the channels first without any IP addressing information
  NS_LOG_INFO ("Create channels.");
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
  NetDeviceContainer d0d1 = p2p.Install (n0n1);
  NetDeviceContainer d0d2 = p2p.Install (n0n2);
  NetDeviceContainer d1d3 = p2p.Install (n1n3);
  NetDeviceContainer d2d3 = p2p.Install (n2n3);
  NetDeviceContainer d3d4 = p2p.Install (n3n4);
  NetDeviceContainer d3d5 = p2p.Install (n3n5);
  NetDeviceContainer d4d6 = p2p.Install (n4n6);
  NetDeviceContainer d5d6 = p2p.Install (n5n6);
  NetDeviceContainer d6d7 = p2p.Install (n6n7);
  NetDeviceContainer d6d8 = p2p.Install (n6n8);
  NetDeviceContainer d7d9 = p2p.Install (n7n9);
  NetDeviceContainer d8d9 = p2p.Install (n8n9);
  NetDeviceContainer d9d10 = p2p.Install (n9n10);
  NetDeviceContainer d9d11 = p2p.Install (n9n11);
  NetDeviceContainer d10d12 = p2p.Install (n10n12);
  NetDeviceContainer d11d12 = p2p.Install (n11n12);
  NetDeviceContainer d12d13 = p2p.Install (n12n13);
  NetDeviceContainer d12d14 = p2p.Install (n12n14);
  NetDeviceContainer d13d0 = p2p.Install (n13n0);
  NetDeviceContainer d14d0 = p2p.Install (n14n0);

  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i1=ipv4.Assign (d0d1);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i2=ipv4.Assign (d0d2);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i3=ipv4.Assign (d1d3);
  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i3=ipv4.Assign (d2d3);
  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i4=ipv4.Assign (d3d4);
  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i5=ipv4.Assign (d3d5);
  ipv4.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer i4i6=ipv4.Assign (d4d6);
  ipv4.SetBase ("10.1.8.0", "255.255.255.0");
  Ipv4InterfaceContainer i5i6=ipv4.Assign (d5d6);
  ipv4.SetBase ("10.1.9.0", "255.255.255.0");
  Ipv4InterfaceContainer i6i7=ipv4.Assign (d6d7);
  ipv4.SetBase ("10.1.10.0", "255.255.255.0");
  Ipv4InterfaceContainer i7i9=ipv4.Assign (d7d9);

  ipv4.SetBase ("10.1.11.0", "255.255.255.0");
  Ipv4InterfaceContainer i8i9=ipv4.Assign (d8d9);
  ipv4.SetBase ("10.1.12.0", "255.255.255.0");
  Ipv4InterfaceContainer i9i10=ipv4.Assign (d9d10);
  ipv4.SetBase ("10.1.13.0", "255.255.255.0");
  Ipv4InterfaceContainer i9i11=ipv4.Assign (d9d11);
  ipv4.SetBase ("10.1.14.0", "255.255.255.0");
  Ipv4InterfaceContainer i10i12=ipv4.Assign (d10d12);
  ipv4.SetBase ("10.1.15.0", "255.255.255.0");
  Ipv4InterfaceContainer i11i12=ipv4.Assign (d11d12);
  ipv4.SetBase ("10.1.16.0", "255.255.255.0");
  Ipv4InterfaceContainer i12i13=ipv4.Assign (d12d13);
  ipv4.SetBase ("10.1.17.0", "255.255.255.0");
  Ipv4InterfaceContainer i12i14=ipv4.Assign (d12d14);
  ipv4.SetBase ("10.1.18.0", "255.255.255.0");
  Ipv4InterfaceContainer i13i0=ipv4.Assign (d13d0);
  ipv4.SetBase ("10.1.19.0", "255.255.255.0");
  Ipv4InterfaceContainer i14i0=ipv4.Assign (d14d0);

  // Create router nodes, initialize routing database and set up the routing
  // tables in the nodes.
  //Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Create the OnOff application to send UDP datagrams of size
  // 210 bytes at a rate of 448 Kb/s
  NS_LOG_INFO ("Create Applications.");
  uint16_t port = 9;   // Discard port (RFC 863)
  OnOffHelper onoff ("ns3::UdpSocketFactory",
                     InetSocketAddress (i0i1.GetAddress (0), port));
  onoff.SetConstantRate (DataRate ("2kbps"));
  onoff.SetAttribute ("PacketSize", UintegerValue (200));

  ApplicationContainer apps = onoff.Install (c.Get (7));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (60.0));

  AnimationInterface anim( "first.xml" );
  // Create an optional packet sink to receive these packets
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
  apps = sink.Install (c.Get (0));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (70.0));

  p2p.EnablePcapAll ("multisat");
  Ipv4GlobalRoutingHelper g;
  Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("multisat.routes", std::ios::out);
  g.PrintRoutingTableAllAt (Seconds (15), routingStream);

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop( Seconds( 60.0 ) );
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}
