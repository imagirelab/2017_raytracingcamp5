#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define _CRT_SECURE_NO_WARNINGS

// VCでのリークチェック（_CrtSetDbgFlagも有効に）
//#define _CRTDBG_MAP_ALLOC #include <stdlib.h> #include <crtdbg.h>
// Visual Leak Detector でのチェック
// #include <vld.h>

#include <iostream>
#include <thread>
#include <time.h>
#include <omp.h>

#include "../sdk/stb/stb_image.h"
#include "../sdk/stb/stb_image_write.h"

#include "renderer.h"


// おおよそ30秒毎に、レンダリングの途中経過をbmpかpngで連番(000.png, 001.png, ...) で出力してください。
// 4分33秒以内に自動で終了してください。

//#define WIDTH 200
//#define HEIGHT 100
#define WIDTH 1920
#define HEIGHT 1080

#define OUTPUT_INTERVAL 30
#define FINISH_TIME (4 * 60 + 33)
#define FINISH_MARGIN 2

void save(const double *data, unsigned char *buf, const char *filename, int steps)
{
	const double coeff = 1.0 / (10.1 * (double)steps);
	
	#pragma omp parallel
	{
		#pragma omp for
		for (int i = 0; i < WIDTH * HEIGHT; i++) {
			int src = i * 3;
			double tmp0 = data[src + 0] / (double)steps;// tone mapping
			double tmp1 = data[src + 1] / (double)steps;// tone mapping
			double tmp2 = data[src + 2] / (double)steps;// tone mapping
			int idx = i << 2;
			//		double tmp = 1.0 - exp(-data[i] * coeff);// tone mapping
			buf[idx + 0] = (unsigned char)(pow(tmp0, 1.0 / 2.2) * 255.999);// gamma correct
			buf[idx + 1] = (unsigned char)(pow(tmp1, 1.0 / 2.2) * 255.999);// gamma correct
			buf[idx + 2] = (unsigned char)(pow(tmp2, 1.0 / 2.2) * 255.999);// gamma correct
			buf[idx + 3] = 0xff;
		}
	}

	// save
	int w = WIDTH;
	int h = HEIGHT;
	int comp = STBI_rgb_alpha; // RGB
	int stride_in_bytes = 4 * w;
	int result = stbi_write_png(filename, w, h, comp, buf, stride_in_bytes);
}

static void initFB(double *fb)
{
	#pragma omp parallel for
	for (int i = 0; i < 3 * WIDTH * HEIGHT; i++) {
		fb[i] = 0.0;
	}
}

int main()
{
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);

	time_t t0 = time(NULL);
	time_t t_last = 0;
	int count = 0;

	unsigned char *image = new unsigned char[4 * WIDTH * HEIGHT];

	// frame buffer の初期化
	int current = 0;
	double *fb[2];
	fb[0] = new double[3 * WIDTH * HEIGHT];
	fb[1] = new double[3 * WIDTH * HEIGHT];
	initFB(fb[0]);
	initFB(fb[1]);

	renderer *pRenderer = new renderer(WIDTH, HEIGHT);

	int steps = 0;
	do
	{
		// fb[1-current] を読み込んで fb[current]にレンダリング
//		std::this_thread::sleep_for(std::chrono::seconds(1));
		pRenderer->update(fb[1 - current], fb[current]);

		steps++;

		// swap
		current = 1 - current;

		// 30 秒ごとに出力
		time_t t = time(NULL) - t0;
		int c = (int)(t / OUTPUT_INTERVAL);
		if (count < c) {
			char filename[256] = { '0', '0', '.', 'p', 'n', 'g', '\0' };
			filename[0] = '0' + (c / 10);
			filename[1] = '0' + (c % 10);
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

