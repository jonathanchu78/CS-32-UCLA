// TURN IN
#include "provided.h"
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <cmath>
using namespace std;



class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
	vector<StreetSegment*> m_segments;
	int m_numSegments = 0;
};

MapLoaderImpl::MapLoaderImpl()
{
}

MapLoaderImpl::~MapLoaderImpl()
{
	for (int k = 0; k < m_segments.size(); k++) { // DO I NEED??
		delete m_segments[k];
	}
}

bool MapLoaderImpl::load(string mapFile)
{
	string s;

	string strs[4] = { "", "", "", "" };
	string coords[2] = { "", "" };
	int count = 0;
	int indicator;

	int p;
	int o;
	string str = "";
	int counter = -1;
	ifstream infile(mapFile);  // infile is a name of our choosing
	if (!infile)		        // Did opening the file fail?
	{
		cerr << "Error: Cannot open file!" << endl;
		return false;
	}
	int ddf = 0;
	while (getline(infile, s)) {
		counter++;
		/////////////////////// get and set the name of the segment ///////////////////////////////
		ddf++;
		m_segments.push_back(new StreetSegment);
		m_segments[counter]->streetName = s; m_numSegments++;
		//////////////////////// now to set the coords of the segment ///////////////////////////////
		getline(infile, s);
		for (int k = 0; k < s.size(); k++) {
			if (s[k] != ' ' && s[k] != ',') {
				indicator = 0;
				strs[count] += s[k];
			}
			else if ((s[k] == ' ' || s[k] == ',') && indicator == 0) {
				count++;
				indicator++;
			}
		} count = 0; 


		m_segments[counter]->segment.start.latitudeText = strs[0]; //cerr << strs[0] << ' ' << strs[1] << endl;
		m_segments[counter]->segment.start.longitudeText = strs[1];
		m_segments[counter]->segment.end.latitudeText = strs[2]; 
		m_segments[counter]->segment.end.longitudeText = strs[3];

		m_segments[counter]->segment.start.latitude = stod(strs[0]);
		m_segments[counter]->segment.start.longitude = stod(strs[1]);
		m_segments[counter]->segment.end.latitude = stod(strs[2]);
		m_segments[counter]->segment.end.longitude = stod(strs[3]);
		for (int k = 0; k < 4; k++) strs[k] = "";

		////////////////////// attractions /////////////////////////////////////////
		s = "";
		str = "";
		getline(infile, s);
		p = stod(s);

		if (!infile)
			return false; 
		if (p == 0) // there are no attractions, so skip 
			continue;

		char c = ' ';
		for (int k = 0; k < p; k++) { // now process each line of attractions
			str = "";
			s = "";
			getline(infile, s); 
			if (!infile)
				return false;
			for (int j = 0; j < s.size(); j++) {
				if (s[j] == '|')
					break;
				str += s[j];
				o = j;
			}
			m_segments[counter]->attractions.push_back(*(new Attraction)); // push the attraction
			m_segments[counter]->attractions[k].name = str; // set attraction name 

			coords[0] = ""; coords[1] = "";
			for (int k = o + 2; k < s.size(); k++) {
				if (s[k] != ' ' && s[k] != ',') {
					indicator = 0;
					coords[count] += s[k];
				}
				else if ((s[k] == ' ' || s[k] == ',') && indicator == 0) {
					count++;
					indicator++;
				}
			} count = 0;

			m_segments[counter]->attractions[k].geocoordinates.latitudeText = coords[0]; // set attr latitude text
			m_segments[counter]->attractions[k].geocoordinates.longitudeText = coords[1]; // set attr longitude text

			m_segments[counter]->attractions[k].geocoordinates.latitude = stod(coords[0]); // set attr latitude
			m_segments[counter]->attractions[k].geocoordinates.longitude = stod(coords[1]); // set attr longitude
		}
	}
	/*cerr << counter << endl;
	cerr << ddf << endl;;
	cout << "Map loaded." << endl;
	for (int i = 0; i < 1000; ++i)
	{
		cout << m_segments[i]->streetName << endl;
		cout << m_segments[i]->segment.start.latitude << ", ";
		cout << m_segments[i]->segment.start.longitude << endl;
		cout << m_segments[i]->segment.end.latitude << ", ";
		cout << m_segments[i]->segment.end.longitude << endl;
		for (int j = 0; j < m_segments[i]->attractions.size(); ++j)
		{
			cout << m_segments[i]->attractions[j].name << endl;
		}
		cout << "------------------------" << endl;
	}*/
	return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
	return m_numSegments;
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
	if (segNum < 0 || segNum >= m_numSegments)
		return false;

	seg = *(m_segments[segNum]);
	return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
