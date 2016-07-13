#include <map>
#include <string>
#include <sstream>

#ifndef TBCALIB_h
#define TBCALIB_h

class TBCalibSource
{
private:
    std::map<std::string, double> qie8MIPCal_, qie11MIPCal_;

public:
    //MIP signal in fC for a 17 layer tower
    static constexpr double  QIE8MIP = 10.253070;
    static constexpr double QIE11MIP = 9586.7068;

    double getQIE8Corr(int ieta, int iphi)
    {
	std::stringstream ss;
	ss << ieta << "_" << iphi;
	auto ref = qie8MIPCal_.find(ss.str());
	if(ref != qie8MIPCal_.end())  return ref->second;
	else                          return 1.0;
    }

    double getQIE11Corr(int ieta, int iphi, int depth)
    {
	std::stringstream ss;
	ss << ieta << "_" << iphi << "_" << depth;
	auto ref = qie11MIPCal_.find(ss.str());
	if(ref != qie8MIPCal_.end())  return ref->second;
	else                          return 1.0;
    }

    TBCalibSource()
    {
	qie8MIPCal_["17_15"] = 0.8386631693;
	qie8MIPCal_["18_15"] = 0.8656163845;
	qie8MIPCal_["19_15"] = 0.9076405744;
	qie8MIPCal_["20_15"] = 0.9070865132;
	qie8MIPCal_["21_15"] = 0.921359338;
	qie8MIPCal_["23_15"] = 1.0788390943;
	qie8MIPCal_["18_16"] = 0.7964898899;
	qie8MIPCal_["22_16"] = 1.010822019;
	qie8MIPCal_["17_13"] = 1.2285541894;
	qie8MIPCal_["17_14"] = 0.8521227383;
	qie8MIPCal_["18_13"] = 1.308707765;
	qie8MIPCal_["18_14"] = 0.7791501816;
	qie8MIPCal_["19_13"] = 1.3456180095;
	qie8MIPCal_["19_14"] = 0.8177792566;
	qie8MIPCal_["20_13"] = 1.3973711376;
	qie8MIPCal_["20_14"] = 0.8749996573;
	qie8MIPCal_["21_13"] = 1.3523981039;
	qie8MIPCal_["21_14"] = 0.8298520461;
	qie8MIPCal_["22_13"] = 1.4904452531;
	qie8MIPCal_["23_14"] = 0.9171396484;
	qie8MIPCal_["24_13"] = 1.7950368502;
	qie8MIPCal_["25_14"] = 0.9356784589;


	qie11MIPCal_["16_5_2"] = 1.9441528184;
	qie11MIPCal_["16_6_2"] = 2.013751174 ;
	qie11MIPCal_["16_6_3"] = 0.4986163134;
	qie11MIPCal_["17_5_2"] = 3.2946313712;
	qie11MIPCal_["17_5_3"] = 0.7455221079;
	qie11MIPCal_["17_5_4"] = 0.7975740173;
	qie11MIPCal_["17_5_5"] = 1.6343407732;
	qie11MIPCal_["17_6_2"] = 3.1012657584;
	qie11MIPCal_["17_6_3"] = 0.767118548 ;
	qie11MIPCal_["17_6_4"] = 0.7791867065;
	qie11MIPCal_["17_6_5"] = 2.5021356715;
	qie11MIPCal_["18_5_1"] = 1.0687279689;
	qie11MIPCal_["18_5_2"] = 0.9971397878;
	qie11MIPCal_["18_5_3"] = 0.9562857385;
	qie11MIPCal_["18_5_4"] = 0.9104631842;
	qie11MIPCal_["18_5_5"] = 0.676192492 ;
	qie11MIPCal_["18_5_6"] = 1.3704110253;
	qie11MIPCal_["18_6_1"] = 1.0782201907;
	qie11MIPCal_["18_6_2"] = 0.9187264332;
	qie11MIPCal_["18_6_3"] = 0.9042764275;
	qie11MIPCal_["18_6_4"] = 0.7780273619;
	qie11MIPCal_["18_6_5"] = 0.6615254063;
	qie11MIPCal_["19_5_1"] = 0.9488795087;
	qie11MIPCal_["19_5_2"] = 0.8490424671;
	qie11MIPCal_["19_5_3"] = 0.9233850502;
	qie11MIPCal_["19_5_4"] = 0.8624361326;
	qie11MIPCal_["19_5_5"] = 0.7002144619;
	qie11MIPCal_["19_5_6"] = 0.8456338136;
	qie11MIPCal_["19_6_1"] = 1.0055412643;
	qie11MIPCal_["19_6_2"] = 0.8490941994;
	qie11MIPCal_["19_6_3"] = 0.8973002597;
	qie11MIPCal_["19_6_4"] = 0.7557852258;
	qie11MIPCal_["19_6_6"] = 0.9163459848;
	qie11MIPCal_["20_5_1"] = 0.9706126844;
	qie11MIPCal_["20_5_2"] = 1.0892253128;
	qie11MIPCal_["20_5_3"] = 1.0254588086;
	qie11MIPCal_["20_5_4"] = 0.8755609788;
	qie11MIPCal_["20_5_5"] = 0.7544687736;
	qie11MIPCal_["20_5_6"] = 0.9389818121;
	qie11MIPCal_["20_6_1"] = 1.1262515138;
	qie11MIPCal_["20_6_2"] = 0.9117652685;
	qie11MIPCal_["20_6_3"] = 0.8838815334;
	qie11MIPCal_["20_6_4"] = 0.9905582115;
	qie11MIPCal_["20_6_5"] = 0.7882680269;
	qie11MIPCal_["20_6_6"] = 0.9494836817;
	qie11MIPCal_["21_5_1"] = 1.2089083079;
	qie11MIPCal_["21_5_2"] = 0.9606358689;
	qie11MIPCal_["21_5_3"] = 0.9148805625;
	qie11MIPCal_["21_5_4"] = 0.7484087948;
	qie11MIPCal_["21_5_5"] = 0.7760019457;
	qie11MIPCal_["21_5_6"] = 0.9669993227;
	qie11MIPCal_["21_6_1"] = 1.0861103786;
	qie11MIPCal_["21_6_2"] = 0.9485471557;
	qie11MIPCal_["21_6_3"] = 0.9185183311;
	qie11MIPCal_["21_6_4"] = 0.8455059266;
	qie11MIPCal_["21_6_5"] = 0.828008267 ;
	qie11MIPCal_["21_6_6"] = 0.8576773742;
	qie11MIPCal_["22_6_1"] = 1.3381975259;
	qie11MIPCal_["22_6_2"] = 1.2108214617;
	qie11MIPCal_["22_6_3"] = 1.1605301718;
	qie11MIPCal_["22_6_4"] = 0.813782307 ;
	qie11MIPCal_["22_6_5"] = 0.6611133964;
	qie11MIPCal_["22_6_6"] = 1.276461222 ;
	qie11MIPCal_["23_5_1"] = 1.3225108848;
	qie11MIPCal_["23_5_2"] = 1.4438139941;
	qie11MIPCal_["23_5_3"] = 0.9960388538;
	qie11MIPCal_["23_5_4"] = 1.3676638977;
	qie11MIPCal_["23_5_5"] = 1.2775210106;
	qie11MIPCal_["23_5_6"] = 0.9945633163;
	qie11MIPCal_["23_5_7"] = 1.0997515208;
	qie11MIPCal_["24_6_1"] = 1.8797464378;
	qie11MIPCal_["24_6_2"] = 1.0577660987;
	qie11MIPCal_["24_6_3"] = 1.02666509 ;
	qie11MIPCal_["24_6_4"] = 1.0373154004;
	qie11MIPCal_["24_6_5"] = 1.2401377926;
	qie11MIPCal_["24_6_6"] = 1.6730267009;
	qie11MIPCal_["24_6_7"] = 2.2769115316;
	qie11MIPCal_["25_5_1"] = 1.2047151977;
	qie11MIPCal_["25_5_2"] = 1.3349337292;
	qie11MIPCal_["25_5_3"] = 0.9354988781;
	qie11MIPCal_["25_5_4"] = 1.1387557765;
	qie11MIPCal_["25_5_5"] = 1.0435177819;
	qie11MIPCal_["25_5_6"] = 1.1161135655;
	qie11MIPCal_["25_5_7"] = 1.838148524 ;

    }
};

#endif
