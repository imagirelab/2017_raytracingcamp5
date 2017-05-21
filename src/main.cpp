#include <iostream>
#include <thread>
#include <time.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../sdk/stb/stb_image_write.h"

#include "renderer.h"


// おおよそ30秒毎に、レンダリングの途中経過をbmpかpngで連番(000.png, 001.png, ...) で出力してください。
// 4分33秒以内に自動で終了してください。

#define WIDTH 1980
#define HEIGHT 1080

#define OUTPUT_INTERVAL 30
#define FINISH_TIME (4 * 60 + 33)
#define FINISH_MARGIN 2

void save(const double *data, unsigned char *buf, const char *filename, int steps)
{
	const double coeff = 1.0 / (10.1 * (double)steps);
	#pragma omp parallel for
	for (int i = 0; i < 3 * WIDTH * HEIGHT; i++) {
		double tmp = 1.0 - exp(-data[i] * coeff);// tone mapping
		buf[i] = (unsigned char)(pow(tmp, 1.0/2.2) * 255.999);// gamma correct
	}

	// save
	int w = WIDTH;
	int h = HEIGHT;
	int comp = 3; // RGB
	int stride_in_bytes = 3 * w;
	int result = stbi_write_png(filename, w, h, comp, buf, stride_in_bytes);
}

int main()
{
	time_t t0 = time(NULL);
	time_t t_last = 0;
	int count = 0;

	unsigned char *image = new unsigned char[3 * WIDTH * HEIGHT];

	// frame buffer の初期化
	int current = 0;
	double *fb[2];
	fb[0] = new double[3 * WIDTH * HEIGHT];
	fb[1] = new double[3 * WIDTH * HEIGHT];
	double *fb0 = fb[0], *fb1 = fb[1];
	#pragma omp parallel for
	for (int i = 0; i < 3 * WIDTH * HEIGHT; i++) {
		fb0[i] = fb1[i] = 0;
	}

	renderer *pRenderer = new renderer(WIDTH, HEIGHT);

	do
	{
		// fb[1-current] を読み込んで fb[current]にレンダリング
//		std::this_thread::sleep_for(std::chrono::seconds(1));
		int steps = pRenderer->update(fb[1 - current], fb[current]);

		// swap
		current = 1 - current;

		// 30 秒ごとに出力
		time_t t = time(NULL) - t0;
		int c = (int)(t / OUTPUT_INTERVAL);
		if (count < c) {
			char filename[256];
			snprintf(filename, 256, "%d.png", c);
			save(fb[1 - current], image, filename, steps);
			count++;
		}

		// 4分33秒以内に終了なので、前のフレームを考えてオーバーしそうならば終了する
		if (FINISH_TIME - FINISH_MARGIN <= t + (t - t_last)) break;

		t_last = t;
	}while (true);

	delete pRenderer;
	delete[] image;
	delete[] fb[0];
	delete[] fb[1];

	return 0;
}

