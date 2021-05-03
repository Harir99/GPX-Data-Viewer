/**
 * Library that consist of functions that aid in parsing gpx files
 * @author Harir Al-Rubaye (1108085)
 * @since 08-02-2021
 */
#include <stdio.h>
// #include <libxml/tree.h>
#include "GPXParser.h"
#include "LinkedListAPI.h"
#include "GPXParser2.h"
#include <libxml/xmlschemastypes.h>
int numberR = 1;
int numberT = 1;
int numberW = 0;
#define LIBXML_SCHEMAS_ENABLED
/**
 * setting the initial values of the GPXData struct variables and lists
**/
GPXdoc *initializeGPXdoc()
{
    GPXdoc *tmpdoc = malloc(sizeof(GPXdoc));
    tmpdoc->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
    tmpdoc->routes = initializeList(routeToString, deleteRoute, compareRoutes);
    tmpdoc->tracks = initializeList(trackToString, deleteTrack, compareTracks);
    return tmpdoc;
}
/**
 * setting the initial values of the Waypoint struct variables and lists
**/
Waypoint *initializeWaypoint()
{
    Waypoint *data = calloc(1, sizeof(Waypoint));
    data->name = calloc(256, sizeof(char));
    strcpy(data->name, "");
    data->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData);
    data->longitude = 0;
    data->latitude = 0;
    return data;
}
/**
 * setting the initial values of the Track struct variables and lists
**/
Track *initializeTrack()
{
    Track *data = calloc(1, sizeof(Track));
    data->name = calloc(256, sizeof(char));
    strcpy(data->name, "");
    data->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData);
    data->segments = initializeList(trackSegmentToString, deleteTrackSegment, compareTrackSegments);
    return data;
}
/**
 * setting the initial values of the TrackSegment struct list
**/
TrackSegment *initializeTrackSegment()
{
    TrackSegment *trkseg = calloc(1, sizeof(TrackSegment));
    trkseg->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
    return trkseg;
}
/**
 * setting the initial values of the Route struct variables and lists
**/
Route *initializeRoute()
{
    Route *data = calloc(1, sizeof(Route));
    data->name = calloc(256, sizeof(char));
    data->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData);
    data->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
    return data;
}
int isText(char *name)
{
    if (strcmp(name, "text") == 0)
        return 1;
    else
        return 0;
}

/**
 * parsing and handling of GPXdoc in the gpx file 
**/
int extractGPX(xmlNode *a_node, GPXdoc *tmpdoc)
{
    // Iterate through every attribute of the current node
    xmlAttr *attr;
    for (attr = a_node->properties; attr != NULL; attr = attr->next)
    {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        if (strcmp(attrName, "version") == 0)
        {
            tmpdoc->version = atof(cont);
        }
        if (strcmp(attrName, "creator") == 0)
        {
            if ((cont == NULL) || (strcmp(cont, "") == 0))
                return -1;
            tmpdoc->creator = malloc(sizeof(char) * strlen(cont) + 1);
            strcpy(tmpdoc->creator, cont);
        }
    }
    return 0;
}
/**
 * parsing and handling of GPXData in the gpx file 
**/
GPXData *extractGPXData(xmlNode *a_node)
{
    GPXData *data = calloc(1, sizeof(GPXData) + 256);
    char *name = (char *)a_node->name;

    //Error check for empty string, if not  then copy name into the GPXData* struct
    if (strcmp(name, "") == 0)
        return NULL;
    else
        strcpy(data->name, name);

    //Error check for empty string, if not  then copy value into the GPXData* struct
    char *value = (char *)a_node->children->content;
    if (strcmp(value, "") == 0)
        return NULL;
    else
        strcpy(data->value, value);
    return data;
}
/**
 * parsing and handling of Waypoints in the gpx file 
**/
Waypoint *extractWaypoint(xmlNode *a_node)
{
    Waypoint *data = initializeWaypoint();

    // Iterate through every attribute of the current node
    xmlAttr *attr;
    for (attr = a_node->properties; attr != NULL; attr = attr->next)
    {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);

        // compare the attributes with the ones from the gpx file to find matches
        // if match if found copy its content into the Waypoint* struct

        // compare the string "lon" with the wpt(waypoint) attributes from the gpx file to find matches
        if (strcmp(attrName, "lon") == 0)
        {
            double longtitude = atof(cont);
            data->longitude = longtitude;
        }
        // compare the string "lat" with the wpt(waypoint) attributes from the gpx file to find matches
        if (strcmp(attrName, "lat") == 0)
        {
            double latitude = atof(cont);
            data->latitude = latitude;
        }
    }

    // Iterate through every child of the current node
    for (xmlNode *node = a_node->children; node != NULL; node = node->next)
    {
        // compare the children with the ones from the gpx file to find matches
        // if match if found copy its content into the Waypoint* struct
        char *name = (char *)node->name;
        if (isText(name))
            continue;

        // compare the string "name" with the wpt(waypoint) children from the gpx file to find matches
        if (strcmp(name, "name") == 0)
        {
            char *nameCont = (char *)node->children->content;
            if (strcmp(nameCont, "\0") == 0)
                strcpy(data->name, " ");
            else
                strcpy(data->name, nameCont);
        }
        // otherwise GPXData
        else
        {
            GPXData *gpxData = extractGPXData(node);
            insertBack(data->otherData, gpxData);
        }
    }

    return data;
}
/**
 * parsing and handling of TrackSegment in the gpx file 
**/
TrackSegment *extractTrackSegment(xmlNode *a_node)
{
    TrackSegment *trkseg = initializeTrackSegment();

    // Iterate through every child of the current node
    for (xmlNode *node = a_node->children; node != NULL; node = node->next)
    {
        char *name = (char *)node->name;

        // compare the children with the ones from the gpx file to find matches
        // if match if found copy its content into the TrackSegment * struct
        if (strcmp(name, "trkpt") == 0)
        {
            Waypoint *waypoint = extractWaypoint(node);
            insertBack(trkseg->waypoints, waypoint);
        }
    }

    return trkseg;
}
/**
 * parsing and handling of Track in the gpx file 
**/
Track *extractTrack(xmlNode *a_node)
{
    Track *data = initializeTrack();
    TrackSegment *trackSeg;

    // Iterate through every child of the current node
    for (xmlNode *node = a_node->children; node != NULL; node = node->next)
    {
        // compare the children with the ones from the gpx file to find matches
        // if match if found copy its content into the Track * struct
        char *name = (char *)node->name;
        if (isText(name))
            continue;
        // compare the string "name" with the trk(Track) children from the gpx file to find a match
        if (strcmp(name, "name") == 0)
        {
            char *nameCont = (char *)node->children->content;
            if (strcmp(nameCont, "\0") == 0)
                strcpy(data->name, "");
            else
                strcpy(data->name, nameCont);
        }
        // compare the string "trkseg" with the trk(Track) children from the gpx file to find a match
        else if (strcmp(name, "trkseg") == 0)
        {
            trackSeg = extractTrackSegment(node);
            insertBack(data->segments, trackSeg);
        }
        // otherwise it will be a GPXData
        else
        {
            GPXData *gpxData = extractGPXData(node);
            insertBack(data->otherData, gpxData);
        }
    }
    return data;
}
/**
 * parsing and handling of Route in the gpx file 
**/
Route *extractRoute(xmlNode *a_node)
{
    Route *data = initializeRoute();

    // Iterate through every child of the current node
    for (xmlNode *node = a_node->children; node != NULL; node = node->next)
    {
        // compare the children with the ones from the gpx file to find matches
        // if match if found copy its content into the Route * struct
        char *name = (char *)node->name;

        if (isText(name))
            continue;
        // compare the string "name" with the rte (Route) children from the gpx file to find a match
        if (strcmp(name, "name") == 0)
        {
            char *nameCont = (char *)node->children->content;
            if (strcmp(nameCont, "\0") == 0)
                strcpy(data->name, "");
            else
                strcpy(data->name, nameCont);
        }
        // compare the string "rtept" with the rte (Route) children from the gpx file to find a match
        else if (strcmp(name, "rtept") == 0)
        {
            Waypoint *waypoint = extractWaypoint(node);
            insertBack(data->waypoints, waypoint);
        }
        // otherwise its GPXData
        else
        {
            GPXData *gpxData = extractGPXData(node);
            insertBack(data->otherData, gpxData);
        }
    }
    return data;
}
/**
 * This function does the parsing and allocates a GPXdoc object by calling helper functions initializeGPXdoc and extractGPX
 * then add calls others helper functions: extractWaypoint, extractTrack, and extractRoute that were used 
 * to do the handling for waypoints, routes and tracks
**/
GPXdoc *createGPXdoc(char *fileName)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    // initialize the library and check for mismatches
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL)
    {
        return NULL;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);
    GPXdoc *tmpdoc = initializeGPXdoc();
    tmpdoc = GPXParser(tmpdoc,root_element);

    /*free the document */
    xmlFreeDoc(doc);

    xmlCleanupParser();

    return tmpdoc;
}
GPXdoc* GPXParser(GPXdoc* ptr,xmlNode* root_element)
{
    /*Get the root element node */
    if (root_element->ns != NULL)
    {
        strcpy(ptr->namespace, (char *)root_element->ns->href);
    }
    else
    {
        strcpy(ptr->namespace, "http://www.topografix.com/GPX/1/1");
    }
    extractGPX(root_element, ptr);

    for (xmlNode *child = root_element->children; child != NULL; child = child->next)
    {
        char *name = (char *)child->name;
        if (isText(name))
            continue;

        // compare the string "wpt" with the gpx children from the gpx file to find a match
        if (strcmp(name, "wpt") == 0)
        {
            Waypoint *waypt = extractWaypoint(child);
            insertBack(ptr->waypoints, waypt);
        }
        // compare the string "trk" with the gpx children from the gpx file to find a match
        else if (strcmp(name, "trk") == 0)
        {
            Track *trk = extractTrack(child);
            insertBack(ptr->tracks, trk);
        }
        // compare the string "rte" with the gpx children from the gpx file to find a match
        else if (strcmp(name, "rte") == 0)
        {
            Route *rte = extractRoute(child);
            insertBack(ptr->routes, rte);
        }
    }
    return ptr;
}
/**
 * deallocates the memory previously allocated by initializeGPXdoc
**/
void deleteGPXdoc(GPXdoc *doc)
{
    if (doc == NULL)
        return;
    free(doc->creator);
    freeList(doc->waypoints);
    freeList(doc->routes);
    freeList(doc->tracks);
    free(doc);
}
/**
 * returns a string that contains GPXdoc so we can print it 
 * it does this by taking a store a GPXdoc* in a char* (string)
**/
char *GPXdocToString(GPXdoc *doc)
{

    char *waypoints = toString(doc->waypoints);
    char *routes = toString(doc->routes);
    char *tracks = toString(doc->tracks);
    char *gpxString = calloc(1000 + strlen(waypoints) + strlen(routes) + strlen(tracks), sizeof(char));
    sprintf(gpxString, "namespace = %s\nversion = %0.1lf\ncreator = %s\n%s\n%s\n%s\n\n", doc->namespace, doc->version, doc->creator, waypoints, routes, tracks);
    free(waypoints);
    free(routes);
    free(tracks);
    return gpxString;
}
/**
 * deallocates the memory previously allocated when we initialized GPXdata 
**/
void deleteGpxData(void *data)
{
    GPXData *doc = (GPXData *)data;
    free(doc);
}
/**
 * returns a string that contains GPXdata so we can print it 
 * it does this by taking a store a GPXData * in a char* (string)
**/
char *gpxDataToString(void *data)
{
    char *string = malloc(sizeof(char) + 256);
    strcpy(string, "");
    if (data == NULL)
        return string;
    free(string);

    GPXData *doc = (GPXData *)data;
    char *gpxString = calloc(10000, sizeof(char));
    sprintf(gpxString, "Node Type: Element, Name: GPXData \n");
    if (strcmp(doc->name, "\0") != 0)
        sprintf(gpxString + strlen(gpxString), "name = %s\n", doc->name);
    if (strcmp(doc->value, "\0") != 0)
        sprintf(gpxString + strlen(gpxString), "value = %s\n", doc->value);

    return gpxString;
}
/**
 * takes two GPXData * structures and contains logic to decide their relative 
**/
int compareGpxData(const void *first, const void *second)
{
    GPXData *structOne = (GPXData *)first;
    GPXData *structTwo = ((GPXData *)second);

    if (structOne == NULL || structTwo == NULL)
        return -1;

    int compareValue = strcmp(structOne->name, structTwo->name) ? 0 : -1;
    compareValue += strcmp(structOne->value, structTwo->value) ? 0 : -1;

    return compareValue;
}
/**
 * deallocates the memory previously allocated by initializeWaypoint
**/
void deleteWaypoint(void *data)
{
    Waypoint *wpt = (Waypoint *)data;
    freeList(wpt->otherData);
    free(wpt->name);
    free(wpt);
}
/**
 * returns a string that contains Waypoint so we can print it 
 * it does this by taking a store a Waypoint * in a char* (string)
**/
char *waypointToString(void *data)
{
    char *string = malloc(sizeof(char) + 256);
    strcpy(string, "");
    if (data == NULL)
        return string;
    free(string);

    Waypoint *doc = (Waypoint *)data;
    char *gpxString = calloc(1000, sizeof(char));
    char *otherData = toString(doc->otherData);
    sprintf(gpxString, "Node Type: Element, Name: Wayppint\nname = %s\nlongtitude = %lf\nlatitude = %lf\n%s\n", doc->name, doc->latitude, doc->longitude, otherData);
    free(otherData);
    return gpxString;
}
/**
 * takes two Waypoint * structures and contains logic to decide their relative 
**/
int compareWaypoints(const void *first, const void *second)
{
    Waypoint *structOne = (Waypoint *)first;
    Waypoint *structTwo = (Waypoint *)second;

    if (structOne == NULL || structTwo == NULL)
        return -1;

    int compareValue = strcmp(structOne->name, structTwo->name) ? 0 : -1;
    compareValue += structOne->longitude - structTwo->longitude ? 0 : -1;
    compareValue += structOne->latitude - structTwo->latitude ? 0 : -1;
    compareValue += (compareList(structOne->otherData, structTwo->otherData) == 0) ? 0 : -1;

    return compareValue;
}

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc *doc)
{
    if (doc == NULL)
        return -1;
    Track *trkList;

    int numOfSegments = 0;
    ListIterator trackIter = createIterator(doc->tracks);
    void *tracks;
    while ((tracks = nextElement(&trackIter)) != NULL)
    {
        trkList = (Track *)tracks;
        numOfSegments += getLength(trkList->segments);
    }

    return numOfSegments;
}

//Total number of GPXData elements in the document
int getNumGPXData(const GPXdoc *doc)
{
    if (doc == NULL)
        return -1;
    int otherData = getNumOfOtherData(doc);
    return otherData;
}

int getNumOfOtherData(const GPXdoc *doc)
{
    Track *trkList;
    TrackSegment *trkSegList;
    Waypoint *waypointList;

    Route *routeList;
    int finalOther = 0;
    ListIterator trackIter = createIterator(doc->tracks);
    void *tracks;
    while ((tracks = nextElement(&trackIter)) != NULL)
    {
        // get none empty track names and count them into the finalOther count
        trkList = (Track *)tracks;
        if (strcmp(trkList->name, "") != 0)
        {
            finalOther++;
        }
        // iterate through the tracks and add number of OtherData to finalOther count
        finalOther += getLength(trkList->otherData);
        ListIterator segIter = createIterator(trkList->segments);
        void *segs;
        while ((segs = nextElement(&segIter)) != NULL)
        {
            trkSegList = (TrackSegment *)segs;
            ListIterator wptIter = createIterator(trkSegList->waypoints);
            void *wpts;
            while ((wpts = nextElement(&wptIter)) != NULL)
            {
                // get none empty track waypoint names and count them into the finalOther count
                waypointList = (Waypoint *)wpts;
                if (strcmp(waypointList->name, "") != 0)
                {
                    finalOther++;
                }
                // iterate through the tracks waypoints and add number of OtherData to finalOther count
                finalOther += getLength(waypointList->otherData);
            }
        }
    }
    ListIterator routesIter = createIterator(doc->routes);
    void *routes;

    while ((routes = nextElement(&routesIter)) != NULL)
    {
        // get none empty route names and count them into the finalOther count
        routeList = (Route *)routes;
        if (strcmp(routeList->name, "") != 0)
        {
            finalOther++;
        }
        // iterate through the routes and add number of OtherData to finalOther count
        finalOther += getLength(routeList->otherData);
        ListIterator wptIter = createIterator(routeList->waypoints);
        void *wpts;
        while ((wpts = nextElement(&wptIter)) != NULL)
        {
            // get none empty route waypoint names and count them into the finalOther count
            waypointList = (Waypoint *)wpts;
            if (strcmp(waypointList->name, "") != 0)
            {
                finalOther++;
            }
            // iterate through the routes waypoints and add number of OtherData to finalOther count
            finalOther += getLength(waypointList->otherData);
        }
    }
    ListIterator wptIter = createIterator(doc->waypoints);
    void *wpts;
    while ((wpts = nextElement(&wptIter)) != NULL)
    {
        // get none empty waypoint names and count them into the finalOther count
        waypointList = (Waypoint *)wpts;
        if (strcmp(waypointList->name, "") != 0)
        {
            finalOther++;
        }
        // iterate through the waypoints and add number of OtherData to finalOther count
        finalOther += getLength(waypointList->otherData);
    }

    return finalOther;
}
/**
 * deallocates the memory previously allocated by initializeRoute
**/
void deleteRoute(void *data)
{
    Route *doc = (Route *)data;
    freeList(doc->waypoints);
    freeList(doc->otherData);
    free(doc->name);
    free(doc);
}
/**
 * returns a string that contains Route so we can print it 
 * it does this by taking a store a Route * in a char* (string)
**/
char *routeToString(void *data)
{
    char *string = malloc(sizeof(char) + 256);
    strcpy(string, "");
    if (data == NULL)
        return string;
    free(string);

    Route *doc = (Route *)data;
    char *gpxString = calloc(100000, sizeof(char));

    char *wpt = toString(doc->waypoints);
    char *otherData = toString(doc->otherData);

    sprintf(gpxString, "Node Type: Element, Name: Route\nname = %s\n%s\n%s\n", doc->name, wpt, otherData);
    free(wpt);
    free(otherData);
    return gpxString;
}
/**
 * takes two Routes * structures and contains logic to decide their relative 
**/
int compareRoutes(const void *first, const void *second)
{
    Route *structOne = (Route *)first;
    Route *structTwo = (Route *)second;

    if (structOne == NULL || structTwo == NULL)
        return -1;

    int compareValue = strcmp(structOne->name, structTwo->name) ? 0 : -1;
    compareValue += (compareList(structOne->waypoints, structTwo->waypoints) == 0) ? 0 : -1;
    compareValue += (compareList(structOne->otherData, structTwo->otherData) == 0) ? 0 : -1;

    return compareValue;
}

/**
 * deallocates the memory previously allocated by initializeTrackSegment
**/
void deleteTrackSegment(void *data)
{
    TrackSegment *doc = (TrackSegment *)data;
    freeList(doc->waypoints);
    free(doc);
}
/**
 * returns a string that contains TrackSegment so we can print it 
 * it does this by taking a store a TrackSegment * in a char* (string)
**/
char *trackSegmentToString(void *data)
{
    char *string = malloc(sizeof(char) + 256);
    strcpy(string, "");
    if (data == NULL)
        return string;
    free(string);

    TrackSegment *trkseg = (TrackSegment *)data;
    return toString(trkseg->waypoints);
}
int compareTrackSegments(const void *first, const void *second)
{
    TrackSegment *structOne = (TrackSegment *)first;
    TrackSegment *structTwo = (TrackSegment *)second;

    if (structOne == NULL || structTwo == NULL)
        return -1;

    int waypoint = (compareList(structOne->waypoints, structTwo->waypoints) == 0) ? 0 : -1;

    return waypoint;
}
/**
 * deallocates the memory previously allocated by initializeTrack
**/
void deleteTrack(void *data)
{
    Track *doc = (Track *)data;
    freeList(doc->segments);
    freeList(doc->otherData);
    free(doc->name);
    free(doc);
}
/**
 * returns a string that contains Track so we can print it 
 * it does this by taking a store a Track* in a char* (string)
**/
char *trackToString(void *data)
{
    char *string = malloc(sizeof(char) + 256);
    strcpy(string, "");
    if (data == NULL)
        return string;
    free(string);

    Track *doc = (Track *)data;
    char *gpxString = calloc(1000, sizeof(char));
    char *seg = toString(doc->segments);
    char *gpxData = toString(doc->otherData);
    sprintf(gpxString, "Node Type: Element, Name: Track\nname = %s\n%s\n%s\n\n", doc->name, seg, gpxData);
    free(gpxData);
    free(seg);
    return gpxString;
}
/**
 * takes two Track * structures and contains logic to decide their relative 
**/
int compareTracks(const void *first, const void *second)
{
    Track *structOne = (Track *)first;
    Track *structTwo = (Track *)second;

    if (structOne == NULL || structTwo == NULL)
        return -1;

    int compareValue = strcmp(structOne->name, structTwo->name) ? 0 : -1;
    compareValue += (compareList(structOne->segments, structTwo->segments) == 0) ? 0 : -1;
    compareValue += (compareList(structOne->otherData, structTwo->otherData) == 0) ? 0 : -1;

    return compareValue;
}
// total number of waypoints in the GPX file
int getNumWaypoints(const GPXdoc *doc)
{
    if (doc == NULL)
        return -1;
    int NumOfWayPoints;
    NumOfWayPoints = getLength(doc->waypoints);
    return NumOfWayPoints;
}
// Total number of tracks in the GPX file
int getNumTracks(const GPXdoc *doc)
{
    if (doc == NULL)
        return -1;
    int NumOfTracks;
    NumOfTracks = getLength(doc->tracks);
    return NumOfTracks;
}

//Total number of routes in the GPX file
int getNumRoutes(const GPXdoc *doc)
{
    if (doc == NULL)
        return -1;
    int NumOfRoutes;
    NumOfRoutes = getLength(doc->routes);
    return NumOfRoutes;
}

// Function that returns a track with the given name.  If more than one exists, return the first one.
// Return NULL if the track does not exist
Track *getTrack(const GPXdoc *doc, char *name)
{
    if (doc == NULL)
        return NULL;
    List *trk = doc->tracks;
    void *data = findElement(trk, compareTrk, name);
    return data;
}
// Function that returns a route with the given name.  If more than one exists, return the first one.
// Return NULL if the route does not exist
Route *getRoute(const GPXdoc *doc, char *name)
{
    if (doc == NULL)
        return NULL;
    List *rte = doc->routes;
    void *data = findElement(rte, compareRte, name);
    return data;
}

// Function that returns a Waypoint with the given name.  If more than one exists, return the first one.
// Return NULL if the Waypoint does not exist
Waypoint *getWaypoint(const GPXdoc *doc, char *name)
{
    if (doc == NULL)
        return NULL;
    List *wpt = doc->waypoints;
    void *data = findElement(wpt, compareWpt, name);
    return data;
}
// boolean function that compares given name with the names in the waypoint struct looking for a match
bool compareWpt(const void *first, const void *second)
{
    Waypoint *structOne = (Waypoint *)first;
    char *nameToFind = (char *)second;
    bool name = false;
    if (structOne->name == NULL && nameToFind == NULL)
        return true;
    if (structOne->name == NULL || nameToFind == NULL)
        return false;

    if (strcmp(structOne->name, nameToFind) == 0)
        name = true;
    else
        name = false;

    return (name);
}
// boolean function that compares given name with the names in the Track struct looking for a match
bool compareTrk(const void *first, const void *second)
{
    Track *structOne = (Track *)first;
    char *nameToFind = (char *)second;
    bool name = false;
    if (structOne->name == NULL && nameToFind == NULL)
        return true;
    if (structOne->name == NULL || nameToFind == NULL)
        return false;

    if (strcmp(structOne->name, nameToFind) == 0)
        name = true;
    else
        name = false;

    return (name);
}
// boolean function that compares given name with the names in the Route struct looking for a match
bool compareRte(const void *first, const void *second)
{
    Route *structOne = (Route *)first;
    char *nameToFind = (char *)second;
    bool name = false;

    if (structOne->name == NULL && nameToFind == NULL)
        return true;
    if (structOne->name == NULL || nameToFind == NULL)
        return false;

    if (strcmp(structOne->name, nameToFind) == 0)
        name = true;
    else
        name = false;

    return (name);
}

GPXdoc *createValidGPXdoc(char *fileName, char *gpxSchemaFile)
{
    xmlDocPtr doc;
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    char *XMLFileName = fileName;
    char *XSDFileName = gpxSchemaFile;
    xmlNode *root_element = NULL;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(XSDFileName);

    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    doc = xmlReadFile(XMLFileName, NULL, 0);
    GPXdoc *ptr = NULL;

    if (doc == NULL)
    {
        return NULL;
    }
    else
    {
        xmlSchemaValidCtxtPtr ctxt;
        int ret;

        // schema context (contains the rules ) matches the rules of whether the doc is valid
        ctxt = xmlSchemaNewValidCtxt(schema);
        ret = xmlSchemaValidateDoc(ctxt, doc);
        if (ret == 0)
        {
            root_element = xmlDocGetRootElement(doc);
            ptr = initializeGPXdoc();
            ptr = GPXParser(ptr,root_element);
        }
        else if (ret > 0) { }
        else { }
        xmlSchemaFreeValidCtxt(ctxt);
        xmlFreeDoc(doc);
    }
    // free the resource
    if (schema != NULL)
        xmlSchemaFree(schema);

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();
    return ptr;
}
int checkGPXData(GPXData *data)
{
    if (data == NULL)
        return 1;
    if (strcmp(data->name, "") == 0)
        return 1;

    if (strcmp(data->value, "") == 0)
        return 1;
    return 0;
}
int checkWayPoint(Waypoint *data)
{
    if (data == NULL)
        return 1;
    if (data->name == NULL)
        return 1;
    if (data->otherData == NULL)
        return 1;

    ListIterator iter = createIterator(data->otherData);
    void *other;
    while ((other = nextElement(&iter)) != NULL)
    {
        GPXData *dataList = (GPXData *)other;
        int dataNum = checkGPXData(dataList);
        if (dataNum == 1)
        {
            return dataNum;
        }
    }

    return 0;
}
int checkTrackSeg(TrackSegment *data)
{
    if (data == NULL)
        return 1;

    if (data->waypoints == NULL)
        return 1;

    ListIterator iter = createIterator(data->waypoints);
    void *other;
    while ((other = nextElement(&iter)) != NULL)
    {
        Waypoint *dataList = (Waypoint *)other;
        int dataNum = checkWayPoint(dataList);
        if (dataNum == 1)
        {
            return dataNum;
        }
    }

    return 0;
}
int checkTrack(Track *data)
{
    if (data == NULL)
        return 1;
    if (data->name == NULL)
        return 1;
    if (data->segments == NULL)
        return 1;

    if (data->otherData == NULL)
        return 1;

    ListIterator iter = createIterator(data->segments);
    void *other;
    while ((other = nextElement(&iter)) != NULL)
    {
        TrackSegment *dataList = (TrackSegment *)other;
        int dataNum = checkTrackSeg(dataList);
        if (dataNum == 1)
        {
            return dataNum;
        }
    }
    iter = createIterator(data->otherData);
    while ((other = nextElement(&iter)) != NULL)
    {
        GPXData *dataList = (GPXData *)other;
        int dataNum = checkGPXData(dataList);
        if (dataNum == 1)
        {
            return dataNum;
        }
    }

    return 0;
}
int checkRoute(Route *data)
{
    if (data == NULL)
        return 1;
    if (data->name == NULL)
        return 1;
    if (data->waypoints == NULL)
        return 1;

    if (data->otherData == NULL)
        return 1;

    ListIterator iter = createIterator(data->waypoints);
    void *other;
    while ((other = nextElement(&iter)) != NULL)
    {
        Waypoint *dataList = (Waypoint *)other;
        int dataNum = checkWayPoint(dataList);
        if (dataNum == 1)
        {
            return dataNum;
        }
    }
    iter = createIterator(data->otherData);
    while ((other = nextElement(&iter)) != NULL)
    {
        GPXData *dataList = (GPXData *)other;
        int dataNum = checkGPXData(dataList);
        if (dataNum == 1)
        {
            return dataNum;
        }
    }

    return 0;
}
int checkGPXdoc(GPXdoc *data)
{
    if (data == NULL)
        return 1;
    if (strlen(data->namespace) == 0)
        return 1;
    if (data->creator == NULL || strlen(data->creator) == 0)
        return 1;

    if (data->waypoints == NULL)
        return 1;

    ListIterator iter = createIterator(data->waypoints);
    void *other;
    while ((other = nextElement(&iter)) != NULL)
    {
        Waypoint *wpt = (Waypoint *)other;
        int checkwpt = checkWayPoint(wpt);
        if (checkwpt == 1)
        {
            return checkwpt;
        }
    }

    if (data->tracks == NULL)
        return 1;
    iter = createIterator(data->tracks);

    while ((other = nextElement(&iter)) != NULL)
    {
        Track *wpt = (Track *)other;
        int checkwpt = checkTrack(wpt);
        if (checkwpt == 1)
        {
            return checkwpt;
        }
    }

    if (data->routes == NULL)
        return 1;
    iter = createIterator(data->routes);
    while ((other = nextElement(&iter)) != NULL)
    {
        Route *wpt = (Route *)other;
        int checkwpt = checkRoute(wpt);
        if (checkwpt == 1)
        {
            return checkwpt;
        }
    }

    return 0;
}

bool validateGPXDoc(GPXdoc *gpxDoc, char *gpxSchemaFile)
{
    if (gpxDoc == NULL || gpxSchemaFile == NULL)
        return false;

    int check = checkGPXdoc(gpxDoc);
    if (check == 1)
    {
        deleteGPXdoc(gpxDoc);
        return false;
    }

    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    char *XSDFileName = gpxSchemaFile;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(XSDFileName);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    xmlSchemaValidCtxtPtr ctxt2;
    int ret;

    ctxt2 = xmlSchemaNewValidCtxt(schema);
    xmlDoc *doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNode *root = GPXDoc2xmlNode(gpxDoc, true);
    xmlDocSetRootElement(doc, root);

    ret = xmlSchemaValidateDoc(ctxt2, doc);

    if (ret == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
    xmlSchemaFreeValidCtxt(ctxt2);
    if (schema != NULL)
        xmlSchemaFree(schema);

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();
}

bool writeGPXdoc(GPXdoc *doc, char *fileName)
{
    if (doc == NULL || strlen(fileName) == 0 || strcmp(fileName, "") == 0)
        return false;

    //create root node
    xmlNode *root = GPXDoc2xmlNode(doc, false);
    //create xml doc
    xmlDoc *xmlDoc = xmlNewDoc(BAD_CAST "1.0");
    // adding the root node to the xmldoc
    xmlDocSetRootElement(xmlDoc, root);
    // write the xmldoc to the file
    xmlSaveFormatFileEnc(fileName, xmlDoc, "UTF-8", 1);
    xmlFreeDoc(xmlDoc);
    xmlCleanupParser();
    return true;
}

xmlNode *GPXdata2xmlNode(GPXData *data, char *namespace, bool getNamespace)
{
    xmlNode *name = xmlNewNode(NULL, BAD_CAST data->name);
    xmlNode *value = xmlNewText(BAD_CAST data->value);

    if (getNamespace == true)
    {
        xmlNsPtr ns = xmlNewNs(name, BAD_CAST namespace, BAD_CAST NULL);
        xmlSetNs(name, ns);
    }

    xmlAddChild(name, value);
    return name;
}

xmlNode *Name2xmlNode(char *data, char *namespace, bool getNamespace)
{
    if (strlen(data) == 0)
        return NULL;
    xmlNode *name = xmlNewNode(NULL, BAD_CAST "name");
    xmlNode *text = xmlNewText(BAD_CAST data);
    if (getNamespace == true)
    {
        xmlNsPtr ns = xmlNewNs(name, BAD_CAST namespace, BAD_CAST NULL);
        xmlSetNs(name, ns);
    }

    xmlAddChild(name, text);
    return name;
}
xmlNode *waypoints2xmlNode(Waypoint *data, char *type, char *namespace, bool getNamespace)
{
    xmlNode *node = xmlNewNode(NULL, BAD_CAST type);

    if (getNamespace == true)
    {
        xmlNsPtr ns = xmlNewNs(node, BAD_CAST namespace, BAD_CAST NULL);
        xmlSetNs(node, ns);
    }

    char *s = calloc(100, sizeof(char));
    sprintf(s, "%.6f", data->latitude);
    xmlNewProp(node, BAD_CAST "lat", BAD_CAST s);
    free(s);

    s = calloc(100, sizeof(char));
    sprintf(s, "%.6f", data->longitude);
    xmlNewProp(node, BAD_CAST "lon", BAD_CAST s);
    free(s);

    ListIterator iter = createIterator(data->otherData);
    void *other;
    while ((other = nextElement(&iter)) != NULL)
    {
        GPXData *DataList = (GPXData *)other;
        xmlNode *dataNode = GPXdata2xmlNode(DataList, namespace, getNamespace);
        xmlAddChild(node, dataNode);
    }

    xmlNode *name = Name2xmlNode(data->name, namespace, getNamespace);
    xmlAddChild(node, name);

    return node;
}

xmlNode *Route2xmlNode(Route *data, char *namespace, bool getNamespace)
{
    xmlNode *node = xmlNewNode(NULL, BAD_CAST "rte");
    xmlNode *name = Name2xmlNode(data->name, namespace, getNamespace);
    xmlAddChild(node, name);

    if (getNamespace == true)
    {
        xmlNsPtr ns = xmlNewNs(node, BAD_CAST namespace, BAD_CAST NULL);
        xmlSetNs(node, ns);
    }

    ListIterator iter = createIterator(data->otherData);
    void *other;
    while ((other = nextElement(&iter)) != NULL)
    {
        GPXData *DataList = (GPXData *)other;
        xmlNode *dataNode = GPXdata2xmlNode(DataList, namespace, getNamespace);
        xmlAddChild(node, dataNode);
    }

    ListIterator wptIter = createIterator(data->waypoints);
    void *wpts;
    while ((wpts = nextElement(&wptIter)) != NULL)
    {
        Waypoint *wayPointList = (Waypoint *)wpts;
        xmlNode *waypointsNode = waypoints2xmlNode(wayPointList, "rtept", namespace, getNamespace);
        xmlAddChild(node, waypointsNode);
    }

    return node;
}

xmlNode *TrackSegment2xmlNode(TrackSegment *data, char *namespace, bool getNamespace)
{
    xmlNode *node = xmlNewNode(NULL, BAD_CAST "trkseg");
    if (getNamespace == true)
    {
        xmlNsPtr ns = xmlNewNs(node, BAD_CAST namespace, BAD_CAST NULL);
        xmlSetNs(node, ns);
    }

    ListIterator wptIter = createIterator(data->waypoints);
    void *wpts;
    while ((wpts = nextElement(&wptIter)) != NULL)
    {
        Waypoint *wayPointList = (Waypoint *)wpts;
        xmlNode *waypointsNode = waypoints2xmlNode(wayPointList, "trkpt", namespace, getNamespace);
        xmlAddChild(node, waypointsNode);
    }
    return node;
}
xmlNode *Track2xmlNode(Track *data, char *namespace, bool getNamespace)
{
    xmlNode *node = xmlNewNode(NULL, BAD_CAST "trk");
    xmlNode *name = Name2xmlNode(data->name, namespace, getNamespace);
    xmlAddChild(node, name);
    if (getNamespace == true)
    {
        xmlNsPtr ns = xmlNewNs(node, BAD_CAST namespace, BAD_CAST NULL);
        xmlSetNs(node, ns);
    }

    ListIterator iter = createIterator(data->otherData);
    void *other;
    while ((other = nextElement(&iter)) != NULL)
    {
        GPXData *DataList = (GPXData *)other;
        xmlNode *dataNode = GPXdata2xmlNode(DataList, namespace, getNamespace);
        xmlAddChild(node, dataNode);
    }

    ListIterator segIter = createIterator(data->segments);
    void *seg;
    while ((seg = nextElement(&segIter)) != NULL)
    {
        TrackSegment *segList = (TrackSegment *)seg;
        xmlNode *segNode = TrackSegment2xmlNode(segList, namespace, getNamespace);
        xmlAddChild(node, segNode);
    }
    return node;
}

xmlNode *GPXDoc2xmlNode(GPXdoc *data, bool getNamespace)
{
    xmlNode *node = xmlNewNode(NULL, BAD_CAST "gpx");

    char *s = calloc(100, sizeof(char));
    sprintf(s, "%0.1lf", data->version);
    xmlNewProp(node, BAD_CAST "version", BAD_CAST s);
    free(s);

    xmlNewProp(node, BAD_CAST "creator", BAD_CAST data->creator);
    xmlNsPtr ns = xmlNewNs(node, BAD_CAST data->namespace, BAD_CAST NULL);
    xmlSetNs(node, ns);

    ListIterator wptIter = createIterator(data->waypoints);
    void *wpts;
    while ((wpts = nextElement(&wptIter)) != NULL)
    {
        Waypoint *wayPointList = (Waypoint *)wpts;
        xmlNode *waypointsNode = waypoints2xmlNode(wayPointList, "wpt", data->namespace, getNamespace);
        xmlAddChild(node, waypointsNode);
    }

    ListIterator routesIter = createIterator(data->routes);
    void *rte;
    while ((rte = nextElement(&routesIter)) != NULL)
    {
        Route *routeList = (Route *)rte;
        xmlNode *routeNode = Route2xmlNode(routeList, data->namespace, getNamespace);
        xmlAddChild(node, routeNode);
    }

    ListIterator trackIter = createIterator(data->tracks);
    void *trk;
    while ((trk = nextElement(&trackIter)) != NULL)
    {
        Track *trackList = (Track *)trk;
        xmlNode *trackNode = Track2xmlNode(trackList, data->namespace, getNamespace);
        xmlAddChild(node, trackNode);
    }
    return node;
}

float round10(float len)
{
    int rounded = roundInt(len);
    return (float)rounded;
}
// function to round the number
int roundInt(int len)
{
    // Smaller multiple
    int smallest = (len / 10) * 10;

    // Larger multiple
    int largest = smallest + 10;

    // Return of closest of two
    return (len - smallest >= largest - len) ? largest : smallest;
}

float getRouteLen(const Route *rt)
{
    double firstLat = 0, secondLat = 0;
    double firstLon = 0, secondLon = 0;

    if (rt == NULL)
        return 0;

    float length = 0;
    ListIterator wptIter = createIterator(rt->waypoints);
    void *wpts;

    while ((wpts = nextElement(&wptIter)) != NULL)
    {
        Waypoint *wayPointOne = (Waypoint *)wpts;

        ListIterator wptIter2 = wptIter;
        void *wpts2 = nextElement(&wptIter2);
        Waypoint *wayPointTwo = (Waypoint *)wpts2;

        if (wayPointTwo == NULL)
            break;

        firstLat = (wayPointOne->latitude);
        secondLat = (wayPointTwo->latitude);

        firstLon = (wayPointOne->longitude);
        secondLon = (wayPointTwo->longitude);

        length += getDistance(firstLat, firstLon, secondLat, secondLon);
    }

    return length;
}
float getDistance(float startLat, float startLon, float endLat, float endLon)
{
    float R = 6371000;                // metres
    float phi1 = startLat * PI / 180; // φ, λ in radians
    float phi2 = endLat * PI / 180;
    float deltaPhi = (endLat - startLat) * PI / 180;
    float deltaLamda = (endLon - startLon) * PI / 180;

    float a = sin(deltaPhi / 2) * sin(deltaPhi / 2) +
              cos(phi1) * cos(phi2) *
                  sin(deltaLamda / 2) * sin(deltaLamda / 2);
    float c = 2 * atan2(sqrt(a), sqrt(1 - a));

    float d = R * c; // in metres
    return d;
}

float getTrackLen(const Track *tr)
{
    double firstLat = 0;
    double secondLat = 0;
    double firstLon = 0;
    double secondLon = 0;
    if (tr == NULL)
        return 0;

    float length = 0;
    List *list = initializeList(waypointToString, dummyDelete, compareWaypoints);

    for (Node *node = tr->segments->head; node != NULL; node = node->next)
    {
        TrackSegment *segment = node->data;

        for (Node *node2 = segment->waypoints->head; node2 != NULL; node2 = node2->next)
        {
            Waypoint *wpts = node2->data;
            insertBack(list, wpts);
        }
    }
    for (Node *node = list->head; node->next != NULL; node = node->next)
    {
        Waypoint *wayPointOne = node->data;
        Waypoint *wayPointTwo = node->next->data;

        firstLat = (wayPointOne->latitude);
        secondLat = (wayPointTwo->latitude);

        firstLon = (wayPointOne->longitude);
        secondLon = (wayPointTwo->longitude);

        length += getDistance(firstLat, firstLon, secondLat, secondLon);
    }
    freeList(list);
    return length;
}

int numRoutesWithLength(const GPXdoc *doc, float len, float delta)
{
    if (doc == NULL)
        return 0;
    if (len < 0 || delta < 0)
        return 0;
    int numOfRoutes = 0;

    ListIterator routesIter = createIterator(doc->routes);
    void *rte;

    while ((rte = nextElement(&routesIter)) != NULL)
    {
        Route *routeOne = (Route *)rte;
        float lengthOne = getRouteLen(routeOne);

        float difference = lengthOne -len;
        if (difference < 0)
        {
            difference *= -1;
        }
        if(difference <= delta){
            numOfRoutes++;
        }
    }
    return numOfRoutes;
}
int numTracksWithLength(const GPXdoc *doc, float len, float delta)
{
    if (doc == NULL)
        return 0;
    if (len < 0 || delta < 0)
        return 0;
    int numOfTracks = 0;

    ListIterator trackIter = createIterator(doc->tracks);
    void *trk;

    while ((trk = nextElement(&trackIter)) != NULL)
    {
        Track *trackOne = (Track *)trk;
        float lengthOne = getTrackLen(trackOne);

        float difference = lengthOne - len;
        if (difference < 0)
        {
            difference *= -1;
        }
        if(difference <= delta){
            numOfTracks++;
        }
    }
    return numOfTracks;
}
bool isLoopRoute(const Route *rt, float delta)
{
    if (rt == NULL)
        return false;
    if (delta < 0)
        return false;

    int numOfWaypoints = 0;
    Waypoint *wayPointOne = getFromFront(rt->waypoints);
    Waypoint *wayPointTwo = getFromBack(rt->waypoints);
    double sourceLat = wayPointOne->latitude;
    double sourceLong = wayPointOne->longitude;
    double destLat = wayPointTwo->latitude;
    double destLong = wayPointTwo->longitude;
    float lengthOne = getDistance(sourceLat, sourceLong, destLat, destLong);

    numOfWaypoints += getLength(rt->waypoints);

    if (numOfWaypoints >= 4)
    {
        if (lengthOne <= delta)
        {
            return true;
        }
    }
    return false;
}
bool isLoopTrack(const Track *tr, float delta)
{
    if (tr == NULL)
        return false;
    if (delta < 0)
        return false;
    int numOfWaypoints = 0;

    void *segs;
    ListIterator segIter = createIterator(tr->segments);
    while ((segs = nextElement(&segIter)) != NULL)
    {
        TrackSegment *segment = (TrackSegment *)segs;
        Waypoint *wayPointOne = getFromFront(segment->waypoints);
        Waypoint *wayPointTwo = getFromBack(segment->waypoints);
        double sourceLat = wayPointOne->latitude;
        double sourceLong = wayPointOne->longitude;
        double destLat = wayPointTwo->latitude;
        double destLong = wayPointTwo->longitude;
        float lengthOne = getDistance(sourceLat, sourceLong, destLat, destLong);
        numOfWaypoints += getLength(segment->waypoints);

        if (numOfWaypoints >= 4)
        {
            if (lengthOne <= delta)
            {
                return true;
            }
        }
    }
    return false;
}
void dummyDelete() {}

List *getRoutesBetween(const GPXdoc *doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta)
{
    if (doc == NULL)
        return NULL;

    List *list = initializeList(routeToString, dummyDelete, compareRoutes);
    ListIterator routesIter = createIterator(doc->routes);
    Route *rte;

    while ((rte = nextElement(&routesIter)) != NULL)
    {
        Waypoint *wayPointOne = getFromFront(rte->waypoints);
        Waypoint *wayPointTwo = getFromBack(rte->waypoints);
        double firstLat = wayPointOne->latitude;
        double firstLon = wayPointOne->longitude;
        double secondLat = wayPointTwo->latitude;
        double secondLon = wayPointTwo->longitude;
        float lengthOne = getDistance(firstLat, firstLon, sourceLat, sourceLong);
        float lengthTwo = getDistance(secondLat, secondLon, destLat, destLong);

        if (lengthOne <= delta && lengthTwo <= delta)
        {
            insertFront(list, rte);
        }
    }
    if (getLength(list) == 0)
        return NULL;
    return list;
}
List *getTracksBetween(const GPXdoc *doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta)
{
    if (doc == NULL)
        return NULL;
  
    List *list = initializeList(trackToString, dummyDelete, compareTracks);
    ListIterator trackIter = createIterator(doc->tracks);
    Track *trk;

    while ((trk = nextElement(&trackIter)) != NULL)
    {
        ListIterator segIter = createIterator(trk->segments);
        TrackSegment *segs;

        while ((segs = nextElement(&segIter)) != NULL)
        {
            Waypoint *wayPointOne = getFromFront(segs->waypoints);
            Waypoint *wayPointTwo = getFromBack(segs->waypoints);
            double firstLat = wayPointOne->latitude;
            double firstLon = wayPointOne->longitude;
            double secondLat = wayPointTwo->latitude;
            double secondLon = wayPointTwo->longitude;
            float lengthOne = getDistance(firstLat, firstLon, sourceLat, sourceLong);
            float lengthTwo = getDistance(secondLat, secondLon, destLat, destLong);

            if (lengthOne <= delta && lengthTwo <= delta)
            {
                insertFront(list, trk);
            }
        }
    }
    if (getLength(list) == 0){
        return NULL;
    }
    return list;
}
char *OneDecimal2String(double num)
{ // convert a double to a string
    char *output = calloc(256, sizeof(char));
    sprintf(output, "%.1f", num);
    return output;
}
char *routeToJSON(const Route *rt, int *number)
{
    if (rt == NULL)
    {
        return "{}";
    }
    char *string = (char *)malloc(sizeof(char) * 256);
    strcpy(string, "{\"component\":");
    concatinateString(&string, "\"");
    char *json = calloc(256, sizeof(char));
    sprintf(json, "Route %d", *number);
    concatinateString(&string, json);
    free(json);
    concatinateString(&string, "\",");

    concatinateString(&string, "\"name\":");

    if (strcmp(rt->name, "") == VALID)
    {
        concatinateString(&string, "");
    }
    else
    {
        concatinateString(&string, "\"");
        concatinateString(&string, rt->name);
        concatinateString(&string, "\",");
    }

    concatinateString(&string, "\"numPoints\":");

    int num_point = getLength(rt->waypoints);
    char output[100];
    sprintf(output, "%d", num_point);
    concatinateString(&string, output);
    concatinateString(&string, ",");

    concatinateString(&string, "\"len\":");
    char *len = OneDecimal2String(round10(getRouteLen(rt)));
    concatinateString(&string, len);
    concatinateString(&string, ",");

    free(len);

    concatinateString(&string, "\"loop\"");
    concatinateString(&string, ":");

    char *status;

    if (isLoopRoute(rt, 10) == true)
    {
        status = "true";
    }
    else
    {
        status = "false";
    }

    concatinateString(&string, status);
    concatinateString(&string, "}");
    *number = *number + 1;
    return string;
}
char *trackToJSON(const Track *tr, int *number)
{

    if (tr == NULL)
    {
        return "{}";
    }
    char *string = (char *)malloc(sizeof(char) * 256);
    strcpy(string, "{\"component\":");
    concatinateString(&string, "\"");

    char *json = calloc(256, sizeof(char));
    sprintf(json, "Track %d", *number);
    concatinateString(&string, json);
    free(json);

    concatinateString(&string, "\",");

    concatinateString(&string, "\"name\":");

    if (strcmp(tr->name, "") == VALID)
    {
        concatinateString(&string, "");
    }
    else
    {
        concatinateString(&string, "\"");
        concatinateString(&string, tr->name);
        concatinateString(&string, "\",");
    }
    concatinateString(&string, "\"numPoints\":");
    int Segmentspoints = 0;

    ListIterator segIter = createIterator(tr->segments);
    TrackSegment *segs;
    while ((segs = nextElement(&segIter)) != NULL)
    {
        Segmentspoints = getLength(segs->waypoints);
    }

    char *numWaypoints = int2String(Segmentspoints);
    concatinateString(&string, numWaypoints);
    concatinateString(&string, ",");
    free(numWaypoints);

    concatinateString(&string, "\"len\":");
    char *len = OneDecimal2String(round10(getTrackLen(tr)));

    concatinateString(&string, len);
    concatinateString(&string, ",");
    free(len);

    concatinateString(&string, "\"loop\"");
    concatinateString(&string, ":");

    char *status;

    if (isLoopTrack(tr, 10) == true)
    {
        status = "true";
    }
    else
    {
        status = "false";
    }

    concatinateString(&string, status);
    concatinateString(&string, "}");
    *number = *number + 1;
    return string;
}
int concatinateString(char **Original, char *toAdd)
{
    if (toAdd == NULL || strcmp(toAdd, "") == 0)
    {
        return FALSE;
    }
    int originalLength = strlen(*Original);
    int toAddLength = strlen(toAdd);

    *Original = (char *)realloc(*Original, originalLength + toAddLength + 1);
    strcat(*Original, toAdd);

    return TRUE;
}
int compareList(void *first, void *second)
{
    char *str1 = toString((List *)first);
    char *str2 = toString((List *)second);

    int retVal = strcmp(str1, str2);
    free(str1);
    free(str2);
    return retVal;
}
char *routeListToJSON(const List *routeList)
{
    
    if (routeList == NULL)
    {
        return "[]";
    }
    numberR = 1;
    char *string = (char *)malloc(sizeof(char) * 256);
    strcpy(string, "[");
    ListIterator iter = createIterator((List *)routeList);
    Route *rte;
    Route *last = getFromBack((List *)routeList);

    while ((rte = nextElement(&iter)) != NULL)
    {
        char *jsonRoute = routeToJSON(rte, &numberR);
        concatinateString(&string, jsonRoute);
        free(jsonRoute);
        if (compareRoutes(last, rte) != 0)
        {
            concatinateString(&string, ",");
        }
    }
    if (string[strlen(string) - 1] == ',')
    {
        string[strlen(string) - 1] = '\0';
    }
    concatinateString(&string, "]");
    return string;
}

char *trackListToJSON(const List *trackList)
{
    if (trackList == NULL)
    {
        return "[]";
    }
    numberT = 1;
    char *string = (char *)malloc(sizeof(char) * 256);
    strcpy(string, "[");
    ListIterator iter = createIterator((List *)trackList);
    Track *trk;
    Track *last = getFromBack((List *)trackList);

    while ((trk = nextElement(&iter)) != NULL)
    {
        char *jsonTrack = trackToJSON(trk, &numberT);
        concatinateString(&string, jsonTrack);
        free(jsonTrack);

        if (compareTracks(last, trk) != 0)
        {
            concatinateString(&string, ",");
        }
    }
    if (string[strlen(string) - 1] == ',')
    {
        string[strlen(string) - 1] = '\0';
    }
    concatinateString(&string, "]");

    return string;
}

char *GPXtoJSON(const GPXdoc *gpx)
{
    if (gpx == NULL)
    {
        return "{}";
    }
    char *string = (char *)malloc(sizeof(char) * 256);
    strcpy(string, "{\"version\":");
    char* version = OneDecimal2String(gpx->version);
    concatinateString(&string, version);
    concatinateString(&string, ",");
    free(version);

    concatinateString(&string, "\"creator\":");

    if (strcmp(gpx->creator, "") == VALID)
    {
        concatinateString(&string, "");
    }
    else
    {
        concatinateString(&string, "\"");
        concatinateString(&string, gpx->creator);
        concatinateString(&string, "\",");
    }

    concatinateString(&string, "\"numWaypoints\":");
    char *numWaypoints = int2String(getNumWaypoints(gpx));
    concatinateString(&string, numWaypoints);
    concatinateString(&string, ",");
    free(numWaypoints);

    concatinateString(&string, "\"numRoutes\":");
    char *numRoutes = int2String(getNumRoutes(gpx));
    concatinateString(&string, numRoutes);
    concatinateString(&string, ",");
    free(numRoutes);

    concatinateString(&string, "\"numTracks\":");
    char *numTracks = int2String(getNumTracks(gpx));
    concatinateString(&string, numTracks);
    free(numTracks);

    concatinateString(&string, "}");
    return string;
}
void addWaypoint(Route *rt, Waypoint *pt)
{
    if (rt == NULL || pt == NULL)
        return;
    insertBack(rt->waypoints, pt);
}
void addRoute(GPXdoc *doc, Route *rt)
{
    if (doc == NULL || rt == NULL)
        return;
    insertBack(doc->routes, rt);
}
void addGPXdataToRoute(Route *rt, GPXData *data)
{
    if (rt == NULL || data == NULL)
        return;
    insertBack(rt->otherData, data);
}
int getIndex(char *string, char c)
{
    for (int i = 0; i < strlen(string); i++)
    {
        if (string[i] == c)
        {
            return i;
        }
    }
    return -1;
}
char *stringCopy(int startIndex, int endIndex, char *source)
{
    char *toReturn = calloc(strlen(source) + 1, sizeof(char));
    if (startIndex >= endIndex)
    {
        return toReturn;
    }
    for (int i = startIndex; i < endIndex; i++)
    {
        toReturn[i - startIndex] = source[i];
    }
    return toReturn;
}
char *strip(char *string, char *toRemove)
{
    int startIndex = 0, endIndex = strlen(string) - 1;
    int counter = 0;
    for (int i = 0; i < strlen(string); i++)
    {
        counter = 0;
        for (int j = 0; j < strlen(toRemove); j++)
        {
            if (string[i] == toRemove[j])
            {
                startIndex++;
                counter++;
            }
        }
        if (counter == 0)
            break;
    }
    counter = 0;
    for (int i = strlen(string) - 1; i >= 0; i--)
    {
        counter = 0;
        for (int j = strlen(toRemove) - 1; j >= 0; j--)
        {
            if (string[i] == toRemove[j])
            {
                endIndex--;
                counter++;
            }
        }
        if (counter == 0)
            break;
    }
    char *stripped = calloc(strlen(string) + 1, sizeof(char));
    for (int i = startIndex; i <= endIndex; i++)
    {
        stripped[i - startIndex] = string[i];
    }
    return stripped;
}
GPXData *JSONtoGPXdata(const char *gpxString)
{
    if (gpxString == NULL)
        return NULL;
    char *gpx = (char *)gpxString;
    char *stripped = strip(gpx, "{\"}");

    char *token = strtok(stripped, ",");
    int index = getIndex(token, ':');
    char *name = stringCopy(index + 1, strlen(token), token);
    GPXData *doc = calloc(1, sizeof(GPXData) + 256);
    char* dataName = strip(name, "\"");
    strcpy(doc->name, dataName);
    free(dataName);
    free(name);
    

    token = strtok(NULL, ",");
    index = getIndex(token, ':');
    char *value = stringCopy(index + 1, strlen(token), token);
    char* dataValue = strip(value, "\"");
    strcpy(doc->value, dataValue);
    free(value);
    free(dataValue);
    free(stripped);

    if (strcmp(doc->name, "") == 0)
        return NULL;

    if (strcmp(doc->value, "") == 0)
        return NULL;

    return doc;
}
Waypoint *JSONtoWaypoint(const char *gpxString)
{
    if (gpxString == NULL)
        return NULL;
    char *gpx = (char *)gpxString;
    char *stripped = strip(gpx, "{\"}");

    char *token = strtok(stripped, ",");
    int index = getIndex(token, ':');
    char *name = stringCopy(index + 1, strlen(token), token);
    Waypoint *doc = initializeWaypoint();
    doc->name = strip(name, "\"");

    token = strtok(NULL, ",");
    index = getIndex(token, ':');
    char *lat = stringCopy(index + 1, strlen(token), token);
    doc->latitude = atof(lat);

    token = strtok(NULL, ",");
    index = getIndex(token, ':');
    char *lon = stringCopy(index + 1, strlen(token), token);
    doc->longitude = atof(lon);
    free(name);
    free(lat);
    free(lon);
    free(stripped);
    return doc;
}
Route *JSONtoRoute(const char *gpxString)
{
    if (gpxString == NULL)
        return NULL;
    char *gpx = (char *)gpxString;
    char *stripped = strip(gpx, "{\"}");

    char *token = strtok(stripped, ",");
    int index = getIndex(token, ':');
    char *name = stringCopy(index + 1, strlen(token), token);
    Route *data = calloc(1, sizeof(Route));
    data->name = strip(name, "\"");
    data->otherData = initializeList(gpxDataToString, deleteGpxData, compareGpxData);
    data->waypoints = initializeList(waypointToString, deleteWaypoint, compareWaypoints);
    free(name);
    free(stripped);
    return data;
}
GPXdoc *JSONtoGPX(const char *gpxString)
{
    if (gpxString == NULL)
        return NULL;
    char *gpx = (char *)gpxString;
    char *stripped = strip(gpx, "{\"}");
    char *token = strtok(stripped, ",");
    int index = getIndex(token, ':');
    char *namespace = stringCopy(index + 1, strlen(token), token);
    GPXdoc *doc = initializeGPXdoc();
    char* nSpace = strip(namespace, "\"");
    strcpy(doc->namespace, nSpace);
    free(nSpace);
    free(namespace);
    

    token = strtok(NULL, ",");
    index = getIndex(token, ':');
    char *version = stringCopy(index + 1, strlen(token), token);
    doc->version = atof(version);
    free(version);

    token = strtok(NULL, ",");
    index = getIndex(token, ':');
    char *creator = stringCopy(index + 1, strlen(token), token);
    doc->creator = strip(creator, "\"");
    free(creator);

    free(stripped);
    return doc;
}
char *gpxfiletoJSON(char *filename)
{
    GPXdoc *doc = createValidGPXdoc(filename, "./gpx.xsd");
    char *json = GPXtoJSON(doc);
    deleteGPXdoc(doc);
    return json;
}
int numOfChars(char *s)
{
    int n = 0;
    while (s[n] != '\0')
        ++n;
    return n;
}
char *insertString(char *s1, char *s2, int pos)
{
    int n1 = numOfChars(s1);
    int n2 = numOfChars(s2);

    if (n1 < pos)
        pos = n1;

    for (int i = 0; i < n1 - pos; i++)
    {
        s1[n1 + n2 - i - 1] = s1[n1 - i - 1];
    }
    for (int i = 0; i < n2; i++)
    {
        s1[pos + i] = s2[i];
    }
    s1[n1 + n2] = '\0';
    return s1;
}
char **tokenizeArray(char *line, char *delim, int *arraySize)
{
    char *string;
    int length = 1000;
    int lastCharacter = 0;
    char **tokens = malloc(sizeof(char *) * length);

    if (tokens == NULL)
    {
        printf("memory allocation error\n");
        exit(1);
    }
    //split command line into list of arguments
    string = strtok(line, delim);

    // add extra space if needed in case user ran out of it,
    // since we dont know how much will be entered to command line
    while (string)
    {
        *arraySize = *arraySize + 1;
        tokens[lastCharacter] = string;
        lastCharacter = lastCharacter + 1;

        if (lastCharacter >= length)
        {
            length = length + 1000;
            line = realloc(line, length * sizeof(char *));

            if (line == NULL)
            {
                printf("memory allocation error\n");
                exit(1);
            }
        }
        string = strtok('\0', delim);
    }
    tokens[lastCharacter] = '\0';
    return tokens;
}
char *gpxroutetoJSON(char *filename)
{
    GPXdoc *doc = createValidGPXdoc(filename, "./gpx.xsd");
    char *route = routeListToJSON(doc->routes);
    char *routeStripped = strip(route, "[]");
    free(route);
    deleteGPXdoc(doc);
    return routeStripped;
}
char *gpxtracktoJSON(char *filename)
{
    GPXdoc *doc = createValidGPXdoc(filename, "./gpx.xsd");
    char *tracks = trackListToJSON(doc->tracks);
    char *trackStripped = strip(tracks, "[]");
    free(tracks);
    deleteGPXdoc(doc);
    return trackStripped;
}

char *GPXViewtoJSON(char *filename)
{
    numberR = 1;
    numberT = 1;
    char *routes = gpxroutetoJSON(filename);
    char *tracks = gpxtracktoJSON(filename);
    char *str = (char *)malloc(sizeof(char) * 256);
    strcpy(str, "[");

    concatinateString(&str, routes);
    concatinateString(&str, ",");
    free(routes);
    concatinateString(&str, tracks);
    concatinateString(&str, "]");
    free(tracks);
    if (str[strlen(str) - 2] == ',')
    {
        str[strlen(str) - 2] = '\0';
        strcat(str, "]");
    }
    char *stripped = calloc(strlen(str) + 1, sizeof(char));
    if (str[1] == ',')
    {
        for (int i = 2; i < strlen(str); i++)
        {
            stripped[i - 2] = str[i];
        }
        stripped = insertString(stripped, "[", 0);
        free(str);
        return stripped;
    }
    free(stripped);
    return str;
}

char *createGPXFileFromJSON(char *filename, char *namespace, double version, char *creator, int numWaypoints, int numRoutes, int numTracks)
{

    char *str = (char *)malloc(sizeof(char) * 256);

    strcpy(str, "{\"namespace\":");
    concatinateString(&str, "\"");
    concatinateString(&str, namespace);
    concatinateString(&str, "\",");

    concatinateString(&str, "\"version\":");
    char *vers = OneDecimal2String(version);
    concatinateString(&str, vers);
    concatinateString(&str, ",");
    free(vers);

    concatinateString(&str, "\"creator\":");
    concatinateString(&str, "\"");
    concatinateString(&str, creator);
    concatinateString(&str, "\",");

    concatinateString(&str, "\"numWaypoints\":");
    char *wpts = int2String(numWaypoints);
    concatinateString(&str, wpts);
    concatinateString(&str, "\",");
    free(wpts);

    concatinateString(&str, "\"numRoutes\":");
    char *rts = int2String(numRoutes);
    concatinateString(&str, rts);
    concatinateString(&str, "\",");
    free(rts);
    concatinateString(&str, "\"numTracks\":");
    char *trk = int2String(numTracks);
    concatinateString(&str, trk);
    free(trk);

    concatinateString(&str, "}");

    GPXdoc *doc = JSONtoGPX(str);

    writeGPXdoc(doc, filename);
    return str;
}
char *LongFloat2String(double num)
{
    char *output = calloc(256, sizeof(char));
    sprintf(output, "%lf", num);
    return output;
}
char *int2String(int num)
{
    char *output = calloc(256, sizeof(char));
    sprintf(output, "%d", num);
    return output;
}
void addRouteFromJSON(char *filename, char *routeName, char *name, double latitude, double longtitude, char *dataName, char *dataValue)
{
    GPXdoc *doc = createValidGPXdoc(filename, "./gpx.xsd");
    char *str = (char *)malloc(sizeof(char) * 256);
    strcpy(str, "{\"name\":");
    if (strcmp(routeName, "") == VALID)
    {
        concatinateString(&str, "");
    }
    else
    {
        concatinateString(&str, "\"");
        concatinateString(&str, routeName);
        concatinateString(&str, "\"");
    }

    concatinateString(&str, "}");

    Route *route = JSONtoRoute(str);
    addRoute(doc, route);

    writeGPXdoc(doc, filename);
    deleteGPXdoc(doc);
    addGPXDataFromJSON(filename, routeName, dataName, dataValue);
    addWaypointFromJSON(filename, routeName, name, latitude, longtitude);
    free(str);
}
void addGPXDataFromJSON(char *filename, char *routeName, char *dataName, char *dataValue)
{
    GPXdoc *doc = createValidGPXdoc(filename, "./gpx.xsd");
    char *str = (char *)malloc(sizeof(char) * 256);
    strcpy(str, "{\"name\":");
    concatinateString(&str, "\"");
    concatinateString(&str, dataName);
    concatinateString(&str, "\",");

    concatinateString(&str, "\"value\":");
    concatinateString(&str, "\"");
    concatinateString(&str, dataValue);
    concatinateString(&str, "\"");
    concatinateString(&str, "}");

    GPXData *data = JSONtoGPXdata(str);
    Route *route = getRoute(doc, routeName);
    addGPXdataToRoute(route, data);
    writeGPXdoc(doc, filename); 
    deleteGPXdoc(doc);
    free(str);
}

char* addWaypointFromJSON(char *filename, char *routeName, char *name, double latitude, double longtitude)
{
    GPXdoc *doc = createValidGPXdoc(filename, "./gpx.xsd");
    char *str = (char *)malloc(sizeof(char) * 256);
    strcpy(str, "{\"name\":");
    if (strcmp(name, " ") == VALID)
    {
        concatinateString(&str, " ");
    }
    else
    {
        concatinateString(&str, "\"");
        concatinateString(&str, name);
        concatinateString(&str, "\",");
    }

    concatinateString(&str, "\"latitude\":");
    char *wpts = LongFloat2String(latitude);
    concatinateString(&str, wpts);
    concatinateString(&str, ",");
    free(wpts);

    concatinateString(&str, "\"longtitude\":");
    char *rts = LongFloat2String(longtitude);
    concatinateString(&str, rts);
    concatinateString(&str, "}");
    free(rts);

    Route *route = getRoute(doc, routeName);
    Waypoint *waypoint = JSONtoWaypoint(str);
        if(route == NULL) return "{\"check\":false}";
    addWaypoint(route, waypoint);
    writeGPXdoc(doc, filename);
    free(str);
    deleteGPXdoc(doc);
    return "{\"check\":true}";
}
char *routesBetweenFromJSON(char *filename, float lat1, float lat2, float lon1, float lon2, float delta)
{
    GPXdoc *doc = createValidGPXdoc(filename, "./gpx.xsd");
    List *list = getRoutesBetween(doc, lat1, lon1, lat2, lon2, delta);
    char *str = routeListToJSON(list);
    char *routeStripped = strip(str, "[]");
    if(strcmp(str,"") == 0) free(str);
    deleteGPXdoc(doc);
    return routeStripped;
}
char *TracksBetweenFromJSON(char *filename, float lat1, float lat2, float lon1, float lon2, float delta)
{
    GPXdoc *doc = createValidGPXdoc(filename, "./gpx.xsd");
    List *list = getTracksBetween(doc, lat1, lon1, lat2, lon2, delta);
    char *str = trackListToJSON(list);
    char *trackStripped = strip(str, "[]");
    if(strcmp(str,"") == 0) free(str);
    deleteGPXdoc(doc);
    return trackStripped;
}
char *routesBetweenFromJSON2(char *filename, float lat1, float lat2, float lon1, float lon2, float delta)
{
   GPXdoc *doc = createValidGPXdoc(filename,"./gpx.xsd");
    List *list = getRoutesBetween(doc, lat1, lon1, lat2, lon2, delta);
    char *str = routeListToJSON(list);
    return str;
}
char *TracksBetweenFromJSON2(char *filename, float lat1, float lat2, float lon1, float lon2, float delta)
{
    GPXdoc *doc = createValidGPXdoc(filename,"./gpx.xsd");
    List *list = getTracksBetween(doc, lat1, lon1, lat2, lon2, delta);
    char *str = trackListToJSON(list);
    return str;
}
char* findBetween(char *filename, float lat1, float lat2, float lon1, float lon2, float delta)
{
    char* routes = routesBetweenFromJSON(filename,lat1,lat2,lon1,lon2,delta);
    char* tracks = TracksBetweenFromJSON(filename,lat1,lat2,lon1,lon2,delta);
    char *str = (char *)malloc(sizeof(char) * 256);
    strcpy(str, "[");

    concatinateString(&str, routes);
    concatinateString(&str, ",");
    free(routes);
    concatinateString(&str, tracks);
    concatinateString(&str, "]");
    free(tracks);
    if (str[strlen(str) - 2] == ',')
    {
        str[strlen(str) - 2] = '\0';
        strcat(str, "]");
    }
    char *stripped = calloc(strlen(str) + 1, sizeof(char));
    if (str[1] == ',')
    {
        for (int i = 2; i < strlen(str); i++)
        {
            stripped[i - 2] = str[i];
        }
        stripped = insertString(stripped, "[", 0);
        free(str);
        return stripped;
    }
    free(stripped);
    return str;
}
char *renameRoute(char *filename, char *originalName, char *newName)
{
    GPXdoc *doc = createValidGPXdoc(filename, "./gpx.xsd");
    Route *route = getRoute(doc, originalName);
    if (route == NULL)
        return "{\"check\":false}";
    strcpy(route->name, newName);
    writeGPXdoc(doc, filename);
    deleteGPXdoc(doc);
    return "{\"check\":true}";
}

char *renameTrack(char *filename, char *originalName, char *newName)
{
    GPXdoc *doc = createValidGPXdoc(filename, "./gpx.xsd");
    Track *track = getTrack(doc, originalName);
    if (track == NULL)
        return "{\"check\":false}";
    strcpy(track->name, newName);
    writeGPXdoc(doc, filename);
    deleteGPXdoc(doc);
    return "{\"check\":true}";
}
char *GPXDataToJSON(const GPXData *data)
{

    if (data == NULL)
    {
        return "{}";
    }

    char *string = (char *)malloc(sizeof(char) * 256);
    strcpy(string, "{\"name\":");

    if (strcmp(data->name, "") == VALID)
    {
        concatinateString(&string, "\"None\"");
    }
    else
    {
        concatinateString(&string, "\"");
        concatinateString(&string, (char *)data->name);
        concatinateString(&string, "\",");
    }

    concatinateString(&string, "\"value\":");
    char *value = strip((char *)data->value, " \n");
    if (strcmp(value, "") == VALID)
    {
        concatinateString(&string, "");
    }
    else
    {
        concatinateString(&string, "\"");
        concatinateString(&string, value);
        concatinateString(&string, "\"");
    }

    concatinateString(&string, "}");
    free(value);
    return string;
}
char *GPXDataListToJSON(const List *dataList)
{
    if (dataList == NULL)
    {
        return "[]";
    }
    char *string = (char *)malloc(sizeof(char) * 256);
    strcpy(string, "[");
    ListIterator iter = createIterator((List *)dataList);
    GPXData *data;
    GPXData *last = getFromBack((List *)dataList);

    while ((data = nextElement(&iter)) != NULL)
    {
        char *jsonData = GPXDataToJSON(data);
        concatinateString(&string, jsonData);
        free(jsonData);

        if (compareGpxData(last, data) != 0)
        {
            concatinateString(&string, ",");
        }
    }
    concatinateString(&string, "]");
    return string;
}
char *ShowRouteData(char *filename, char *routeName)
{
    GPXdoc *doc = createValidGPXdoc(filename, "./gpx.xsd");
    Route *route = getRoute(doc, routeName);
    if(route == NULL) return NULL;
    char *string = NULL;
    char *stringList = malloc(sizeof(char) * 256);
    strcpy(stringList, "[");
   
    Waypoint *waypointList;

    // get none empty route names and count them into the finalOther count
    string = GPXDataListToJSON(route->otherData);
    if (strcmp(string, "[]") != 0)
    {
        char *stripped = strip(string, "[]");
        concatinateString(&stringList, stripped);
        free(stripped);
    }
    free(string);

    ListIterator wptIter = createIterator(route->waypoints);
    void *wpts;
    while ((wpts = nextElement(&wptIter)) != NULL)
    {
        // get none empty route waypoint names and count them into the finalOther count
        waypointList = (Waypoint *)wpts;
        string = GPXDataListToJSON(waypointList->otherData);
        if (strcmp(string, "[]") != 0)
        {
            char *stripped = strip(string, "[]");
            concatinateString(&stringList, stripped);
            free(stripped);
        }
        free(string);
    }

    concatinateString(&stringList, "]");
    if (stringList[strlen(stringList) - 2] == ',')
    {
        stringList[strlen(stringList) - 2] = '\0';
        concatinateString(&stringList, "]");
    }
    char *stripped = calloc(strlen(stringList) + 1, sizeof(char));
    if (stringList[1] == ',')
    {
        for (int i = 2; i < strlen(stringList); i++)
        {
            stripped[i - 2] = stringList[i];
        }
        stripped = insertString(stripped, "[", 0);
        return stripped;
    }
    free(stripped);
    deleteGPXdoc(doc);
    return stringList;
}
char *ShowTrackData(char *filename, char *trackName)
{
    GPXdoc *doc = createValidGPXdoc(filename, "./gpx.xsd");
    Track *track = getTrack(doc, trackName);
    if(track == NULL) return NULL;
    char *string = NULL;
    char *stringList = malloc(sizeof(char) * 256);
    strcpy(stringList, "[");
   
    TrackSegment *trkSegList;
    Waypoint *waypointList;

    // get none empty route names and count them into the finalOther count
    string = GPXDataListToJSON(track->otherData);

    if (strcmp(string, "[]") != 0)
    {
        char *stripped = strip(string, "[]");
        concatinateString(&stringList, stripped);
        free(stripped);
    }
    free(string);

    ListIterator segIter = createIterator(track->segments);
    void *segs;
    while ((segs = nextElement(&segIter)) != NULL)
    {
        trkSegList = (TrackSegment *)segs;
        ListIterator wptIter = createIterator(trkSegList->waypoints);
        void *wpts;
        while ((wpts = nextElement(&wptIter)) != NULL)
        {
            // get none empty track waypoint names and count them into the finalOther count
            waypointList = (Waypoint *)wpts;
            string = GPXDataListToJSON(waypointList->otherData);
            if (strcmp(string, "[]") != 0)
            {
                char *stripped = strip(string, "[]");
                concatinateString(&stringList, stripped);
                free(stripped);
            }
            free(string);
        }
    }

    concatinateString(&stringList, "]");
    if (stringList[strlen(stringList) - 2] == ',')
    {
        stringList[strlen(stringList) - 2] = '\0';
        concatinateString(&stringList, "]");
    }
    char *stripped = calloc(strlen(stringList) + 1, sizeof(char));
    if (stringList[1] == ',')
    {
        for (int i = 2; i < strlen(stringList); i++)
        {
            stripped[i - 2] = stringList[i];
        }
        stripped = insertString(stripped, "[", 0);
        return stripped;
    }
    free(stripped);
    deleteGPXdoc(doc);
    return stringList;
}

char *WaypointToJSON(const Waypoint *wpt)
{
    if (wpt == NULL)
    {
        return "{}";
    }
    char *str = (char *)malloc(sizeof(char) * 256);
    strcpy(str, "{\"name\":");

    if (strcmp(wpt->name, "") == 0)
    {
        concatinateString(&str, "\"\"");
    }
    else
    {
        concatinateString(&str, "\"");
        concatinateString(&str, wpt->name);
        concatinateString(&str, "\"");
    }
    concatinateString(&str, ",");

    
    concatinateString(&str, "\"latitude\":");
    char *wpts = LongFloat2String(wpt->latitude);
    concatinateString(&str, wpts);
    concatinateString(&str, ",");
    free(wpts);

    concatinateString(&str, "\"longtitude\":");
    char *rts = LongFloat2String(wpt->longitude);
    concatinateString(&str, rts);
    concatinateString(&str, ",");
    free(rts);

    concatinateString(&str, "\"pointIndex\":");
    char *pointIndex = int2String(numberW);
    concatinateString(&str, pointIndex);
    concatinateString(&str, "}");
    free(pointIndex);
    numberW = numberW +1;
    return str;
}

char *WaypointListToJSON(const List *waypointList)
{
    if (waypointList == NULL)
    {
        return "[]";
    }
    numberW = 0;
    char *string = (char *)malloc(sizeof(char) * 256);
    strcpy(string, "[");
    ListIterator iter = createIterator((List *)waypointList);
    Waypoint *rte;
    while ((rte = nextElement(&iter)) != NULL)
    {
        char *jsonWpt = WaypointToJSON(rte);
        concatinateString(&string, jsonWpt);
        free(jsonWpt);
        concatinateString(&string, ",");  
    }
    if (string[strlen(string) - 1] == ',')
    {
        string[strlen(string) - 1] = '\0';
    }
    concatinateString(&string, "]");
    return string;
}
char *waypointsFromJSON(char *filename,char* name)
{
    GPXdoc *doc = createValidGPXdoc(filename,"./gpx.xsd");
    Route* route = getRoute(doc,name);
    char *str = WaypointListToJSON(route->waypoints);
    deleteGPXdoc(doc);
    return str;
}
float routeLenFromJSON(char* name, char* filename)
{
    GPXdoc *doc = createValidGPXdoc(filename,"./gpx.xsd");
    Route* route = getRoute(doc,name);
    float len = round10(getRouteLen(route));
    deleteGPXdoc(doc);
    return len;
}