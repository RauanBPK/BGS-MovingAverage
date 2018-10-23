# BGS-MovingAverage
Simple background subtraction algorithm (simple!). Finds contours, some morphological operations, uses convexHull (see Opencv) and uses it as a mask for extracting
moving parts of the video.

!Good(ish) for static cameras only. (like, reaaally static)!
Compile with -O3 (y)

Estimated BG - Current Frame - Moving parts - Mask 
![Demomain](https://github.com/RauanBPK/BGS-MovingAverage/blob/countCars/demo2.png)

The parameters are hard-coded. ( yeah, sorry)
Not the best piece of code i've written, but ok.
(:

Video used: https://www.youtube.com/watch?v=dTdsjKRyMuU

* Now it detects passing cars. The detector is hard coded as well.. gotta change it later. Well, this was just for fun anyway.
* Just checks if the center point of each bounding box intersects with the detector (blue rectangle)
* Doesn't track or label blobs. Pretty simple.
