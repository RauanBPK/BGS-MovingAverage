# BGS-MovingAverage
Simple background subtraction algorithm (simple!). Finds contours, some morphological operations, uses convexHull (see Opencv) and uses it as a mask for extracting
moving parts of the video.

!Good(ish) for static cameras only. (like, reaaally static)!
Compile with -O3 (y)

Estimated BG - Current Frame - Moving parts - Mask 
![Demomain](https://github.com/RauanBPK/BGS-MovingAverage/blob/master/demo.png)

The parameters are hard-coded. ( yeah, sorry)
Not the best piece of code i've written, but ok.
(:
