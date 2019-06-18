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
 #include "ns3/antenna-module.h"
 #include "ns3/netanim-module.h"
 #include "ns3/spectrum-wifi-phy.h"
#include "ns3/spectrum-helper.h"
using namespace ns3;



int
main( int argc, char *argv[] )
{

LogComponentEnable("SingleModelSpectrumChannel", LOG_LEVEL_ALL);
LogComponentEnable("RandomWalk2d", LOG_LEVEL_ALL);
LogComponentEnable("Application", LOG_LEVEL_ALL);
LogComponentEnable("WifiPhy", LOG_LEVEL_ALL);
LogComponentEnable("GlobalRouteManager", LOG_LEVEL_ALL);
    
    NodeContainer AdHocNode;
    AdHocNode.Create(3);
    YansWifiChannelHelper wifiChannel;
    SpectrumChannelHelper specchannel;
    Ptr<CosineAntennaModel> antenna=CreateObject<CosineAntennaModel> ();
    Ptr<SpectrumWifiPhy> spectrumWifiPhy=CreateObject<SpectrumWifiPhy> ();
    spectrumWifiPhy->SetAntenna(antenna);
    spectrumWifiPhy->SetChannel (specchannel.Create ());
    Ptr<WifiSpectrumPhyInterface> interface=CreateObject<WifiSpectrumPhyInterface>();
    interface->SetSpectrumWifiPhy (spectrumWifiPhy);

    SpectrumWifiPhyHelper spectrumPhy = SpectrumWifiPhyHelper::Default();
    Config::SetDefault( "ns3::WifiPhy::CcaMode1Threshold", DoubleValue( -62.0 ) );
    Ptr<MultiModelSpectrumChannel> spectrumChannel
        = CreateObject<MultiModelSpectrumChannel> ();
    Ptr<FriisPropagationLossModel> lossModel
        = CreateObject<FriisPropagationLossModel> ();
    //lossModel->SetFrequency( 10.180e9 );
    spectrumChannel->AddPropagationLossModel( lossModel );
    Ptr<ConstantSpeedPropagationDelayModel> delayModel
        = CreateObject<ConstantSpeedPropagationDelayModel> ();
    spectrumChannel->SetPropagationDelayModel( delayModel );
    spectrumChannel->AddRx(interface);
    spectrumPhy.SetChannel( spectrumChannel );
    spectrumPhy.SetErrorRateModel( "ns3::NistErrorRateModel" );
    //spectrumPhy.Set( "Frequency", UintegerValue( 10180 ) );
    //spectrumPhy.Set(" ")
    //Ptr<CosineAntennaModel> cosantenna=CreateObject<CosineAntennaModel>();
    //spectrumPhy.SetAntenna(cosantenna);
    WifiHelper wifi;
   wifi.SetStandard( WIFI_PHY_STANDARD_80211a );
    wifi.SetRemoteStationManager( "ns3::ConstantRateWifiManager", "DataMode", StringValue( "OfdmRate6Mbps" ) );
    NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
    mac.SetType( "ns3::AdhocWifiMac",
             "Slot", StringValue( "16us" ) );
    NetDeviceContainer AdHocDevices;
    AdHocDevices = wifi.Install( spectrumPhy, mac, AdHocNode );
    //NetDeviceContainer AdHocDevices2=wifi.Install( spectrumPhy, mac, AdHocNode );
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
    positionAlloc->Add (Vector (10, 10, 10));
    positionAlloc->Add (Vector (20, 10, 10));
    positionAlloc->Add (Vector (10, 20, 10));
    MobilityHelper mobility;
    mobility.SetPositionAllocator (positionAlloc);
    mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel");
    mobility.Install(AdHocNode);
    InternetStackHelper internet;
    /* has effect on the next Install () */
    internet.Install( AdHocNode );
    Ipv4AddressHelper address;
    address.SetBase( "195.1.1.0", "255.255.255.0" );
    Ipv4InterfaceContainer AdHocIp;
    AdHocIp = address.Assign( AdHocDevices );
    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    OnOffHelper onOff1( "ns3::UdpSocketFactory", Address( InetSocketAddress( AdHocIp.GetAddress( 1 ), 9 ) ) );
    Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (210));
    Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("448kb/s"));
    onOff1.SetAttribute( "OnTime", StringValue( "ns3::ConstantRandomVariable[Constant=0.1]" ) );
    onOff1.SetAttribute( "OffTime", StringValue( "ns3::ConstantRandomVariable[Constant=0.9]" ) );
    ApplicationContainer apps1 = onOff1.Install( AdHocNode.Get(0) );
    apps1.Start( Seconds( 1.0 ) );
    apps1.Stop( Seconds( 20.0 ) );
    //NS_LOG_INFO( "Create Applications." );
    //uint16_t port = 99;
    AnimationInterface anim( "first.xml" );
    Simulator::Stop( Seconds( 25.0 ) );
    Simulator::Run();

    Simulator::Destroy();
    NS_LOG_UNCOND("end app");
    return(0);
}