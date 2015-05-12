short emptyCell[36] = {
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0,
    0,0,0,0,0,0};

short player[36] = { //Main Fighter
    1,1,0,0,1,1,
    1,0,1,1,0,1,
    0,1,0,0,1,0,
    0,1,1,1,1,0,
    1,0,1,1,0,1,
    1,1,0,0,1,1};

short starPlayer[36] = {
    1,0,1,1,0,1,
    0,1,0,0,1,0,
    1,1,1,1,1,1,
    0,1,0,0,1,0,
    0,1,1,1,1,0,
    1,0,1,1,0,1};
	
short diamond[36] = {
    0,0,0,1,0,0,
    0,0,1,1,1,0,
    0,1,1,1,1,1,
    0,0,1,1,1,0,
    0,0,0,1,0,0,
    0,0,0,0,0,0};

short obstacle[36] = {
    0,0,1,1,0,0,
    0,1,0,0,1,0,
    1,0,0,0,0,1,
    1,0,0,0,0,1,
    0,1,0,0,1,0,
    0,0,1,1,0,0};

short apple[36] = {
    0,0,0,1,0,0,
    0,0,1,0,0,0,
    0,1,1,1,1,0,
    0,1,1,1,1,0,
    0,0,1,1,0,0,
    0,0,0,0,0,0};


void drawObject(short initialX, short initialY, short object[36]) {
    short i = 0;
    
    long absoluteX = initialX * 6 + 3;
    long absoluteY = initialY * 6 + 3;
    
    for(long y = absoluteY; y < absoluteY+6; ++y)
    {
        for(long x = absoluteX; x < absoluteX+6; ++x)
        {
	        updatePixel(x,y,object[i++]);
        }
    }
}

