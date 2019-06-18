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
 #include "ns3/netanim-module.h"
using namespace ns3;
int
main( int argc, char *argv[] )
{
/*
 * uint32_t nAdHoc = 10;
 * CommandLine cmd;
 * cmd.AddValue( "nAdHoc", "Number of wifi ad devices", nAdHoc );
 * cmd.Parse( argc, argv );*/
    NodeContainer AdHocNode;
    AdHocNode.Create( 15 );
    
    LogComponentEnable("RandomWalk2d", LOG_LEVEL_ALL);
    SpectrumWifiPhyHelper spectrumPhy = SpectrumWifiPhyHelper::Default();
    Config::SetDefault( "ns3::WifiPhy::CcaMode1Threshold", DoubleValue( -62.0 ) );
    std::cout<<"djdjd";
    Ptr<MultiModelSpectrumChannel> spectrumChannel
        = CreateObject<MultiModelSpectrumChannel> ();
    Ptr<FriisPropagationLossModel> lossModel
        = CreateObject<FriisPropagationLossModel> ();
       /* std::vector<int> ilist = {1,1,3,4,5,6,7};
        std::vector<std::vector<int> > q;
        q.push_back(ilist);
        ilist = {0,2,2,3,4,5,6,7};
        q.push_back(ilist);
        ilist = {1,1,2,3,4,5,6,7};
        q.push_back(ilist);
        q.push_back(ilist);
    lossModel->SetAssNode(q);*/
    std::cout<<"djdjd";
    lossModel->SetFrequency( 10.180e9 );
    spectrumChannel->AddPropagationLossModel( lossModel );
    Ptr<ConstantSpeedPropagationDelayModel> delayModel
        = CreateObject<ConstantSpeedPropagationDelayModel> ();
    spectrumChannel->SetPropagationDelayModel( delayModel );
    spectrumPhy.SetChannel( spectrumChannel );
    spectrumPhy.SetErrorRateModel( "ns3::NistErrorRateModel" );
    spectrumPhy.Set( "Frequency", UintegerValue( 10180 ) );
    spectrumPhy.Set( "TxPowerStart", DoubleValue( 5 ) );
    spectrumPhy.Set( "TxGain", DoubleValue( 45 ) );
    spectrumPhy.Set( "RxGain", DoubleValue( 45 ) );
    spectrumPhy.Set( "TxPowerEnd", DoubleValue( 5 ) );
    WifiHelper wifi;
    wifi.SetStandard( WIFI_PHY_STANDARD_80211a );
    wifi.SetRemoteStationManager( "ns3::ConstantRateWifiManager", "DataMode", StringValue( "OfdmRate6Mbps" ) );
    NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
    mac.SetType( "ns3::AdhocWifiMac",
             "Slot", StringValue( "16us" ) );
    NetDeviceContainer AdHocDevices;
    AdHocDevices = wifi.Install( spectrumPhy, mac, AdHocNode );

 
    MobilityHelper mobility;

    std::cout<<"djdjd";
  mobility.SetMobilityModel( "ns3::GaussMarkovMobilityModel",
                  "Bounds", BoxValue( Box( -150000000, 150000000, -150000000, 150000000, -150000000, 150000000 ) ),
                  "TimeStep", TimeValue( Seconds( 5 ) ));
    mobility.Install(AdHocNode);
    OlsrHelper      olsr;
    Ipv4StaticRoutingHelper staticRouting;
    Ipv4ListRoutingHelper   list;
    list.Add( staticRouting, 0 );
    list.Add( olsr, 10 );
    InternetStackHelper internet;
    internet.SetRoutingHelper( list );
    /* has effect on the next Install () */
    internet.Install( AdHocNode );
    Ipv4AddressHelper address;
    address.SetBase( "195.1.1.0", "255.255.255.0" );
    Ipv4InterfaceContainer AdHocIp;
    AdHocIp = address.Assign( AdHocDevices );
    //NS_LOG_INFO( "Create Applications." );
    //uint16_t port = 99;
    OnOffHelper onOff1( "ns3::UdpSocketFactory", Address( InetSocketAddress( AdHocIp.GetAddress( 0 ), 9 ) ) );
    Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (210));
    Config::SetDefault ("ns3::OnOffApplication::DataRate", StringValue ("448kb/s"));


    onOff1.SetAttribute( "OnTime", StringValue( "ns3::ConstantRandomVariable[Constant=0.1]" ) );
    onOff1.SetAttribute( "OffTime", StringValue( "ns3::ConstantRandomVariable[Constant=0.9]" ) );
    ApplicationContainer apps1 = onOff1.Install( AdHocNode.Get(2) );
    apps1.Start( Seconds( 1.0 ) );
    apps1.Stop( Seconds( 20.0 ) );


    ApplicationContainer apps2=onOff1.Install(AdHocNode.Get(1));
    apps2.Start( Seconds( 1.0 ) );
    apps2.Stop( Seconds( 20.0 ) );

    PacketSinkHelper sink ("ns3::UdpSocketFactory",
    Address( InetSocketAddress( AdHocIp.GetAddress( 0 ), 9)));
    ApplicationContainer apps= sink.Install (AdHocNode);
    apps.Start (Seconds (1.1));
    apps.Stop (Seconds (20.0));

    AnimationInterface anim( "first.xml" );
    Simulator::Stop( Seconds( 25.0 ) );
    Simulator::Run();
    Simulator::Destroy();
    return(0);
}
