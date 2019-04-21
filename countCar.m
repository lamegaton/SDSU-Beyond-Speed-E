poleDistance = 100;% m
time_interval = 120;% second
avrCarLen = 5;% m
avrSpace = 1;
minSpeed = 15/2.237; %15 miles/sec
maxSpeed = 45/2.237; %45 miles/sec

car = avrCarLen + avrSpace;
% numbers of car can fit in 100m pole distance
maxCars = poleDistance / car;
%time takes to pass with maxSpeed:
timePassMax = poleDistance/maxSpeed;
%max numbers of car with maxSpeed:
NumbCarMaxSpeed = time_interval/timePassMax;

%time takes to pass with minSpeed:
timePassMin = poleDistance/minSpeed;
%max numbers of car with minSpeed:
NumbCarMinSpeed = time_interval/timePassMin;
