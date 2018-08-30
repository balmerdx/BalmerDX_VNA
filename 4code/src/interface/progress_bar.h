#pragma once

void ProgressInit(int x, int y, int dx, int dy);

//Когда показываем progress - сбрасываем x=0
void ProgressSetVisible(bool show);
bool ProgressVisible();

//pos=[0..1]
void ProgressSetPos(float pos);
float ProgressPos();
