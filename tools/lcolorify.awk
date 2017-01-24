#!/usr/bin/awk -f
#
# this is level colorifier for GNU Robbo
# it uses ranodm colors, that have brightness between maxbrightness and minbrightness
# 

BEGIN {
maxbright=240
minbright=90
lvl=1;
red=80;
green=80;
blue=40;

}


# just in case we fix (if broken) level numbers
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

