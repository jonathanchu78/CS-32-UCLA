// TURN IN
#include "provided.h"
#include "MyMap.h"
#include <string>
#include <iostream>
#include <iomanip>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
	MyMap<string, GeoCoord> m_map;
};


AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
	StreetSegment ss;
	string s;
	int numSegments = ml.getNumSegments();
	for (int k = 0; k < numSegments; k++) {
		if (!ml.getSegment(k, ss))
			return;
		for (int j = 0; j < ss.attractions.size(); j++) {
			s = ss.attractions[j].name; //cerr << s << endl;
			for (int l = 0; l < s.size(); l++)
				s[l] = tolower(s[l]);
			m_map.associate(s, ss.attractions[j].geocoordinates); //cerr << s << endl;
		}
	}

}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
	for (int k = 0; k < attraction.size(); k++)
		attraction[k] = tolower(attraction[k]);

	const GeoCoord* geo = m_map.find(attraction);
	if (geo == nullptr)
		return false;

	gc = *geo;
	return true;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
