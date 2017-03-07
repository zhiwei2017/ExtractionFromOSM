#ExtractionFromOSM
This is the source code of my bachelor thesis "public-transit data extraction from OpenStreetMap Data"

##Introduction
Built a network using public-transit data extracted from OpenStreetMap data in Europe and Planet, then completed its inside information through solving various problems, such as wrong orders, existence of gaps, missing labels, poor structure, using bidirectional A*, A*, Dijkstra, SVMs, R-Tree, DFS, finally transformed the network into a General Transit Feed Specification feed which simulates a transport system and can be displayed in TRAVIC. This feed and software OpenTripPlaner together build a public-transit- combination query system for the whole Europe as well as the Planet.

##Results
there are four pictures which show the results, you can find them inthe folder
>**ExtractionFromOSM/display results**

##Run the program
To run this program you need to download a osm file from http://download.geofabrik.de
**get the code**
```
git clone https://github.com/zhiwei2017/ExtractionFromOSM
```
**Generate executable file:**
```
make
./ExtractionFromOSMMain [Address] [Mode] [ExtendRadius]
```
>*Address*: paricular address of osm-file
>*Mode*: use the extend mode (Y/N). The default value is "N"
>*ExtendRadius*: if using the extend mode, the nodes and ways are loaded into graph when 
              they are in the radius of a route. The default value is 500



**Example:**
you have a germany.osm file in the directory germany.
you can run the program in *non-extend mode* like:
```
./ExtractionFromOSMMain germany/germany.osm N
```
or
```
./ExtractionFromOSMMain germany/germany.osm
```
If you run the program with 
```
./ExtractionFromOSMMain germany/germany.osm 50
```
, the program will run in non-extend mode.
