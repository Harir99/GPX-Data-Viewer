/**
 * Library that consist of functions that aid in parsing gpx files
 * @author Harir Al-Rubaye (1108085)
 * @since 08-02-2021
 */
#ifndef GPX_PARSER2_H
#define GPX_PARSER2_H
#define LIBXML_SCHEMAS_ENABLED
#include <math.h>


#define PI 3.141592653589793
#define VALID 0

// helper functions to do the initialization
GPXdoc* initializeGPXdoc();
Waypoint* initializeWaypoint();
Track* initializeTrack();
TrackSegment* initializeTrackSegment();
Route* initializeRoute();

// helper functions for handling and parsing 
int extractGPX(xmlNode*a_node, GPXdoc* tmpdoc); 
GPXData* extractGPXData(xmlNode*a_node);
Waypoint* extractWaypoint(xmlNode*a_node);
TrackSegment* extractTrackSegment(xmlNode*a_node);
Track* extractTrack(xmlNode*a_node);
Route* extractRoute(xmlNode*a_node);

// helper functions to compare names in structs 
bool compareWpt(const void *first, const void *second);
bool compareTrk(const void *first, const void *second);
bool compareRte(const void *first, const void *second);

// other helper functions
int getNumOfOtherData(const GPXdoc* doc);
int isText(char* name);
int roundInt(int n);
float getDistance(float startLat, float startLon, float endLat, float endLon);
void dummyDelete();


xmlNode* GPXdata2xmlNode(GPXData* data,char* namespace,bool getNamespace);
xmlNode* Name2xmlNode(char* data,char* namespace,bool getNamespace);
xmlNode* waypoints2xmlNode(Waypoint* data,char* type,char* namespace,bool getNamespace);
xmlNode* Route2xmlNode(Route* data,char* namespace,bool getNamespace);
xmlNode* TrackSegment2xmlNode(TrackSegment* data,char* namespace,bool getNamespace);
xmlNode* Track2xmlNode(Track* data,char* namespace,bool getNamespace);
xmlNode* GPXDoc2xmlNode(GPXdoc* data,bool getNamespace);

int  checkGPXData(GPXData *data);
int checkWayPoint(Waypoint *data);
int checkGPXdoc(GPXdoc *data);
int checkRoute(Route *data);
int checkTrack(Track *data);
int checkTrackSeg(TrackSegment *data);
char *strip(char *string, char *toRemove);
int getIndex(char* string, char c);
char* stringCopy(int startIndex,int endIndex,char *source);
char* GPXViewtoJSON(char* filename);
char* gpxtracktoJSON(char*filename);
char* gpxroutetoJSON(char*filename);
char* gpxfiletoJSON(char*filename);

int compareList (void *first, void *second);
char*routeListToJSON(const List *routeList);
int concatinateString(char **Original, char *toAdd);
int compareRoutes2(const void *first, const void *second);
char *createGPXFileFromJSON(char *filename, char* namespace, double version, char *creator, int numWaypoints, int numRoutes, int numTracks);
char *int2String(int num);
char *LongFloat2String(double num);
char * OneDecimal2String(double num);

void addRouteFromJSON(char *filename, char *routeName, char *name, double latitude, double longtitude, char* dataName, char* dataValue);
// void addWaypointFromJSON(char* filename, char* routeName,char* name, double longtitude, double latitude);
char* addWaypointFromJSON(char *filename, char *routeName, char *name, double latitude, double longtitude);
char* routesBetweenFromJSON(char* filename, float lat1, float lat2, float lon1, float lon2,float delta);
char* TracksBetweenFromJSON(char* filename, float lat1, float lat2, float lon1, float lon2,float delta);
char **tokenizeArray(char *line, char *delim, int *arraySize);
char* renameTrack(char* filename, char* originalName,char* newName);
char* renameRoute(char* filename, char* originalName,char* newName);
char *GPXDataListToJSON(const List *dataList);
char *GPXDataToJSON(const GPXData *data);
int compareGPXData(const void *first, const void *second);
char *ShowdataFromJSON(char *filename);
char* getNewTrackFromJSON(char *filename, char *newName);
char* getNewRouteFromJSON(char *filename, char *newName);
char *insertString(char *s1, char *s2, int pos);
GPXData *JSONtoGPXdata(const char *gpxString);
void addGPXdataToRoute(Route *rt, GPXData* data);
void addGPXDataFromJSON(char *filename, char *routeName,char* dataName, char* dataValue);
GPXData *getGPXData(Route *route, char *name);
char *ShowRouteData(char *filename, char *routeName);
char *ShowTrackData(char *filename, char *trackName);
GPXdoc* GPXParser(GPXdoc* ptr,xmlNode* root_element);
char *GPXtoJSON2(const GPXdoc *gpx);
char* findBetween(char *filename, float lat1, float lat2, float lon1, float lon2, float delta);
char* routesBetweenFromJSON2(char* filename, float lat1, float lat2, float lon1, float lon2,float delta);
char* TracksBetweenFromJSON2(char* filename, float lat1, float lat2, float lon1, float lon2,float delta);
char *WaypointListToJSON(const List *waypointList);
char *WaypointToJSON(const Waypoint *wpt);
char *waypointsFromJSON(char *filename,char* name);
int compareWaypoints2(const void *first, const void *second);
float routeLenFromJSON(char* name, char* filename);
#endif 




