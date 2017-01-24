the command format:
./ExtractionFromOSMMain [Address] [Mode] [ExtendRadius]

Address: paricular address of osm-file
Mode: use the extend mode (Y/N). The default value is "N"
ExtendRadius: if using the extend mode, the nodes and ways are loaded into graph when 
              they are in the radius of a route. The default value is 500

Example: you have a germany.osm file in the directory germany.
you can run the program in non-extend mode like:
./ExtractionFromOSMMain germany/germany.osm N
or
./ExtractionFromOSMMain germany/germany.osm
If you run the program with "./ExtractionFromOSMMain germany/germany.osm 50", the program will run in non-extend mode.