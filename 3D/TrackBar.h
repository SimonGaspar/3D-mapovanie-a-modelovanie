#pragma once
void ChangeTrackbarSADWindowSize(int p, void* data) {
	int TEMP = sadWindowSize;
	if (TEMP <= 255 && TEMP >= 5) {
		sadWindowSize = TEMP % 2 == 0 ? TEMP + 1 : TEMP;
	}
	else {
		sadWindowSize = TEMP > 255 ? 255 : 5;
	}
}

void ChangeTrackbarNumDisparities(int p, void* data) {
	int TEMP = numDisparities;
	if (TEMP < 16) {
		numDisparities = 16;
	}
	else {
		numDisparities = TEMP % 16 == 0 ? TEMP : TEMP - TEMP % 16;
	}
}

void ChangeTrackbarPreFilterCap(int p, void* data) {
	int TEMP = preFilterCap;
	preFilterCap = TEMP > 63 ? TEMP : 63;
	TEMP = preFilterCap;
	preFilterCap = TEMP < 1 ? 1 : TEMP;
}