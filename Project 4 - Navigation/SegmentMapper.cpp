// TURN IN
#include "provided.h"
#include "MyMap.h"
#include <vector>
using namespace std;


class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
	MyMap<pair<double, double>, vector<StreetSegment>> m_map;
};


SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
	GeoCoord g;
	StreetSegment ss;
	pair<double, double> pair;
	for (int k = 0; k < ml.getNumSegments(); k++) {
		if (!ml.getSegment(k, ss)) // get the segment
			return;
		/////////////////////////////////// beginning coordinate //////////////////////////////////
		g = ss.segment.start;
		vector<StreetSegment> v1;
		pair.first = g.latitude; pair.second = g.longitude; // because GeoCoord has no comparison operator, but pair does
		if (m_map.find(pair) == nullptr) { //  coord has not been pushed to map before
			v1.push_back(ss);
			m_map.associate(pair, v1);
		}
		else // that coord was already in the map somewhere
			m_map.find(pair)->push_back(ss);

		//////////////////////////////// ending coordinate ////////////////////////////////////////
		g = ss.segment.end;
		vector<StreetSegment> v2;
		pair.first = g.latitude; pair.second = g.longitude; // because GeoCoord has no comparison operator, but pair does
		if (m_map.find(pair) == nullptr) { //  coord has not been pushed to map before
			v2.push_back(ss);
			m_map.associate(pair, v2);
		}
		else // that coord was already in the map somewhere
			m_map.find(pair)->push_back(ss);
			
		////////////////////////////// attractions /////////////////////////////////////////////////
		for (int j = 0; j < ss.attractions.size(); j++) {
			g = ss.attractions[j].geocoordinates;
			pair.first = g.latitude; pair.second = g.longitude;
			vector<StreetSegment> v3;
			if (m_map.find(pair) == nullptr) { // beginning coordinate of segment; coord has not been pushed to map before
				v3.push_back(ss);
				m_map.associate(pair, v3);
			}
			else // that coord was already in the map somewhere
				v3.push_back(ss);
		}
	}
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
	pair<double, double> pair;
	pair.first = gc.latitude; pair.second = gc.longitude;

	const vector<StreetSegment>* segments;
	segments = m_map.find(pair);
	vector<StreetSegment> vector;
	if (segments == nullptr)
		return vector;

	return *segments;  
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
