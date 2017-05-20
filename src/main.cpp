#include <iostream>
#include <thread>
#include <time.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../sdk/stb/stb_image_write.h"

// おおよそ30秒毎に、レンダリングの途中経過をbmpかpngで連番(000.png, 001.png, ...) で出力してください。
// 4分33秒以内に自動で終了してください。

#define WIDTH 256
#define HEIGHT 256

#define OUTPUT_INTERVAL 30
#define FINISH_TIME (4 * 60 + 33)

void save(const char *data, const char *filename)
{
	int w = WIDTH;
	int h = HEIGHT;
	int comp = 3; // RGB
	int stride_in_bytes = 3 * w;
	int result = stbi_write_png(filename, w, h, comp, data, stride_in_bytes);
}

int main()
{
	time_t t0 = time(NULL);
	time_t t_last = 0;
	int count = 0;

	// frame buffer の初期化
	int current = 0;
	char *fb[2];
	fb[0] = new char[3 * WIDTH * HEIGHT];
	fb[1] = new char[3 * WIDTH * HEIGHT];
	char *fb0 = fb[0], *fb1 = fb[1];
	for (int i = 0; i < 3 * WIDTH * HEIGHT; i++) {
		fb0[i] = fb1[i] = 0;
	}

	do
	{
		// fb[current] にレンダリング
		std::this_thread::sleep_for(std::chrono::seconds(1));

		time_t t = time(NULL) - t0;

		// 30 秒ごとに出力
		int c = t / OUTPUT_INTERVAL;
		if (count < c) {
			current = 1 - current;
			char *dest = fb[current];
			char *src = fb[1 - current];
			for (int i = 0; i < 3 * WIDTH * HEIGHT; i++) {
				dest[i] = src[i];
			}
			char filename[256];
			sprintf_s<256>(filename, "%d.png", c);
			save(fb[1 - current], filename);
			count++;
		}

		// 4分33秒以内に終了なので、前のフレームを考えてオーバーしそうならば終了する
		if (FINISH_TIME - 2 <= t + (t - t_last)) break;

		t_last = t;
	}while (true);

	delete[] fb[0];
	delete[] fb[1];

	return 0;
}

