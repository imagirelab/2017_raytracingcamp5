#ifndef RENDERER_H
#define RENDERER_H

class renderer{
private:
	int steps_ = 0;
	int WIDTH;
	int HEIGHT;
public:
	renderer(int w, int h);
	~renderer();
	
	int update(double *dest, const double *src);
};

#endif // !RENDERER_H