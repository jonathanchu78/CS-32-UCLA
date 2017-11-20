// TURN IN
#include "provided.h"
#include "MyMap.h"
#include <string>
#include <set>
#include <queue>
#include <list>
#include <map>
#include <iostream>
using namespace std;

bool GeoCoordComp(const GeoCoord& coord1, const GeoCoord& coord2) {
	if (coord1.latitude == coord2.latitude && coord1.longitude == coord2.longitude)
		return true;
	return false;
}

bool GeoSegComp(const GeoSegment& seg1, const GeoSegment& seg2) {
	if (GeoCoordComp(seg1.end, seg2.end) && GeoCoordComp(seg1.start, seg2.start))
		return true;
	return false;
}



struct ANode {
	// STRUCT FUNCTIONS
	ANode() { // default constructor because was getting an error
		gDist = hDist = fDist = 0;
		parent = nullptr;
	}

	ANode(GeoSegment newSeg, string street, ANode* myParent, const GeoCoord& end) { // CONSTRUCTOR
		streetName = street;
		seg.end.latitude = newSeg.end.latitude; seg.end.longitude = newSeg.end.longitude;
		seg.end.latitudeText = newSeg.end.latitudeText; seg.end.longitudeText = newSeg.end.longitudeText;
		seg.start.latitude = newSeg.start.latitude; seg.start.longitude = newSeg.start.longitude;
		seg.start.latitudeText = newSeg.start.latitudeText; seg.start.longitudeText = newSeg.start.longitudeText;
		parent = myParent;
		// GDIST
		if (parent != nullptr)
			gDist = parent->gDist;
		else {
			gDist = 0;
		}
		gDist += distanceEarthMiles(newSeg.start, newSeg.end); 
		// HDIST
		hDist = distanceEarthMiles(newSeg.end, end);
		//FDIST
		fDist = gDist + hDist;
	}

	bool operator<(const ANode& node) const { // if the one thats passed in is less than this one return true
		if (node.hDist <  hDist) return true;
		else return false;
	}

	ANode& operator=(const ANode& node)  {
		gDist = node.gDist;
		hDist = node.hDist;
		fDist = node.fDist;
		seg.end.latitude = node.seg.end.latitude; seg.end.longitude = node.seg.end.longitude;
		seg.end.latitudeText = node.seg.end.latitudeText; seg.end.longitudeText = node.seg.end.longitudeText;
		seg.start.latitude = node.seg.start.latitude; seg.start.longitude = node.seg.start.longitude;
		seg.start.latitudeText = node.seg.start.latitudeText; seg.start.longitudeText = node.seg.start.longitudeText;
		streetName = node.streetName;
		parent = node.parent;
		return *this;
	}

	ANode(const ANode& node) { // copy constructor
		*this = node;
	}

	// STRUCT MEMBER VARIABLES
	double gDist; // distance thus far in total path
	double hDist; // distance to end
	double fDist; // g + h
	GeoSegment seg;
	ANode* parent;
	string streetName;
};

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const; 

private:
	// MEMBER VARIABLES
	MapLoader m_mapLoad;
	AttractionMapper m_attMap;
	SegmentMapper m_segMap;
};

NavigatorImpl::NavigatorImpl(){}

NavigatorImpl::~NavigatorImpl(){}

bool NavigatorImpl::loadMapData(string mapFile) {
	if (!m_mapLoad.load(mapFile))
		return false;
	m_attMap.init(m_mapLoad);
	m_segMap.init(m_mapLoad);
	return true;  
}

string getCompassDir(GeoSegment g) {
	double d = angleOfLine(g);
	if (d >= 0 && d <= 22.5)
		return "east";
	if (d > 22.5 && d <= 67.5)
		return "northeast";
	if (d > 67.5 && d <= 112.5)
		return "north";
	if (d > 112.5 && d <= 157.5)
		return "northwest";
	if (d > 157.5 && d <= 202.5)
		return "west";
	if (d > 202.5 && d <= 247.5)
		return "southwest";
	if (d > 247.5 && d <= 292.5)
		return "south";
	if (d > 292.5 && d <= 337.5)
		return "southeast";
	if (d > 337.5 && d <= 360)
		return "east";

}

string getDirection(GeoSegment g1, GeoSegment g2) {
	double d = angleBetween2Lines(g1, g2);
	if (d < 180)
		return "right";
	if (d > 180)
		return "left";
	
	return "FUCK";
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
	bool success = false;
	std::priority_queue<ANode> openList;
	MyMap<pair<double, double>, ANode> closedList;
	pair<double, double> pair;
	std::pair<double, double> tempPair;

	GeoCoord START, END;
	if (!m_attMap.getGeoCoord(start, START)) // START AND END are GeoCoords
		return NAV_BAD_SOURCE;
	if (!m_attMap.getGeoCoord(end, END))
		return NAV_BAD_DESTINATION;
	
	//1) LOOK FOR THE STARTING STREET SEGMENT AND CREATE 2 GEOSEGMENTS FROM IT
	vector<StreetSegment> startSegments = m_segMap.getSegments(START); // get all the segments associated with the start attraction 
	if (startSegments.size() == 0)
		return NAV_NO_ROUTE; // BULLSHIT

	GeoSegment geoSeg1(START, startSegments[0].segment.end); 
	GeoSegment geoSeg2(START, startSegments[0].segment.start); 
	openList.push(ANode(geoSeg1, startSegments[0].streetName, nullptr, END)); 
	openList.push(ANode(geoSeg2, startSegments[0].streetName, nullptr, END));

	//2) LOOK FOR THE ENDING STREET SEGMENT AND STORE IT
	vector<StreetSegment> endSegment = m_segMap.getSegments(END);
	GeoSegment TARGET = endSegment[0].segment;

	while (!openList.empty()) {
		ANode curNode = openList.top();
		GeoSegment tempSeg;
		tempSeg.start = curNode.seg.end; tempSeg.end = curNode.seg.start;
		if (GeoSegComp(curNode.seg, TARGET) || GeoSegComp(tempSeg, TARGET)) {// REACHED THE END
			success = true;
			break;
		}

		pair.first = curNode.seg.end.latitude; pair.second = curNode.seg.end.longitude;
		closedList.associate(pair, openList.top()); // push cur onto closed list 
		openList.pop(); // remove cur from openList

		vector<StreetSegment> successorsSS = m_segMap.getSegments(curNode.seg.end); //cerr << successorsSS.size() << endl; // get all the successors street segments
		vector<GeoSegment> successors; 
		for (int k = 0; k < successorsSS.size(); k++) // get the geosegments of the successors
			successors.push_back(successorsSS[k].segment);
		for (int k = 0; k < successors.size(); k++) { 

			pair.first = curNode.seg.end.latitude; pair.second = curNode.seg.end.longitude;

			ANode nodeSuccessor;
			if (successors[k].start.latitude == pair.first && successors[k].start.longitude == pair.second) {
				ANode nodeSuccessor1(successors[k], successorsSS[k].streetName, closedList.find(pair), END);
				nodeSuccessor = nodeSuccessor1;
			}
			else {
				tempPair.first = successors[k].start.latitude; tempPair.second = successors[k].start.longitude;
				successors[k].start.latitude = successors[k].end.latitude; 
				successors[k].start.longitude = successors[k].end.longitude;
				successors[k].end.latitude = tempPair.first;
				successors[k].end.longitude = tempPair.second;
				ANode nodeSuccessor1(successors[k], successorsSS[k].streetName, closedList.find(pair), END);
				nodeSuccessor = nodeSuccessor1;
			}

			
			// look for nodeSuccessor on closedList //////////////////////////////////////////////////
			pair.first = nodeSuccessor.seg.end.latitude; pair.second = nodeSuccessor.seg.end.longitude;
			if (closedList.find(pair) == nullptr) // if it passes, push onto openList
				openList.push(nodeSuccessor);
		} // end of for loop
	} // end of while loop

	if (!success)
		return NAV_NO_ROUTE;

	////////////// RETRACE THE STEPS /////////////////////////////////////////////////////////
	ANode retraceParent = openList.top();
	ANode lastParent;
	string currentStreet = retraceParent.streetName;
	list<NavSegment> navs;
	GeoSegment sss;
	sss.start = openList.top().seg.end;
	sss.end = END;
	NavSegment nnn(getCompassDir(sss), currentStreet, distanceEarthMiles(sss.start, sss.end), sss);
	navs.push_front(nnn);
	while (retraceParent.seg.start.latitude != START.latitude && retraceParent.seg.start.longitude != START.longitude) {
		lastParent = retraceParent;
		retraceParent = *retraceParent.parent;
		if (retraceParent.streetName != currentStreet) { // NAV STYLE SEGMENT
			NavSegment navv(getDirection(lastParent.seg, retraceParent.seg), currentStreet);
			currentStreet = retraceParent.streetName;
			navs.push_front(navv);
		}
		currentStreet = retraceParent.streetName;
		NavSegment nav(getCompassDir(retraceParent.seg), currentStreet, distanceEarthMiles(retraceParent.seg.start, 
																		retraceParent.seg.end), retraceParent.seg); 
		navs.push_front(nav);
	}

	while (!navs.empty()){
		directions.push_back(navs.front());
		navs.pop_front();
	}
	return NAV_SUCCESS;
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
