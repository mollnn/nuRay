#include <bits/stdc++.h>
using namespace std;

#include "minipt.hpp"

int main(int argc, char *argv[])
{
	Minipt minipt;
	minipt.LoadDefaultLightset();
	minipt.ImportObject("cube.obj");
	minipt.AutoNormal();
	Image image = minipt.Render();
	image.WriteToTGA("output.tga");
}