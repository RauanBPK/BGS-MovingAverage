# BGS-MovingAverage
Simple background subtraction algorithm (simple!). Finds contours, some morphological operations, uses convexHull (see Opencv) and uses it as a mask for extracting
moving parts of the video.

Works good with static cameras only!
Compile with -O3 (y)

Veja o ![Artigo/Article](https://github.com/RauanBPK/BGS-MovingAverage/blob/master/Trabalho_6___Background_subtraction.pdf) para compreender o funcionamento do algoritmo

Estimated BG - Current Frame - Moving parts - Mask 
![Demo-main](https://github.com/RauanBPK/BGS-MovingAverage/blob/master/demo.png)

The parameters are hard-coded.

Not very organized, but proud of this work anyway. :)

Video used: https://www.youtube.com/watch?v=dTdsjKRyMuU
