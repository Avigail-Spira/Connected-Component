# Connected-Component

In this project, I scan the image and count how many connected components there are. A connected component is an independent object which isn't connected to another object.   
I check each pixel's neighbors (either all 8 or just 4) and determine if it is connected to an object, the start of a new one, or part of the background.  
It is a three step process to determine if it is connected or not. 

Each connected component is given a label. The output files show the count of connected components and visualizations for it.
