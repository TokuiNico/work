The goal of this Project is to calcualte the score of ROIs, which used to recommend travel trajectories.
First, read trajectories history and region data from given database.
Second, tranform the trajectories to a sequences of ROIs.
Third, Tacke out the edges from sequences of ROIs, and calculate their weight.
Finally, calculate scores of each region by summarizing all linked ROIs multiplied by their weight and proportion 

input
    trajectory	the travel trajectories database
		format: tid, (tid, index, lon ,lat)...
    region		the regions that contains density of number of passing trajectories
        format: rid, density, range

output
	the region id and its updated score
		format:	rid, score
    
Language
    C++, php
    
File
    1_preprocess.php
        Transform geo trajectories to several lists of potential regions
    2_filterTraDB.cpp
        Filter ¡§repeating¡¨ Rid in each trajectory
        Filter trajectories that do not have regions
    3_linkR.cpp
        Generate link list among PR
    4_importEdge.php
        Insert all edge to DB grid3x_edge
    5_weight.php
        Calculate weight of each edge
    6_scorePR_new.cpp
        Compute scores of each PR from links