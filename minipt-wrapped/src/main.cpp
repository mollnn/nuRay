#include <bits/stdc++.h>
using namespace std;

#include "minipt.hpp"

int main(int argc, char *argv[])
{
	Minipt minipt;
	TestScene ts;
	ts.Load(minipt.scene);
	minipt.AutoNormal();
	Image image = minipt.Render();
	image.WriteToTGA("output.tga");
}