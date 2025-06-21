#pragma once

#include <stdio.h>
#include <unistd.h>

typedef void(* callback_t)(double, double);

//void ProgBar();
void ProgBar(double total, double cur);