Descripts
    The goal of this Project is to calcualte the score of ROIs, which used to recommend travel trajectories.
    First, read trajectories history and region data from given database.
    Second, tranform the trajectories to a sequences of potential regions.
    Third, Tacke out the edges from sequences, and calculate their weight.
    Finally, calculate scores of each region by summarizing all linked ROIs multiplied by their weight and proportion 

Input
    The input are 2 database, trajectory and region.
    Trajectory is a database that contains sequences of points for each trajectory.
        format: tid, tid, index, lon ,lat
    Region is a database that contains region id, rang, and density, which means the number of trajectory that passes through this region
        format: rid, density, range

Output
    The output is the text file that contains ROIs and their score
        format:	rid, score
    
Language
    C++, php
    
File
    1_preprocess.php
        Transform geo trajectories to lists of potential regions
    2_filterTraDB.cpp
        Filter repeating Rid in each trajectory and trajectories that do not have regions
    3_linkR.cpp
        Generate in links and out links of each region
    4_importEdge.php
        Insert all edge to DB grid3x_edge
    5_weight.php
        Calculate weight of each edge
    6_scorePR_new.cpp
        Compute scores of each PR from links