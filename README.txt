This Project is to read jrajectory and region data, generate scores of each rerion

input
    trajectory
        format: tid, (tid, index, lon ,lat)¡K
    region
        format: rid, density, range

output
    rid, score
    
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