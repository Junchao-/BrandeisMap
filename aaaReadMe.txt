http://www.cs.brandeis.edu/~storer/cs180/Assignments/BrandeisMap/Project.html

___________________________________________________________________


HOW TO COMPILE Map.c:

1.
Change to the map directory; for example, something like:
cd /Users/home/SRC/BrandeisMap

2.
Compile the assignment template code:
gcc -ansi -Wall -o Map Map.c

___________________________________________________________________


TO RUN:

1. Open MatLab, load Display.m, get rid of all windows except the one
   to run the program, and make that a little window in the lower left
   corner of the screen (just large enough so you can click on the run arrow).

2. Start Map, which opens a terminal window, and reposition and size the window
   so it is on the left side of the screen.

   OR: If the program was compiled without setting up the PATH
       variable at the beginning of the code,
       open a terminal window change directory to the Map folder,
       and and start Map from a command line by typine ./Map.

3. Now you can repeatedly run Map and then run Matlab to see the
   path on the Brandeis map.

___________________________________________________________________

THE ASSIGNMENT:

Assignment.pdf

AssignmentPart2.pdf

___________________________________________________________________

THE TWO FILES TO BE PASSED IN:

Map.c
Base code for the assignment that must be completed.

Tour.h
Place code here for the assignment Part 2.

___________________________________________________________________

MODIFY IF NECESSARY:

MapPATH.h
A character string for the directory path to where the provided
input and output functions will look to open and save files.

___________________________________________________________________

VIEW THESE BUT DO NOT MODIFY THEM:

Map.h
Declarations of global constants, variables, and arrays for the map data
that are initialized when the program starts.

aaaReadme.txt
This file.

___________________________________________________________________

COMMON FILES - DO NOT MODIFY THESE:

BrandeisMapLabeled.jpg
BrandeisMapLabeledCropped.jpg
Labeled and cropped versions of Brandeis map.

MapData.h
Functions to input the map data and compute time.

MapDataEdges.txt
MapDataVertices.txt
Map data files that are read in when the program starts.

MapInput.h
MapOutput.h
User input and output functions.

Display.m
DisplayArrows.m
DisplayCropped.m
DisplayCroppedArrows.m
MatLab programs to display path on BrandeisMapLabeled or BrandeisMapLabeledCropped,
with or without arrows on the edges.

Project.html
Web page for this assignment.

___________________________________________________________________

SAMPLE OUTPUT:

SampleOutputs
A folder with some sample outputs with the corresponding paths and maps.

Solutions.txt
Output from the assignment tests.

___________________________________________________________________

FILES GENERATED WHEN Map IS RUN:

Route.txt (used by Display.m and DisplayArrows.m)
RouteCropped.txt (used by DisplayCropped.m and DisplayCroppedArrows.m)
RouteEdges.txt (a list of the edges on the path that can be used for debugging)

