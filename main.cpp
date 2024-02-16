#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<chrono>
#include<utility>
#include"jsoncpp.cpp"
#include<windows.h>
#define KEY_DOWN(key) ((GetAsyncKeyState(key)&0x8000)?1:0)
std::vector<double> noteTime;
std::vector<int> midrId;
std::vector<std::tuple<double,int,bool>>keyEvents;
Json::Value chart,parsedChart;
const char letterToDirection[25]="RpJEToUqGQHWLxNZFVDYBCMA";
const char usedKeys[]="𝑨𝑫𝑨𝑵𝑪𝑬𝑶𝑭𝑭𝑰𝑹𝑬𝑨𝑵𝑫𝑰𝑪𝑬";
// D:\adofai\TestLevel\level.adofai
// D:\adofai\Plum - Siknhole\Plum - Sinkhole.adofai
// D:\adofai\ExtratoneTest\Extra Credit on The Extratone Test! VICTORY ROYALE!!! (Stan Schwank)\level.adofai

namespace basicMaths{
	double fmod(double a,double b){
		double t=std::floor(a/b);
		return a-b*t;
	}
	double angleToTime(double angle,double bpm){
		return (angle/180)*(60/bpm)*1000;
	}
}

void ChronoSleep(double mil){
	auto startTime=std::chrono::steady_clock().now();
	while(true){
		auto curTime=std::chrono::steady_clock().now();
		double dura=std::chrono::duration<double>(curTime-startTime).count();
		if(dura*1000>=mil){
			break;
		}
	}
}

int main(){
	std::cout<<"Input Chart Path\n";
	std::string chartPath;
	std::getline(std::cin,chartPath);
	std::ifstream chartIn(chartPath);
	chartIn>>chart;
	
	try{
		const std::string& pathData=chart["pathData"].asString();
		for(unsigned i=0;i<pathData.size();i++){
			if(pathData[i]=='!'){
				chart["angleData"][i]=999.0;
			}else{
				for(int j=0;j<24;j++){
					if(pathData[i]==letterToDirection[j]){
						chart["angleData"][i]=j*15.0;
						break;
					}
				}
			}
		}
	}catch(int i){};
	
	if(true){ //parsing chart
		unsigned midrCount=0;
		for(unsigned i=0;i<chart["angleData"].size();i++){
			if(std::abs(chart["angleData"][i].asDouble()-999)<=0.01){
				++midrCount;
				parsedChart[i-midrCount]["midr"]="true";
				midrId.push_back(i-1);
			}else{
				parsedChart[i-midrCount]["angle"]=basicMaths::fmod(chart["angleData"][i].asDouble(), 360);
				parsedChart[i-midrCount]["midr"]="false";
				parsedChart[i-midrCount]["direction"]=0;
				parsedChart[i-midrCount]["bpm"]="unSet";
				parsedChart[i-midrCount]["extraHold"]=0;
			}
		}
		double curBpm=chart["settings"]["bpm"].asDouble();
		for(const Json::Value& attr: chart["actions"]){
			int tileId=attr["floor"].asInt();
			tileId-=(std::upper_bound(midrId.begin(),midrId.end(),tileId)-midrId.begin());
			if(attr["eventType"].asString()=="SetSpeed"){
				if(attr["speedType"].asString()=="Multiplier"){
					parsedChart[tileId]["bpm"]=curBpm=curBpm*attr["bpmMultiplier"].asDouble();
				}else{
					parsedChart[tileId]["bpm"]=curBpm=attr["beatsPerMinute"].asDouble();
				}
			}else if(attr["eventType"].asString()=="Twirl"){
				parsedChart[tileId]["direction"]=-1;
			}else if(attr["eventType"].asString()=="Hold"){
				parsedChart[tileId]["extraHold"]=attr["duration"].asDouble();
			}else if(attr["eventType"].asString()=="Pause"){
				parsedChart[tileId]["extraHold"]=attr["duration"].asDouble()/2;
			}
		}
		curBpm=chart["settings"]["bpm"].asDouble();
		int direction=1; 
		for(unsigned i=0;i<parsedChart.size();i++){
			if(parsedChart[i]["direction"].asInt()==-1){
				direction=-direction;
			}
			parsedChart[i]["direction"]=direction;
			if(parsedChart[i]["bpm"].asString()=="unSet"){
				parsedChart[i]["bpm"]=curBpm;
			}else{
				curBpm=parsedChart[i]["bpm"].asDouble();
			}
		}
	}
	
	double OFFSET;std::cin>>OFFSET;
	
	if(true){//calculate note time
		double curAngle=0,
		curTime=chart["settings"]["offset"].asDouble()
			+basicMaths::angleToTime(
				chart["settings"]["countdownTicks"].asDouble()*180-180,
				chart["settings"]["bpm"].asDouble()
			)+600+OFFSET;
		double curBpm=chart["settings"]["bpm"].asDouble();
		for(const Json::Value& tile: parsedChart){
			curAngle=basicMaths::fmod(curAngle-180,360);
			curBpm=tile["bpm"].asDouble();
			double destAngle=tile["angle"].asDouble();
			double pAngle=0;
			if(std::abs(destAngle-curAngle)<=0.001){
				pAngle=360;
			}else{
				pAngle=basicMaths::fmod((curAngle-destAngle)*tile["direction"].asInt(),360);
			}
			pAngle+=tile["extraHold"].asDouble()*360;
//			std::cout<<"pAngle="<<pAngle<<std::endl;
			curTime+=basicMaths::angleToTime(pAngle,curBpm);
			curAngle=destAngle;
			if(tile["midr"]=="true"){
				curAngle=curAngle+180;
			}
			noteTime.push_back(curTime);
		}
	}
	
	Json::StyledWriter sw;
//	std::cout<<sw.write(parsedChart);
	for(auto t:noteTime){
		std::cout<<t<<" ";
	}
	
	const int totKeyCount=strlen(usedKeys);
	for(unsigned i=0;i<noteTime.size();i++){
		keyEvents.push_back(std::tuple<double,int,bool>{noteTime[i],usedKeys[i%totKeyCount],true});
		keyEvents.push_back(std::tuple<double,int,bool>{noteTime[i]+30,usedKeys[i%totKeyCount],false});
	}
	std::sort(keyEvents.begin(),keyEvents.end());
	
	while(!KEY_DOWN(VK_HELP)) { ; }//VK_SPACE
	while(!KEY_DOWN('∫')){;}
	auto startTime=std::chrono::steady_clock().now();
	unsigned eventNumber=1;
	while(eventNumber<keyEvents.size()){
		auto curTime=std::chrono::steady_clock().now();
		auto timeMilliseconds=std::chrono::duration<double>(curTime-startTime).count()*1000+std::get<0>(keyEvents[0]);
		while(eventNumber<keyEvents.size()&&std::get<0>(keyEvents[eventNumber])<=timeMilliseconds){
			keybd_event(std::get<1>(keyEvents[eventNumber]),0,
				std::get<2>(keyEvents[eventNumber])?0:KEYEVENTF_KEYUP
			,0);
			eventNumber++;
			std::cout<<eventNumber<<" time: "<<timeMilliseconds<<std::endl;
		}
		ChronoSleep(1);
	}
	return 0;
}
