
float lui_mix(float ratio, float min, float max) {
	return min + (max - min) * ratio;
}

float lui_unmix(float val, float min, float max) {
	return (val - min) / (max - min);
}

float lui_remix(float val, float val_min, float val_max, float min, float max) {
	return lui_mix(lui_unmix(val,val_min,val_max),min,max);
}

float lui_clip(float val, float min, float max) {
	return val < min ? min : val > max ? max : val;
}