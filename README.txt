the commando format:
./OSMMain [Address] [Mode] [ExtendRadius]

Address: paricular address of osm-file
Mode: use the extend mode (Y/N). The default value is "N"
ExtendRadius: if use the extend mode, the nodes and ways are loaded into graph when 
              they are in the radius of a route. The default value is 500

Example: you have a germany.osm file in the directory germany.
you can run the program in non-extend mode like:
./OSMMain germany/germany.osm N
or
./OSMMain germany/germany.osm
If you run the program with "./OSMMain germany/germany.osm 50", the program will run in non-extend mode.