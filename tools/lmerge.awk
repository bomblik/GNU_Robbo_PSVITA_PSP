#!/usr/bin/awk -f
#
# this is level merger for gnu robbo - awk part
# written by neurocyp (at) gmail (dot) com
#
BEGIN {
maxbright=240
minbright=90
lvl=1;
if(lastlevel<=0) lastlevel=32;
red=80;
green=80;
blue=40;
printf "[name]\nWarsaw\n";
printf "[last_level]\n%d\n",lastlevel;

}

/^maxlevel= / {
lastlevel=$2
}

/\[name\]/ {
getline;
getline;
}
/\[last_level\]/ {
getline;
getline;
}


/\[level\]/ {
print;
getline;
printf "%d\n",lvl;
lvl++;
getline;
}

/\[colour\]/ {
print;
getline;
red=rand()*128+16;
blue=rand()*128+16;
green=rand()*128+16;
bright=(red+blue_green)/3;

if(bright>maxbright) {
s=maxbright-bright;
red=red-s;
blue=blue-s;
green=green-s;
} else if (bright<minbright) {
s=minbright-bright;
red=red+s;
blue=blue+s;
green=green+s;
}

red=red%255;
green=green%255;
blue=blue%255;
	
printf "%x%x%x\n",red,green,blue;

getline;
}

{
print ;
}

